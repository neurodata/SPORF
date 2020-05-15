import mne
import numpy as np
from mne_bids import read_raw_bids
from mne_bids.tsv_handler import _from_tsv
from mne_bids.utils import _find_matching_sidecar


def get_trial_info(bids_fname, bids_root):
    """Get behavior and events trial info from tsv files."""
    behav_fpath = _find_matching_sidecar(bids_fname, bids_root, suffix="behav.tsv")
    behav_tsv = _from_tsv(behav_fpath)

    events_fpath = _find_matching_sidecar(bids_fname, bids_root, suffix="events.tsv")
    events_tsv = _from_tsv(events_fpath)

    success_trial_flag = np.array(list(map(int, behav_tsv["successful_trial_flag"])))

    # successful trial indices
    success_inds = np.where(success_trial_flag == 1)[0]
    num_trials = len(behav_tsv["trial_id"])
    print(
        f"Out of {num_trials} trials, there were {len(success_inds)} successful trials "
        f"in {bids_fname}."
    )

    return behav_tsv, events_tsv


def _read_ch_anat(bids_fname, bids_root):
    # electrodes_fpath = _find_matching_sidecar(
    #     bids_fname, bids_root, suffix="electrodes.tsv"
    # )
    electrodes_fpath = _find_matching_sidecar(
        bids_fname, bids_root, suffix="channels.tsv"
    )
    electrodes_tsv = _from_tsv(electrodes_fpath)

    # extract channel names and anatomy
    ch_names = electrodes_tsv["name"]
    ch_anat = electrodes_tsv["anat"]

    # create dictionary of ch name to anatomical region
    ch_anat_dict = {name: anat for name, anat in zip(ch_names, ch_anat)}
    return ch_anat_dict


def _get_maximum_trial_length(events_tsv):
    max_trial_len = _get_trial_length_by_kwarg(
        events_tsv, "Reserved (Start Trial)", "Reserved (End Trial)"
    )
    return max_trial_len


def _get_trial_length_by_kwarg(
    events_tsv, start_trial_type, stop_trial_type, successful_trials
):
    start_trial_inds = [
        i
        for i, x in enumerate(events_tsv["trial_type"])
        if x.lower() == start_trial_type.lower()
        if i in successful_trials
    ]
    end_trial_inds = [
        i
        for i, x in enumerate(events_tsv["trial_type"])
        if x.lower() == stop_trial_type.lower()
        if i in successful_trials
    ]

    print(len(start_trial_inds), len(end_trial_inds))
    assert len(start_trial_inds) == len(end_trial_inds)

    max_trial_len = 0
    trial_lens = []
    for i, (start_ind, stop_ind) in enumerate(zip(start_trial_inds, end_trial_inds)):
        start_sample = int(events_tsv["sample"][start_ind])
        stop_sample = int(events_tsv["sample"][stop_ind])
        trial_sample_len = stop_sample - start_sample
        trial_lens.append(trial_sample_len)
        max_trial_len = max(max_trial_len, trial_sample_len)

    if any(max_trial_len > x for x in trial_lens):
        raise RuntimeError(
            f"Trial length between {start_trial_type} and {stop_trial_type} "
            f"is too strict. If we hard code a max trial length of {max_trial_len} "
            f"samples, then we will cut into other trials. Please choose other "
            f"start and stop keywords."
        )

    return max_trial_len


def _get_bad_chs(bids_fname, bids_root):
    # get the channel anat dict
    ch_anat_dict = _read_ch_anat(bids_fname, bids_root)

    # get bad channels from anatomy
    bads = []
    for ch_name, anat in ch_anat_dict.items():
        if (
            anat in ["out", "white matter", "cerebrospinal fluid"]
            or "ventricle" in anat
        ):
            bads.append(ch_name)
    return bads


def read_trial(bids_fname, bids_root, trial_id, notch_filter=False, picks=None):
    """Read Raw from specific trial id."""
    raw = read_raw_bids(bids_fname, bids_root)

    bads = _get_bad_chs(bids_fname, bids_root)
    raw.info["bads"].extend(bads)
    
    if picks is None:
        good_chs = [ch for ch in raw.ch_names if ch not in raw.info["bads"]]
    else:
        good_chs = [ch for ch in raw.ch_names if ch in picks and ch not in raw.info["bads"]]

    # get trial information
    behav_tsv, events_tsv = get_trial_info(bids_fname, bids_root)

    # trial_info from behav tsv
    behav_trial_ind = behav_tsv["trial_id"].index(str(trial_id))
    trial_metadata = dict()
    for key, values in behav_tsv.items():
        trial_metadata[key] = values[behav_trial_ind]

    # get trial events
    start_trial_inds = [
        i
        for i, x in enumerate(events_tsv["trial_type"])
        if x == "Reserved (Start Trial)"
    ]
    end_trial_inds = [
        i for i, x in enumerate(events_tsv["trial_type"]) if x == "Reserved (End Trial)"
    ]
    assert len(start_trial_inds) == len(
        behav_tsv["trial_id"]
    )  # number of trials should match

    # get the rows of the start/end of the trial
    event_trialstart_ind = start_trial_inds[behav_trial_ind]
    event_trialend_ind = end_trial_inds[behav_trial_ind]

    # trim raw to between those
    start = int(events_tsv["sample"][event_trialstart_ind])
    stop = int(events_tsv["sample"][event_trialend_ind])

    _events_tsv = dict()
    for key, values in events_tsv.items():
        _events_tsv[key] = [
            values[i] for i in range(event_trialstart_ind, event_trialend_ind + 1)
        ]
    events_tsv = _events_tsv

    # load the data for this trial
    rawdata, times = raw.get_data(
        picks=good_chs, start=start, stop=stop, return_times=True
    )

    if notch_filter:
        fs = raw.info["sfreq"]
        rawdata = mne.filter.notch_filter(rawdata, fs, np.arange(60, fs / 2, 60))

    return rawdata, times, events_tsv


