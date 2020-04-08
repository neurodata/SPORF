import mne
import numpy as np
from mne_bids import read_raw_bids
from mne_bids.tsv_handler import _from_tsv
from mne_bids.utils import _find_matching_sidecar


def get_trial_info(bids_fname, bids_root):
    """Get behavior and events trial info from tsv files."""
    behav_fpath = _find_matching_sidecar(bids_fname, bids_root, suffix='behav.tsv')
    behav_tsv = _from_tsv(behav_fpath)

    events_fpath = _find_matching_sidecar(bids_fname, bids_root, suffix='events.tsv')
    events_tsv = _from_tsv(events_fpath)

    # successful trial indices
    success_inds = np.where(behav_tsv['successful_trial_flag'] == 1)[0]
    num_trials = len(behav_tsv['trial_id'])
    print(f"Out of {num_trials} trials, there were {len(success_inds)} successful trials "
          f"in {bids_fname}.")

    return behav_tsv, events_tsv


def _read_ch_anat(bids_fname, bids_root):
    electrodes_fpath = _find_matching_sidecar(bids_fname, bids_root, suffix='electrodes.tsv')
    electrodes_tsv = _from_tsv(electrodes_fpath)

    # extract channel names and anatomy
    ch_names = electrodes_tsv['ch_names']
    ch_anat = electrodes_tsv['anat']

    # create dictionary of ch name to anatomical region
    ch_anat_dict = {name: anat for name, anat in zip(ch_names, ch_anat)}
    return ch_anat_dict


def _get_maximum_trial_length(events_tsv):
    max_trial_len = _get_trial_length_by_kwarg(events_tsv, 'Reserved (Start Trial)', 'Reserved (End Trial)')
    return max_trial_len


def _get_trial_length_by_kwarg(events_tsv, start_trial_type, stop_trial_type):
    start_trial_inds = [i for i, x in enumerate(events_tsv['trial_type']) if x == start_trial_type]
    end_trial_inds = [i for i, x in enumerate(events_tsv['trial_type']) if x == stop_trial_type]

    assert len(start_trial_inds) == len(end_trial_inds)

    max_trial_len = 0
    trial_lens = []
    for i, (start_ind, stop_ind) in enumerate(zip(start_trial_inds, end_trial_inds)):
        start_sample = events_tsv['sample'][start_ind]
        stop_sample = events_tsv['sample'][stop_ind]
        trial_sample_len = stop_sample - start_sample
        trial_lens.append(trial_sample_len)
        max_trial_len = max(max_trial_len, trial_sample_len)

    if any(max_trial_len > x for x in trial_lens):
        raise RuntimeError(f"Trial length between {start_trial_type} and {stop_trial_type} "
                           f"is too strict. If we hard code a max trial length of {max_trial_len} "
                           f"samples, then we will cut into other trials. Please choose other "
                           f"start and stop keywords.")

    return max_trial_len


def read_trial(bids_fname, bids_root, trial_id):
    """Read Raw from specific trial id."""
    raw = read_raw_bids(bids_fname, bids_root)

    # get the channel anat dict
    ch_anat_dict = _read_ch_anat(bids_fname, bids_root)

    # get bad channels from anatomy
    bads = []
    for ch_name, anat in ch_anat_dict.items():
        if anat in ['out',
                    'white matter',
                    'cerebrospinal fluid'] or 'ventricle' in anat:
            bads.append(ch_name)
    raw.info['bads'].extend(bads)
    good_chs = [ch for ch in raw.ch_names if ch not in raw.info['bads']]

    # get trial information
    behav_tsv, events_tsv = get_trial_info(bids_fname, bids_root)

    # trial_info from behav tsv
    behav_trial_ind = behav_tsv['trial_id'].index(int(trial_id))
    trial_metadata = dict()
    for key, values in behav_tsv.items():
        trial_metadata[key] = values[behav_trial_ind]

    # get trial events
    start_trial_inds = [i for i, x in enumerate(events_tsv['trial_type']) if x == 'Reserved (Start Trial)']
    end_trial_inds = [i for i, x in enumerate(events_tsv['trial_type']) if x == 'Reserved (End Trial)']
    assert len(start_trial_inds) == len(behav_tsv['trial_id'])  # number of trials should match

    # get the rows of the start/end of the trial
    event_trialstart_ind = start_trial_inds[behav_trial_ind]
    event_trialend_ind = end_trial_inds[behav_trial_ind]

    # trim raw to between those
    start = events_tsv['sample'][event_trialstart_ind]
    stop = events_tsv['sample'][event_trialend_ind]

    _events_tsv = dict()
    for key, values in events_tsv.items():
        _events_tsv[key] = [values[i] for i in range(event_trialstart_ind, event_trialend_ind + 1)]
    events_tsv = _events_tsv

    # load the data for this trial
    rawdata, times = raw.get_data(picks=good_chs, start=start, stop=stop, return_times=True)

    return rawdata, times, _events_tsv


def read_dataset(bids_fname, bids_root):
    """Read entire dataset as an Epoch."""
    # read in the dataset from mnebids
    raw = read_raw_bids(bids_fname, bids_root)

    # get trial information
    _, events_tsv = get_trial_info(bids_fname, bids_root)

    # get the channel anat dict
    ch_anat_dict = _read_ch_anat(bids_fname, bids_root)
    # get bad channels from anatomy
    bads = []
    for ch_name, anat in ch_anat_dict.items():
        if anat in ['out',
                    'white matter',
                    'cerebrospinal fluid'] or 'ventricle' in anat:
            bads.append(ch_name)
    raw.info['bads'].extend(bads)
    good_chs = [ch for ch in raw.ch_names if ch not in raw.info['bads']]

    # get the events and events id structure
    events, event_id = mne.events_from_annotations(raw)
    # event_id = event_id['Reserved (Start Trial)']  # time lock to the event id for Start Trial
    event_id = event_id['show card']

    tmax = _get_trial_length_by_kwarg(events_tsv,
                                      start_trial_type='show card',
                                      stop_trial_type='hide reward')

    # get the epochs
    epochs = mne.Epochs(raw, events, event_id,
                        picks=good_chs, tmax=tmax)
    print(epochs.shape)

    return epochs
