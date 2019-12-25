#ifndef FILE_FORMAT_TIMESTAMP_TYPE_SIZE_RAW_MESSAGE_H
#define FILE_FORMAT_TIMESTAMP_TYPE_SIZE_RAW_MESSAGE_H

#include "file_format.h"

class FileFormatTimestampTypeSizeRawMessage : public FileFormat
{
public:
    FileFormatTimestampTypeSizeRawMessage();
    bool readHeaderFromStream(QDataStream &stream);
    bool readMessageFromStream(QDataStream &stream, QByteArray &data, qint64 &time, MessageType &type);
};

#endif // FILE_FORMAT_TIMESTAMP_TYPE_SIZE_RAW_MESSAGE_H