def read_label(bids_fname, bids_root, trial_id=None, label_keyword="bet_amount"):
    """Read trial's label"""
    # get trial information
    behav_tsv, events_tsv = get_trial_info(bids_fname, bids_root)
    trial_ids = behav_tsv["trial_id"]

    if trial_id is not None:
        # trial_info from behav tsv
        behav_trial_ind = trial_ids.index(int(trial_id))
        trial_metadata = dict()
        for key, values in behav_tsv.items():
            trial_metadata[key] = values[behav_trial_ind]

        y = trial_metadata[label_keyword]
    else:
        y = behav_tsv[label_keyword]

    y = np.array(y).astype(float)

    return y, trial_ids


def read_dataset(bids_fname, bids_root, tmin=-0.2, tmax=0.5, picks=None):
    """Read entire dataset as an Epoch."""
    # read in the dataset from mnebids
    raw = read_raw_bids(bids_fname, bids_root)

    # get trial information
    behav_tsv, events_tsv = get_trial_info(bids_fname, bids_root)

    # get bad channels
    bads = _get_bad_chs(bids_fname, bids_root)
    raw.info["bads"].extend(bads)
    
    if picks is None:
        good_chs = [ch for ch in raw.ch_names if ch not in raw.info["bads"]]
    else:
        good_chs = [ch for ch in raw.ch_names if ch in picks and ch not in raw.info["bads"]]

    # get the events and events id structure
    events, event_id = mne.events_from_annotations(raw)
    # event_id = event_id['Reserved (Start Trial)']  # time lock to the event id for Start Trial
    event_id = event_id["show card"]  # Change time locked event

    success_trial_flag = np.array(list(map(int, behav_tsv["successful_trial_flag"])))
    # successful trial indices
    successful_trial_inds = np.where(success_trial_flag == 1)[0]

    # print(len(successful_trial_inds))
    # print(successful_trial_inds)
    # tmax = _get_trial_length_by_kwarg(
    #     events_tsv, start_trial_type="show card", stop_trial_type="show card results", successful_trials=successful_trial_inds
    # )

    # get the epochs
    epochs = mne.Epochs(raw, events, event_id, tmin=tmin, tmax=tmax, picks=good_chs)

    return epochs


def train_test_split():
    """Cross-validation expeeriments."""
    from sklearn.model_selection import train_test_split, KFold

    pass


if __name__ == "__main__":
    from pathlib import Path
    from mne_bids import make_bids_basename

    # bids identifiers
    bids_root = Path("/Users/adam2392/Dropbox/efri/")
    # bids_root = Path("/workspaces/research/data/efri/")
    bids_root = Path("/Users/ChesterHuynh/OneDrive - Johns Hopkins/research/data/efri/")
    deriv_path = Path(bids_root / "derivatives")

    # subject identifiers
    subject = "efri06"
    session = "efri"
    task = "war"
    acquisition = "seeg"
    run = "01"

    kind = "ieeg"
    trial_id = 2

    # bids filename
    bids_fname = make_bids_basename(
        subject=subject,
        session=session,
        task=task,
        acquisition=acquisition,
        run=run,
        suffix=f"{kind}.vhdr",
    )

    # read a specific trial
    rawdata, times, events_tsv = read_trial(bids_fname, bids_root, trial_id)

    # get the label of this trial
    y, trial_ids = read_label(
        bids_fname, bids_root, trial_id=None, label_keyword="bet_amount"
    )
    unsuccessful_trial_inds = np.where(np.isnan(y))[
        0
    ]  # get unsuccessful trials based on keyword label

    # read dataset as an epoch
    epochs = read_dataset(bids_fname, bids_root)  # N x C x T' flatten to # N x (C x T')
    epochs = epochs.drop(unsuccessful_trial_inds)
    epochs.load_data()
    epochs_data = epochs.get_data()

    print("Selected events in events.tsv: ", epochs.selection)
    print("Number of epochs: ", len(epochs))
    print("Data shape: ", epochs_data.shape)

    print(len(y))
    print("Unique labels (e.g. bet amounts): ", np.unique(y))

    print("Specific trial: ", rawdata.shape)
    print("Events data structure for specific trial: ", events_tsv)
    print(bids_fname)
    print(times[0:5])
