#include "paraminterface.h"
#include "parammanager.h"
#include "treeitem.h"
#include <QtDebug>
#include <regex>
namespace{
    auto zpm = ZSS::ZParamManager::instance();
}
ParamInterface::ParamInterface(QObject *parent)
    : QAbstractItemModel(parent),rootItem(nullptr) {
    setupModelData();
}
QString ParamInterface::judgeType(const QVariant& value) const{
    const static std::regex boolExp("true|false|t|f",std::regex_constants::icase);
    const static std::regex doubleExp("^(-?)(0|([1-9][0-9]*))(\\.[0-9]+)?$");
    const static std::regex integerExp("^(0|[1-9][0-9]*)$");
    if(std::regex_match(value.toString().toUtf8().constData(),boolExp))
        return "Bool";
    else if(std::regex_match(value.toString().toUtf8().constData(),integerExp))
        return "Int";
    else if(std::regex_match(value.toString().toUtf8().constData(),doubleExp))
        return "Double";
    else
        return "String";
}
QHash<int,QByteArray> ParamInterface::roleNames() const {
     QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
     result.insert(KeyRole, QByteArrayLiteral("settingName"));
     result.insert(TypeRole, QByteArrayLiteral("settingType"));
     result.insert(ValueRole, QByteArrayLiteral("settingValue"));
     return result;
}
Qt::ItemFlags ParamInterface::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
bool ParamInterface::setData(const QModelIndex &index, const QVariant &value,int role){
    Q_UNUSED(role)
    if (!index.isValid()){
        qDebug() << "FUCKME!!!!!!!!";
        return false;
    }
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    item->changeData(2,value.toString());
    item->changeData(1,judgeType(value));
    zpm->changeParam(item->parentItem()->data(0),item->data(0),value);
    qDebug() << item->parentItem()->data(0) << item->data(0) << value.toString();
    emit dataChanged(index, index);
    return true;
}
QVariant ParamInterface::data(const QModelIndex &index, int role) const {
//    qDebug() << role - KeyRole;
    if (index.isValid() && role >= KeyRole) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data(role - KeyRole);
    }
    return QVariant("Error");
}
QString ParamInterface::getType(const QModelIndex &index){
    if(index.isValid()){
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return judgeType(item->data(2));
    }
    return "Invalid";
}
void ParamInterface::reload(){
    beginResetModel();
    ZSS::ZParamManager::instance()->sync();
    setupModelData();
    endResetModel();
    this->resetInternalData();
}
QModelIndex ParamInterface::parent(const QModelIndex &index) const{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
int ParamInterface::columnCount(const QModelIndex &parent) const{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
int ParamInterface::rowCount(const QModelIndex &parent) const{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    return parentItem->childCount();
}
QModelIndex ParamInterface::index(int row, int column,const QModelIndex &parent) const{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
void ParamInterface::setupModelData(){
    if(rootItem != nullptr){
        delete rootItem;
        rootItem = nullptr;
    }
    QList<QString> rootData;
    rootData << "settingName" << "settingType" << "settingValue";
    rootItem = new TreeItem(rootData);
    foreach(QString groupName, zpm->allGroups()){
        QStringList value;
        value << groupName << " " << " ";
        auto* groupParent = new TreeItem(value,rootItem);
        rootItem->appendChild(groupParent);
        foreach(QString key,zpm->allKeys(groupName)){
            auto&& temp = zpm->value(groupName,key).toString();
            QStringList value;
            value << key << judgeType(temp) << temp;
            groupParent->appendChild(new TreeItem(value,groupParent));
        }
    }
}
