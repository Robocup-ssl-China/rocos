#ifndef LOGFILE_H
#define LOGFILE_H

#include "file_format.h"
#include <QList>
#include <QMap>
#include <QFile>

class QtIOCompressor;

class LogFile
{
public:
    LogFile(const QString& filename, bool compressed = false, int formatVersion = DEFAULT_FILE_FORMAT_VERSION);

    bool openRead();
    void close();
    bool readMessage(QByteArray &data, qint64 &time, MessageType &type);
    bool addFormat(FileFormat *format);
    ~LogFile();
private:
    QString m_filename;
    bool m_compressed;
    int m_formatVersion;
    QMap<int, FileFormat*> m_formatMap;
    QIODevice* m_io;
    QFile* m_file;
    QtIOCompressor* m_compressor;

    static const int DEFAULT_FILE_FORMAT_VERSION = 1;
};

#endif // LOGFILE_H
