"""API for converting files to BIDS format."""
import collections
import datetime
import json
import os
from enum import Enum

import h5py
import mne
import numpy as np
import pandas as pd
import scipy.io
from mne_bids import make_bids_folders
from mne_bids.tsv_handler import _to_tsv
from mne_bids.utils import _parse_bids_filename


class MatReader():
    '''
    Object to read mat files into a nested dictionary if need be.
    Helps keep strucutre from matlab similar to what is used in python.
    '''

    def __init__(self, filename=None):
        self.filename = filename

    def loadmat(self, filename):
        '''
        this function should be called instead of direct spio.loadmat
        as it cures the problem of not properly recovering python dictionaries
        from mat files. It calls the function check keys to cure all entries
        which are still mat-objects
        '''
        # data = scipy.io.loadmat(
        #     filename,
        #     struct_as_record=False,
        #     squeeze_me=True)

        # load in setup.mat via scipy
        try:
            data = scipy.io.loadmat(filename, struct_as_record=False,
                                    squeeze_me=True)
        except:
            #     setup_dict = mat73.loadmat(_get_setup_fname(source_fpath))
            # finally:
            data = read_matlab(filename)

        return self._check_keys(data)

    def _check_keys(self, dict):
        '''
        checks if entries in dictionary are mat-objects. If yes
        todict is called to change them to nested dictionaries
        '''
        for key in dict:
            if isinstance(dict[key], scipy.io.matlab.mio5_params.mat_struct):
                dict[key] = self._todict(dict[key])
        return dict

    def _todict(self, matobj):
        '''
        A recursive function which constructs from matobjects nested dictionaries
        '''
        dict = {}
        for strg in matobj._fieldnames:
            elem = matobj.__dict__[strg]
            if isinstance(elem, scipy.io.matlab.mio5_params.mat_struct):
                dict[strg] = self._todict(elem)
            elif isinstance(elem, np.ndarray):
                dict[strg] = self._tolist(elem)
            else:
                dict[strg] = elem
        return dict

    def _tolist(self, ndarray):
        '''
        A recursive function which constructs lists from cellarrays
        (which are loaded as numpy ndarrays), recursing into the elements
        if they contain matobjects.
        '''
        elem_list = []
        for sub_elem in ndarray:
            if isinstance(sub_elem, scipy.io.matlab.mio5_params.mat_struct):
                elem_list.append(self._todict(sub_elem))
            elif isinstance(sub_elem, np.ndarray):
                elem_list.append(self._tolist(sub_elem))
            else:
                elem_list.append(sub_elem)
        return elem_list


def read_matlab(filename):
    def conv(path=''):
        p = path or '/'
        paths[p] = ret = {}
        for k, v in f[p].items():
            if type(v).__name__ == 'Group':
                ret[k] = conv(f'{path}/{k}')  # Nested struct
                continue
            v = v[()]  # It's a Numpy array now
            if v.dtype == 'object':
                # HDF5ObjectReferences are converted into a list of actual pointers
                ret[k] = [r and paths.get(f[r].name, f[r].name) for r in v.flat]
            else:
                # Matrices and other numeric arrays
                ret[k] = v if v.ndim < 2 else v.swapaxes(-1, -2)
        return ret

    paths = {}
    with h5py.File(filename, 'r') as f:
        return conv()


class CHANNEL_MARKERS(Enum):
    # non-eeg markers
    NON_EEG_MARKERS = [
        "DC",
        "EKG",
        "REF",
        "EMG",
        "ECG",
        "EVENT",
        "MARK",
        "STI014",
        "STIM",
        "STI",
        "RFC",
    ]
    # bad marker channel names
    BAD_MARKERS = ["$", "FZ", "GZ", "DC", "STI"]


def _get_misc_fname(fname) -> str:
    return str(fname).replace("Raw", "Misc")


def _get_setup_fname(fname) -> str:
    return str(fname).replace("Raw", "Setup")


def get_xy_fname(fname) -> str:
    return str(fname).replace("Raw", "XY")


