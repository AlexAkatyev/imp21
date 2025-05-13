#include "workplacesmodel.h"

WorkPlacesModel::WorkPlacesModel(QObject* parent)
    : QAbstractItemModel(parent)
    , _workPlacesNames(QStringList())
    , _uids(std::vector<QString>())
    , _indicatorsVec(std::vector<QStringList>())
{
}


int WorkPlacesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _workPlacesNames.size();
}


int WorkPlacesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}


QVariant WorkPlacesModel::data(const QModelIndex &index, int role) const
{
   if (role == Qt::DisplayRole && index.column() == 0)
   {
       return _workPlacesNames.at(index.row());
   }
   return QVariant();
}


QModelIndex WorkPlacesModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}


QModelIndex WorkPlacesModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    QModelIndex index = QAbstractItemModel::createIndex(row, column);
    return index;
}


Qt::ItemFlags WorkPlacesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}


bool WorkPlacesModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole)
     {
         _workPlacesNames.replace(index.row(), value.toString());
         emit dataChanged(index, index);
         return true;
     }
     return false;
 }


QStringList WorkPlacesModel::WorkPlacesNames()
{
    return _workPlacesNames;
}


void WorkPlacesModel::AddRecord(QString name)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _workPlacesNames.push_back(name);
    endInsertRows();
}
