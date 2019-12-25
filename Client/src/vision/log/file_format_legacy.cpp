#include "file_format_legacy.h"
#include <QDebug>

FileFormatLegacy::FileFormatLegacy() :
    FileFormat(0)
{

}

bool FileFormatLegacy::readHeaderFromStream(QDataStream& stream)
{
    QString name;
    stream >> name;

    int format;
    stream >> format;

    if (name == "SSL_LOG_FILE" && format == version()) {
        return true;
    }

    return false;
}

bool FileFormatLegacy::readMessageFromStream(QDataStream& stream, QByteArray& data, qint64& time, MessageType& type)
{
    type = MESSAGE_UNKNOWN;

    stream >> time;
    QByteArray compressedPacket;
    stream >> compressedPacket;
    data = qUncompress(compressedPacket);

    return true;
}
