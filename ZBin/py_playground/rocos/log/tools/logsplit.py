import os, sys, struct
from typing import Optional
import gzip
import numpy as np

# File Format
# Each log file starts with the following header:

# 1: String – File type (“SSL_LOG_FILE”) 
# 2: Int32 – Log file format version 

# Format version 1 encodes the protobuf messages in the following format:

# 1: Int64 – Receiver timestamp in ns 
# 2: Int32 – Message type 
# 3: Int32 – Size of binary protobuf message 
# 4: String – Binary protobuf message 

# The message types are:

# MESSAGE_BLANK = 0 (ignore message)
# MESSAGE_UNKNOWN = 1 (try to guess message type by parsing the data)
# MESSAGE_SSL_VISION_2010 = 2
# MESSAGE_SSL_REFBOX_2013 = 3
# MESSAGE_SSL_VISION_2014 = 4
# MESSAGE_SSL_VISION_TRACKER_2020 = 5
# MESSAGE_SSL_INDEX_2021 = 6

from tzcp.ssl.vision.messages_robocup_ssl_wrapper_tracked_pb2 import TrackerWrapperPacket
from tzcp.ssl.vision.messages_robocup_ssl_wrapper_pb2 import SSL_WrapperPacket
from tzcp.ssl.ref.ssl_referee_pb2 import Referee
sys.path.append('../../..')
from rocos.log.tools.logdefine import TYPE
class SplitLog:
    def __init__(self, name, compress=False):
        self.filename = name+".splitlog" + (".gz" if compress else "")
        # get path prefix
        path = os.path.dirname(self.filename)
        if not os.path.exists(path):
            os.makedirs(path)
        self.file = gzip.open(self.filename, 'wb') if compress else open(self.filename, 'wb')
        self.file.write(b'TZ_SPLIT_LOG')
    def write(self,timestamp,type,size,data):
        self.file.write(struct.pack('>qii',timestamp,type,size))
        self.file.write(data)
    def close(self):
        self.file.close()
    def __del__(self):
        self.close()
class LogSplitter:
    class Config:
        def __init__(self):
            self.skip_not_running_stages = True
            self.tracker_source_name_filter = ['TIGERs']
            self.record_ref_commands = [
                Referee.Command.NORMAL_START,
                Referee.Command.FORCE_START,
                Referee.Command.DIRECT_FREE_YELLOW,
                Referee.Command.DIRECT_FREE_BLUE,
            ]
    def __init__(self, config: Config = Config()):
        self.config = config
        self.counter = np.zeros(7, dtype=int)
        self.current_stage = None
        self.current_ref_command = None
        self.current_vision_tracker = None
        self.current_vision = None
        # self.split_vision = None
        self.split_tracker = None
    def new_log(self, filename):
        # if self.split_vision:
        #     self.split_vision.close()
        if self.split_tracker:
            self.split_tracker.close()
        # self.split_vision = SplitLog(filename + '_vision')
        self.split_tracker = SplitLog(filename + '_tracker')
    def parse_msg(self, type, data, timestamp, size):
        msg = None
        if type == TYPE.SSL_REFBOX_2013:
            msg = Referee()
            msg.ParseFromString(data)
            self.current_ref_command = msg.command
            self.current_stage = msg.stage
            pass
        elif type == TYPE.SSL_VISION_2014:
            msg = SSL_WrapperPacket()
            msg.ParseFromString(data)
            self.current_vision = msg
            self.counter[type] += 1
            # self.split_vision.write(timestamp, type, size, data)
            pass
        elif type == TYPE.SSL_VISION_TRACKER_2020:
            msg = TrackerWrapperPacket()
            msg.ParseFromString(data)
            if msg.source_name in self.config.tracker_source_name_filter:
                self.counter[type] += 1
                self.current_vision_tracker = msg
                self.split_tracker.write(timestamp, type, size, data)
            pass
        else:
            # MESSAGE_BLANK
            # MESSAGE_UNKNOWN
            # SSL_VISION_2010
            # SSL_INDEX_2021
            # 'Unknown message type'
            return
    def split(self,filename,store_prefix=None):
        if store_prefix is None:
            store_prefix = os.path.splitext(filename)[0]
        if not os.path.exists(store_prefix):
            os.makedirs(store_prefix)
        with gzip.open(filename, 'rb') as f:
            content = f.read()
        # get header
        header, msgs = content[:16], content[16:]
        # check header
        if header[:12] != b'SSL_LOG_FILE':
            print('Not a valid log file')
            return
        version = struct.unpack('>I', header[12:])[0]
        # read messages
        msg_index = 0
        while msg_index < len(msgs):
            timestamp, msg_type, msg_size = struct.unpack('>qii', msgs[msg_index:msg_index+16])
            msg_index += 16
            if msg_type == 0:
                continue
            msg = msgs[msg_index:msg_index+msg_size]
            msg_index = msg_index + msg_size
            last_ref_command = self.current_ref_command
            if self.current_ref_command not in self.config.record_ref_commands and msg_type != TYPE.SSL_REFBOX_2013:
                continue
            self.parse_msg(msg_type, msg, timestamp, msg_size)
            if self.current_ref_command != last_ref_command and self.current_ref_command in self.config.record_ref_commands:
                if last_ref_command is not None:
                    print(Referee.Command.Name(last_ref_command), '->', Referee.Command.Name(self.current_ref_command))
                path_prefix = os.path.join(store_prefix, str(self.current_stage) + '-' + Referee.Stage.Name(self.current_stage) + '/' +str(timestamp) + '-' + Referee.Command.Name(self.current_ref_command))
                self.new_log(path_prefix)

if __name__ == '__main__':
    splitter = LogSplitter()
    # set name as arg[1]
    log_name = sys.argv[1]
    splitter.split(log_name)
