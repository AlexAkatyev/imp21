#include "workplacesmodel.h"

WorkPlacesModel::WorkPlacesModel(QObject* parent)
    : QAbstractItemModel(parent)
    , _workPlacesNames(QStringList())
    , _uuids(QStringList())
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
    return 2;
}


QVariant WorkPlacesModel::data(const QModelIndex &index, int role) const
{
   if (role == Qt::DisplayRole && index.column() == 0)
   {
       return _workPlacesNames.at(index.row());
   }
   if (role == Qt::DisplayRole && index.column() == 1)
   {
       return _uuids.at(index.row());
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
    Qt::ItemFlags mflags = QAbstractItemModel::flags(index);
    if (index.column() == 0)
    {
        mflags |= Qt::ItemIsEditable;
    }
    return  mflags;
}


bool WorkPlacesModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole)
     {
         if (index.column() == 0)
         {
             _workPlacesNames.replace(index.row(), value.toString());
             emit dataChanged(index, index);
             return true;
         }
     }
     return false;
 }


QVariant WorkPlacesModel::headerData
(
    int section
    , Qt::Orientation orientation
    , int role
) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if (section == 0)
        {
            return "Имя";
        }
        else
        {
            return "Идентификатор";
        }
    }
    else
    {
        return QVariant();
    }
}


QStringList WorkPlacesModel::WorkPlacesNames()
{
    return _workPlacesNames;
}


QStringList WorkPlacesModel::Uuids()
{
    return _uuids;
}


void WorkPlacesModel::AddRecord(QString name, QString uuid)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _workPlacesNames.push_back(name);
    _uuids.push_back(uuid);
    endInsertRows();
}


void WorkPlacesModel::RemoveRecord(int row)
{
    if (_workPlacesNames.size() == 1) //последнее не удалять
    {
        return;
    }
    if (row < 0 || row >= _workPlacesNames.size())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    _workPlacesNames.removeAt(row);
    _uuids.removeAt(row);
    endRemoveRows();
}