def _compute_durations(trial_time_description):
    durations = []
    for i, description in enumerate(trial_time_description):
        description = description.lower()
        if 'show card' in description:
            durations.append(2)
        elif 'show reward' in description:
            durations.append(1.3)
        else:
            durations.append(0)
    return durations


def _set_channel_types(raw: mne.io.BaseRaw, verbose: bool) -> mne.io.BaseRaw:
    """Set channel types in raw using rules.

    Parameters
    ----------
    raw : mne.io.BaseRaw
    verbose : bool

    Returns
    -------
    raw : mne.io.BaseRaw
    """
    # set DC channels -> MISC for now
    picks = mne.pick_channels_regexp(raw.ch_names, regexp="DC|[$]")
    raw.set_channel_types(
        {raw.ch_names[pick]: "misc" for pick in picks}, verbose=verbose
    )
    # set channels named "E" to miscellaneous
    if "E" in raw.ch_names:
        raw.set_channel_types({"E": "misc"}, verbose=verbose)

    # set vagal nerve stimulation channels to 'bio'
    if "VNS" in raw.ch_names:
        raw.set_channel_types({"VNS": "bio"})

    # set bio channels (e.g. EKG, EMG, EOG)
    picks = mne.pick_channels_regexp(raw.ch_names, regexp="EKG|ECG")
    raw.set_channel_types(
        {raw.ch_names[pick]: "ecg" for pick in picks}, verbose=verbose
    )
    picks = mne.pick_channels_regexp(raw.ch_names, regexp="EMG")
    raw.set_channel_types(
        {raw.ch_names[pick]: "emg" for pick in picks}, verbose=verbose
    )
    picks = mne.pick_channels_regexp(raw.ch_names, regexp="EOG")
    raw.set_channel_types(
        {raw.ch_names[pick]: "eog" for pick in picks}, verbose=verbose
    )

    # set non-eeg channels leftover as 'misc'
    for chan in raw.ch_names:
        if any([x in chan for x in CHANNEL_MARKERS.NON_EEG_MARKERS.value]):
            raw.set_channel_types({chan: "misc"})

    return raw


def _map_trial_words_to_description(event_ids, task, source_dir):
    if task == 'war':
        mapping = {
            9: "Reserved (Start Trial)",
            18: "Reserved (End Trial)",
            30: "fixation",
            31: "show card",
            32: "show bet",
            33: "acquire target",
            34: "did not hold target",
            35: "start move",
            39: 'bet 5',
            40: 'bet 20',
            41: 'win 5',
            42: 'win 20',
            43: 'lose 5',
            44: 'lose 20',
            45: 'draw',
            49: 'did not acquire fix',
            50: 'did not respond',
            51: 'show card results',
            52: 'show reward',
            53: 'hide reward',
            54: 'show fail',
            55: 'hide fail',
            56: 'show reward1',
            57: 'show reward2',
        }
    elif task == 'move':
        mapping_df = pd.read_excel(os.path.join(source_dir, 'code_EFRI_Move_Task.xls'),
                                   index_col=False)
        mapping_dict = mapping_df.to_dict(orient='index')
        mapping = dict()
        # for code, description in zip(mapping_dict['Code'], mapping_dict['Description']):
        for index, code_dict in mapping_dict.items():
            mapping[code_dict['Code']] = code_dict['Description']

    descriptions = [mapping[int(event_id)] for event_id in event_ids]
    return descriptions


def _create_behavior_tsv(fname, descriptions, **kwargs):
    # behavior_dict = collections.OrderedDict({
    #     'trial_id': trial_ids,
    #     'subject_card': subject_cards,
    #     'computer_card': computer_cards,
    #     'subject_win_result': results,
    #     'reaction_time': rt,
    #     'fixation_time': ft,
    #     'bet_amount': bet_amounts,
    #     'successful_trial_flag': success_trial_flag
    # })
    # descriptions = [
    #     'trial identifier',
    #     'card that subject received (2, 4, 6, 8, 10)',
    #     'card that computer received (2, 4, 6, 8, 10)',
    #     'outcome of WAR (-1=loss, 0=tie, 1=win)',
    #     'reaction time of subject to make bet',
    #     'time it took for subject to fixate before trial starts',
    #     'amount bet ($5, or $20)',
    #     'successful completion of the trial (T/F)'
    # ]
    if len(descriptions) != len(kwargs):
        raise RuntimeError("Need to have same number of descriptions "
                           "as behaviors described.")

    behavior_dict = collections.OrderedDict(kwargs)
    _to_tsv(behavior_dict, fname)

    json_fname = fname.replace('tsv', 'json')
    _create_behavior_json(json_fname, behavior_dict.keys(), descriptions)


