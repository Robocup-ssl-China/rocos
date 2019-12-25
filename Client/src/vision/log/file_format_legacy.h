#ifndef FILE_FORMAT_LEGACY_H
#define FILE_FORMAT_LEGACY_H

#include "file_format.h"

class FileFormatLegacy : public FileFormat
{
public:
    FileFormatLegacy();
    bool readHeaderFromStream(QDataStream &stream);
    bool readMessageFromStream(QDataStream &stream, QByteArray &data, qint64 &time, MessageType &type);
};

#endif // FILE_FORMAT_LEGACY_H
