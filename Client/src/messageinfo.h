#ifndef MESSAGEINFO_H
#define MESSAGEINFO_H

#include <QAbstractListModel>
#include <QStringList>
#include <QQmlEngine>

class MessageInfo : public QObject{
    Q_OBJECT
    Q_PROPERTY (char info READ info WRITE setInfo NOTIFY infoChanged)
public:
    static QObject* instance(QQmlEngine* engine = nullptr, QJSEngine* scriptEngine = nullptr){
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        static MessageInfo* instance = new MessageInfo();
        return instance;
    }
    explicit MessageInfo(QObject *parent = Q_NULLPTR) : m_info(0x00){}
    ~MessageInfo(){}

    char info() const { return m_info; }
    Q_INVOKABLE void setInfo(char val) { m_info = val; emit infoChanged(val);}

signals:
    void infoChanged(char newValue);

private:
    char m_info;
};


// class MessageInfo : public QAbstractListModel{
//     Q_OBJECT
//     Q_PROPERTY (int someProperty READ someProperty WRITE setSomeProperty NOTIFY somePropertyChanged)
// public:
//     static QObject* instance(QQmlEngine* engine, QJSEngine* scriptEngine){
//         Q_UNUSED(engine)
//         Q_UNUSED(scriptEngine)
//         static MessageInfo* instance = new MessageInfo();
//         return instance;
//     }
//     explicit MessageInfo(QObject *parent = Q_NULLPTR){}
//     ~MessageInfo(){}

//     enum Roles{
//         messageName = Qt::UserRole + 1,
//         messageValue
//     };
//     Q_ENUM(Roles)
//     QHash<int, QByteArray> roleNames() const{
//         QHash<int, QByteArray> result = QAbstractListModel::roleNames();
//         result.insert(messageName, ("name"));
//         result.insert(messageValue, ("value"));
//         return result;
//     }
//     virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override{
//         return 3;
//     }
//     virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override{
//         return 1;
//     }
//     virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override{
//         if(index.isValid()){
//             switch(role){
//             case messageName:
//                 return m_someProperty;//messageNames[index.row()];
//             case messageValue:
//                 return m_someProperty;//messageValues[index.row()];
//             return 0;
//             }
//         }
//     }
//     QStringList messageNames = {"Sim", "Medusa", "Vision"};
//     static bool messageValues[3];

//    int someProperty() const { return m_someProperty; }
//    void setSomeProperty(int val) { m_someProperty = val; emit somePropertyChanged(val);
//                                    emit dataChanged(createIndex(0,0), createIndex(2,0));}
//    static int m_someProperty;

// signals:
//    void somePropertyChanged(int newValue);

// private:

// };

#endif // MESSAGEINFO_H
