import os, sys, struct, gzip

sys.path.append('./proto_gen')
from tzcp.ssl.vision.messages_robocup_ssl_wrapper_tracked_pb2 import TrackerWrapperPacket
from tzcp.ssl.vision.messages_robocup_ssl_wrapper_pb2 import SSL_WrapperPacket
sys.path.append('../../..')
from rocos.log.tools.logdefine import TYPE
def get_content(filename):
    content = None
    if filename.endswith('.gz'):
        with gzip.open(filename, 'rb') as f:
            content = f.read()
    else:
        with open(filename, 'rb') as f:
            content = f.read()
    return content

class MSG_INDEX:
    TIMESTAMP = 0
    TYPE = 1
    SIZE = 2
    MSG = 3

def read_log(content):
    if content[:12] != b'TZ_SPLIT_LOG':
        print('Not a valid log file')
        return None
    msgs = []
    data = content[12:]
    data_index = 0
    while data_index < len(data):
        timestamp, type, size = struct.unpack('>qii', data[data_index:data_index+16])
        data_index += 16
        msg = data[data_index:data_index+size]
        data_index += size
        msgs.append((timestamp, type, size, msg))
    return msgs, type

def check_log(filename):
    content = get_content(filename)
    msgs, type = read_log(content)

    MsgType = None
    if type == TYPE.SSL_VISION_TRACKER_2020:
        MsgType = TrackerWrapperPacket
    elif type == TYPE.SSL_VISION_2014:
        MsgType = SSL_WrapperPacket
    else:
        print('Unknown message type')
        sys.exit(1)
    for msg in msgs:
        pack = MsgType()
        pack.ParseFromString(msg[3])

        if type == TYPE.SSL_VISION_TRACKER_2020:
            frame_number = pack.tracked_frame.frame_number
            ball_size = pack.tracked_frame.balls.__len__()
            robot_size = pack.tracked_frame.robots.__len__()
            if ball_size != 1 or robot_size != 22:
                print(filename , " - frame_num: ", frame_number, 'ball size:', ball_size, 'robot size:', robot_size)
                # delete file
                os.remove(filename)
                return
    print(filename, ' - OK')
if __name__ == '__main__':
    dir_name = sys.argv[1]
    all_log_files = []
    # get all file in dir_name with walk
    for root, dirs, files in os.walk(dir_name):
        for file in files:
            all_log_files.append(os.path.join(root, file))
    
    for log_file in all_log_files:
        check_log(log_file)