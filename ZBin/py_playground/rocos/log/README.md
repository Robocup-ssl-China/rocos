# log tools & dataloader
* store xxx.log.gz in `__log/` dir
```bash
python tools/logsplit.py __log/xxx.log.gz # generate split log
python tools/logread.py __log/xxx.log/ # check log
python data/tracker_vision.py __log/ # load all log in __log dir
```
