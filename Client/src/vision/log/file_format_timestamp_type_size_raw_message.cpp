#include "file_format_timestamp_type_size_raw_message.h"
#include <QDebug>

FileFormatTimestampTypeSizeRawMessage::FileFormatTimestampTypeSizeRawMessage() :
    FileFormat(1)
{

}

bool FileFormatTimestampTypeSizeRawMessage::readHeaderFromStream(QDataStream& stream)
{
    char name[13];
    name[12] = '\0';
    stream.readRawData(name, sizeof(name) - 1);

    qint32 version;
    stream >> version;

    if (QString(name) == "SSL_LOG_FILE" && version == this->version()) {
        return true;
    }

    return false;
}

bool FileFormatTimestampTypeSizeRawMessage::readMessageFromStream(QDataStream& stream, QByteArray& data, qint64& time, MessageType& type)
{
    stream >> time;
    qint32 typeValue;
    stream >> typeValue;
    type = (MessageType) typeValue;
    stream >> data;

    return true;
}
