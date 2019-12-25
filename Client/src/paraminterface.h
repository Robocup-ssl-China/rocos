#ifndef PARAMINTERFACE_H
#define PARAMINTERFACE_H
#include <QAbstractItemModel>

class TreeItem;

class ParamInterface : public QAbstractItemModel{
    Q_OBJECT
public:
    explicit ParamInterface(QObject *parent = Q_NULLPTR);
    ~ParamInterface(){}

    enum Roles  {
        KeyRole = Qt::UserRole + 1,
        TypeRole,
        ValueRole
    };
    Q_ENUM(Roles)
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QString getType(const QModelIndex &index);
    Q_INVOKABLE void reload();
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex parent(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
private:
    QString judgeType(const QVariant& value) const;
    void setupModelData();
    TreeItem *rootItem;
};

#endif // PARAMINTERFACE_H
