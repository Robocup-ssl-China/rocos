#include "log_file.h"
#include <QFileInfo>
#include <QDataStream>
#include <QDebug>
#include "qtiocompressor.h"
#include "file_format_legacy.h"
#include "file_format_timestamp_type_size_raw_message.h"

const int LogFile::DEFAULT_FILE_FORMAT_VERSION;

LogFile::LogFile(const QString& filename, bool compressed, int formatVersion) :
    m_filename(filename),
    m_compressed(compressed),
    m_formatVersion(formatVersion),
    m_io(NULL),
    m_file(NULL),
    m_compressor(NULL)
{
    addFormat(new FileFormatLegacy());
    addFormat(new FileFormatTimestampTypeSizeRawMessage());
}

LogFile::~LogFile()
{
    close();
    qDeleteAll(m_formatMap);
}

bool LogFile::addFormat(FileFormat* format)
{
//    if (m_formatMap.contains(format->version())) {
//        std::cout << "Error adding log format!" << std::endl;
//        std::cout << "Format version " << format->version() << " has been used twice." << std::endl;

//        return false;
//    }

    m_formatMap[format->version()] = format;

    return true;
}

bool LogFile::openRead() {
    foreach (FileFormat* format, m_formatMap) {
        close();

        m_file = new QFile(m_filename);

        if (m_compressed) {
            m_compressor = new QtIOCompressor(m_file);
            m_compressor->setStreamFormat(QtIOCompressor::GzipFormat);
            m_compressor->open(QIODevice::ReadOnly);

            m_io = m_compressor;
        } else {
            m_file->open(QIODevice::ReadOnly);
            m_io = m_file;
        }

        QDataStream stream(m_io);
        if (format->readHeaderFromStream(stream)) {
//            std::cout << "Detected log file format version " << format->version() << "." << std::endl;
            m_formatVersion = format->version();
            return true;
        }
    }

    close();
    return false;

}

bool LogFile::readMessage(QByteArray& data, qint64& time, MessageType& type)
{
    if (m_io == NULL || !m_io->isReadable()) {
        return false;
    }

    FileFormat* format = m_formatMap.value(m_formatVersion, NULL);

    if (format == NULL) {
//        std::cout << "Error log file format is not supported!" << std::endl;

        return false;
    }

    QDataStream stream(m_io);
    stream.setVersion(QDataStream::Qt_4_6);

    if (stream.atEnd()) {
        return false;
    }

    return format->readMessageFromStream(stream, data, time, type);
}

void LogFile::close()
{
    m_io = NULL;

    delete m_compressor;
    m_compressor = NULL;

    delete m_file;
    m_file = NULL;
}
