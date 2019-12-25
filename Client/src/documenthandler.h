#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H
#include <QObject>

class DocumentHandler : public QObject
{
    Q_OBJECT
public:
    DocumentHandler();
signals:
    void addText(const QString str);
    void clearText();
private:
};

#endif