def _create_behavior_json(fname, keys, desciptions):
    behavior_dict = {key: description for key, description in zip(keys, desciptions)}
    with open(fname, 'w') as fout:
        json.dump(behavior_dict, fout)
    return fname


def _create_electrodes_tsv(source_fpath, bids_basename, bids_root):
    # load in setup.mat via scipy
    try:
        setup_dict = scipy.io.loadmat(_get_setup_fname(source_fpath))
    except:
        print("Using hdf to read...")
        loader = MatReader()
        setup_dict = loader.loadmat(_get_setup_fname(source_fpath))

    # lambda function to flatten out matlab structureees
    flatten = lambda l: np.array([item for sublist in l for item in sublist]).squeeze()

    # extract names
    ch_names = flatten(setup_dict['elec_name'])
    ch_anat = flatten(setup_dict['elec_area'])

    print("channels names: ", ch_names[0:5])
    print("channel anatomy: ", ch_anat[0:5])

    # create behavior tsv and json sidecar files
    params = _parse_bids_filename(bids_basename, False)
    acquisition = params['acq']
    if acquisition in ['seeg', 'ecog']:
        kind = 'ieeg'
    datadir = make_bids_folders(subject=params['sub'], session=params['ses'],
                                kind=kind,
                                bids_root=bids_root, make_dir=False)
    electrodes_fpath = os.path.join(datadir, bids_basename + "_electrodes.tsv")

    x = ['n/a'] * len(ch_anat)
    y = ['n/a'] * len(ch_anat)
    z = ['n/a'] * len(ch_anat)
    sizes = ['n/a'] * len(ch_anat)
    data = collections.OrderedDict([('name', ch_names),
                                    ('x', x), ('y', y), ('z', z), ('size', sizes),
                                    ('anat', ch_anat)])

    _to_tsv(data, electrodes_fpath)


def _convert_mat_to_raw(source_fpath) -> mne.io.BaseRaw:
    # load in the dataset via scipy
    # data_dict = scipy.io.loadmat(source_fpath)
    data_dict = read_matlab(source_fpath)

    # load in setup.mat via scipy
    try:
        setup_dict = scipy.io.loadmat(_get_setup_fname(source_fpath))
    except:
        print("Using hdf to read...")
        loader = MatReader()
        setup_dict = loader.loadmat(_get_setup_fname(source_fpath))

    # lambda function to flatten out matlab structureees
    flatten = lambda l: np.array([item for sublist in l for item in sublist]).squeeze()

    # extract names
    ch_names = list(flatten(setup_dict['elec_name']))

    rawdata = data_dict['lfpdata']
    sfreq = data_dict['Fs']
    # ch_names = flatten(data_dict['infos']['channels']['name'])

    print("These are the channel names: ", ch_names[0:4])
    if rawdata.shape[1] == len(ch_names):
        rawdata = rawdata.T

    # get the raw Array
    info = mne.create_info(ch_names=ch_names, sfreq=sfreq, ch_types='seeg')
    raw = mne.io.RawArray(rawdata, info=info)

    # set a measurement date to allow anonymization to run
    raw.set_meas_date(meas_date=datetime.datetime.now(tz=datetime.timezone.utc))
    mne.io.anonymize_info(raw.info)
    raw.info['line_freq'] = 60

    return raw


