#include "workplacesmodel.h"

enum WPMColumn
{
    Name
    , RecordingInAllIndicators
    , Uuid
    , Count
};


WorkPlacesModel::WorkPlacesModel(QObject* parent)
    : QAbstractItemModel(parent)
    , _workPlacesNames(QStringList())
    , _recordingInAllIndicators(QList<bool>())
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
    return WPMColumn::Count;
}


QVariant WorkPlacesModel::data(const QModelIndex &index, int role) const
{
   if (role == Qt::DisplayRole)
   {
       switch (index.column())
       {
       case WPMColumn::Name:
           return _workPlacesNames.at(index.row());
           break;
       case WPMColumn::RecordingInAllIndicators:
           return _recordingInAllIndicators.at(index.row());
           break;
       case WPMColumn::Uuid:
           return _uuids.at(index.row());
           break;
       default:
           return QVariant();
           break;
       }
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
    switch (index.column())
    {
    case WPMColumn::Name:
        mflags |= Qt::ItemIsEditable;
        break;
    default:
        break;
    }
    return  mflags;
}


bool WorkPlacesModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole)
     {
         switch (index.column())
         {
         case WPMColumn::Name:
             _workPlacesNames.replace(index.row(), value.toString());
             emit dataChanged(index, index);
             return true;
             break;
         case WPMColumn::RecordingInAllIndicators:
             _recordingInAllIndicators.replace(index.row(), value.toBool());
             emit dataChanged(index, index);
             return true;
             break;
         default:
             break;
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
        switch (section)
        {
        case WPMColumn::Name:
            return "Имя";
            break;
        case WPMColumn::RecordingInAllIndicators:
            return "Запись во все\nиндикаторы";
            break;
        case WPMColumn::Uuid:
            return "Идентификатор";
            break;
        default:
            return QVariant();
            break;
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


bool WorkPlacesModel::RecordingInAllIndicators(int active)
{
    if (active < 0 || active >= _recordingInAllIndicators.size())
    {
        return false;
    }
    return _recordingInAllIndicators.at(active);
}


void WorkPlacesModel::SetRecordingInAllIndicators(int active, bool en)
{
    QModelIndex index = QAbstractItemModel::createIndex(active, WPMColumn::RecordingInAllIndicators);
    setData(index, en);
}


QList<bool> WorkPlacesModel::RecordingInAllIndicatorsArr()
{
    return _recordingInAllIndicators;
}


QStringList WorkPlacesModel::Uuids()
{
    return _uuids;
}


void WorkPlacesModel::AddRecord(QString name, bool recInAll, QString uuid)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _workPlacesNames.push_back(name);
    _recordingInAllIndicators.push_back(recInAll);
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
    _recordingInAllIndicators.removeAt(row);
    _uuids.removeAt(row);
    endRemoveRows();
}
