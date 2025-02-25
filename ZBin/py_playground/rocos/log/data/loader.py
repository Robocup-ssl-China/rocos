import sys
from torch.utils.data import DataLoader

sys.path.append('../../..')
from rocos.log.data.tracker_vision import TrackerVisionDataset

def data_loader(args, path):
    dset = TrackerVisionDataset(
        path,
        obs_len=args.obs_len,
        pred_len=args.pred_len,
        skip=args.skip)

    loader = DataLoader(
        dset,
        batch_size=args.batch_size,
        shuffle=True,
        num_workers=args.loader_num_workers)
    return dset, loader


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--data_dir', type=str, default='__log__')
    parser.add_argument('--obs_len', type=int, default=8)
    parser.add_argument('--pred_len', type=int, default=12)
    parser.add_argument('--skip', type=int, default=5)
    parser.add_argument('--batch_size', type=int, default=32)
    parser.add_argument('--loader_num_workers', type=int, default=4)
    parser.add_argument('--delim', type=str, default=' ')
    args = parser.parse_args()

    dset, loader = data_loader(args, args.data_dir)
    for data in iter(loader):
        print(len(data))
        for d in data:
            print(d.shape)
        break