def _convert_trial_info_war(source_fpath, bids_basename, bids_root, raw):
    # load in setup.mat via scipy
    try:
        setup_dict = scipy.io.loadmat(_get_setup_fname(source_fpath))
    except Exception as e:
        # setup_dict = mat73.loadmat(_get_setup_fname(source_fpath))
        # finally:
        print("Using hdf to read...")
        # setup_dict = read_matlab(_get_setup_fname(source_fpath))
        loader = MatReader()
        setup_dict = loader.loadmat(_get_setup_fname(source_fpath))

    # lambda function to flatten out matlab structureees
    flatten = lambda l: np.array([item for sublist in l for item in sublist]).squeeze()

    def _convert_to_list(trial_dict):
        onset_times = []
        for _trial_t in trial_dict:
            for _time in _trial_t:
                for _t in _time:
                    onset_times.append(_t[0])
        return onset_times

    # convert all event times and markers into a sequential list
    onset_times = _convert_to_list(setup_dict['trial_times'])
    event_ids = _convert_to_list(setup_dict['trial_words'])
    # map trial words to description
    params = _parse_bids_filename(bids_basename, False)
    task = params['task']
    source_dir = os.path.join(bids_root, "sourcedata")
    descriptions = _map_trial_words_to_description(event_ids, task, source_dir)

    if any(len(onset_times) != len(x) for x in [event_ids, descriptions]):
        raise RuntimeError("Event times, IDs and descriptions should be the same length.")

    # extract trial filters
    event_filters = setup_dict['filters']
    bet_amounts = flatten(event_filters['bets'])
    success_trial_flag = flatten(event_filters['success'])
    subject_cards = flatten(event_filters['card1'])  # subject's card
    computer_cards = flatten(event_filters['card2'])  # computer's card
    trial_ids = flatten(event_filters['trial'])
    results = flatten(event_filters['result'])
    rt = flatten(event_filters['RT'])
    ft = flatten(event_filters['FT'])

    if any(len(bet_amounts) != len(x) for x in [success_trial_flag, subject_cards, computer_cards,
                                                trial_ids, results, rt, ft]):
        raise RuntimeError("All trial information should be the same length. "
                           f"There should be {len(bet_amounts)} trials.")

    # create behavior tsv and json sidecar files
    params = _parse_bids_filename(bids_basename, False)
    acquisition = params['acq']
    if acquisition in ['seeg', 'ecog']:
        kind = 'ieeg'
    datadir = make_bids_folders(subject=params['sub'], session=params['ses'],
                                kind=kind,
                                bids_root=bids_root, make_dir=True, overwrite=False)
    behavior_fname = os.path.join(datadir, bids_basename + "_behav.tsv")

    behav_descriptions = [
        'trial identifier',
        'card that subject received (2, 4, 6, 8, 10)',
        'card that computer received (2, 4, 6, 8, 10)',
        'outcome of WAR (-1=loss, 0=tie, 1=win)',
        'reaction time of subject to make bet',
        'time it took for subject to fixate before trial starts',
        'amount bet ($5, or $20)',
        'successful completion of the trial (T/F)'
    ]
    _create_behavior_tsv(behavior_fname, behav_descriptions,
                         trial_id=trial_ids,
                         subject_card=subject_cards,
                         computer_card=computer_cards,
                         results=results,
                         reaction_time=rt,
                         fixation_time=ft,
                         bet_amount=bet_amounts,
                         successful_trial_flag=success_trial_flag)

    # determine channel types
    raw = _set_channel_types(raw, False)

    # compute durations
    durations = [0] * len(descriptions)

    # get events over the trials
    my_annot = mne.Annotations(
        onset=onset_times,
        duration=durations,
        description=descriptions)
    raw.set_annotations(my_annot)

    return raw


