import os
from omegaconf import OmegaConf
def get_config(name="default"):
    path = []
    if os.path.isabs(name):
        path.append(os.path.dirname(name))
    else:
        path.append(os.path.dirname(__file__))
        path.append(os.getcwd())
        path.append(os.path.join(os.path.expanduser("~"), ".config", "rocos"))
    for p in path:
        try:
            cfg = OmegaConf.load(os.path.join(p, name + ".yaml"))
            break
        except FileNotFoundError:
            cfg = None
    if cfg is None:
        raise FileNotFoundError(f"Config file {name}.yaml not found in {path}")
    return cfg

if __name__ == "__main__":
    cfg = get_config("default")
    print(cfg)