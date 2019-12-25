#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include "message_type.h"
#include <QDataStream>

class FileFormat
{
public:
    FileFormat(int version) : m_version(version) {}

public:
    int version() { return m_version; }

public:
    virtual bool readHeaderFromStream(QDataStream& stream) = 0;

public:
    virtual bool readMessageFromStream(QDataStream& stream, QByteArray& data, qint64& time, MessageType& type) = 0;

private:
    int m_version;
};

#endif // FILE_FORMAT_H