def _convert_trial_info_move(source_fpath, bids_basename, bids_root, raw):
    # load in setup.mat via scipy
    try:
        setup_dict = scipy.io.loadmat(_get_setup_fname(source_fpath))
    except Exception as e:
        # setup_dict = mat73.loadmat(_get_setup_fname(source_fpath))
        # finally:
        print("Using hdf to read...")
        # setup_dict = read_matlab(_get_setup_fname(source_fpath))
        loader = MatReader()
        setup_dict = loader.loadmat(_get_setup_fname(source_fpath))

    # lambda function to flatten out matlab structureees
    flatten = lambda l: np.array([item for sublist in l for item in sublist]).squeeze()

    def _convert_to_list(trial_dict):
        onset_times = []
        for _trial_t in trial_dict:
            for _time in _trial_t:
                for _t in _time:
                    onset_times.append(_t[0])
        return onset_times

    # convert all event times and markers into a sequential list
    onset_times = _convert_to_list(setup_dict['trial_times'])
    event_ids = _convert_to_list(setup_dict['trial_words'])
    # map trial words to description
    params = _parse_bids_filename(bids_basename, False)
    task = params['task']
    source_dir = os.path.join(bids_root, "sourcedata")
    descriptions = _map_trial_words_to_description(event_ids, task, source_dir)

    if any(len(onset_times) != len(x) for x in [event_ids, descriptions]):
        raise RuntimeError("Event times, IDs and descriptions should be the same length.")

    # extract trial filters
    event_filters = setup_dict['filters']
    success_trial_flag = flatten(event_filters['success'])
    trial_ids = flatten(event_filters['trial'])
    rt = flatten(event_filters['RT'])
    nrt = flatten(event_filters['nRT'])
    rrt = flatten(event_filters['rRT'])
    missed_target = flatten(event_filters['MissedTarget'])
    correct_speed = flatten(event_filters['CorrectSpeed'])
    force_ang = flatten(event_filters['Force_Ang'])
    force_mag = flatten(event_filters['Force_Mag'])
    target_direction = flatten(event_filters['TargetDirection'])
    x_pos = flatten(event_filters['X_Pos'])
    y_pos = flatten(event_filters['Y_Pos'])

    if any(len(trial_ids) != len(x) for x in [success_trial_flag,
                                              rt, nrt, rrt,
                                              missed_target, correct_speed, force_ang, force_mag,
                                              target_direction, x_pos, y_pos
                                              ]):
        raise RuntimeError("All trial information should be the same length. "
                           f"There should be {len(trial_ids)} trials.")

    print(len(rt), rt.shape, )
    print(rt[0:2], nrt[0:2])
    print(success_trial_flag[0:2])
    print(x_pos[0:5], y_pos[0:5])

    # create behavior tsv and json sidecar files
    params = _parse_bids_filename(bids_basename, False)
    acquisition = params['acq']
    if acquisition in ['seeg', 'ecog']:
        kind = 'ieeg'
    datadir = make_bids_folders(subject=params['sub'], session=params['ses'],
                                kind=kind,
                                bids_root=bids_root, make_dir=True, overwrite=False)
    behavior_fname = os.path.join(datadir, bids_basename + "_behav.tsv")

    kwargs = collections.OrderedDict({
        'trial_id': trial_ids,
        'successful_trial_flag': success_trial_flag,
        'reaction_time': rt,
        'n_reaction_time': nrt,
        'r_reaction_time': rrt,
        'missed_target_flag': missed_target,
        'correct_speed_flag': correct_speed,
        'force_angular': force_ang,
        'force_magnitude': force_mag,
        'target_direction': target_direction,
        'x_position': x_pos,
        'y_position': y_pos
    })
    behav_descriptions = [
        'trial identifier',
        'successful completion of the trial (T/F)',
        'reaction time of subject to make bet',
        'n reaction time of subject to make bet',
        'r reaction time of subject to make bet',
        'if subject missed target',
        'if subject reached correct speed',
        'angular force',
        'magnitude of force',
        'target direction (0, 1, 2, 3)',
        'x position of goal',
        'y position of goal',
    ]
    _create_behavior_tsv(behavior_fname, behav_descriptions, **kwargs)

    # compute durations
    durations = [0] * len(onset_times)

    # get events over the trials
    my_annot = mne.Annotations(
        onset=onset_times,
        duration=durations,
        description=descriptions)
    raw.set_annotations(my_annot)

    # determine channel types
    raw = _set_channel_types(raw, False)

    return raw
