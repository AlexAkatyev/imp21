#include "workplacesmodel.h"

enum WPMColumn
{
    Name
    , RecordingInAllIndicators
    , EnModbusTCP
    , EnRS485
    , Uuid
    , ModbusAddresses
    , Count
};


WorkPlacesModel::WorkPlacesModel(QObject* parent)
    : QAbstractItemModel(parent)
    , _workPlacesNames(QStringList())
    , _recordingInAllIndicators(QList<bool>())
    , _enModbusTCP(QList<bool>())
    , _enRS485(QList<bool>())
    , _uuids(QStringList())
    , _modbusAddresses(QList<QStringList>())
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
       case WPMColumn::EnModbusTCP:
           return _enModbusTCP.at(index.row());
           break;
       case WPMColumn::EnRS485:
           return _enRS485.at(index.row());
           break;
       case WPMColumn::ModbusAddresses:
           return _modbusAddresses.at(index.row());
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
    case WPMColumn::ModbusAddresses:
        mflags = 0;
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
         case WPMColumn::EnModbusTCP:
             _enModbusTCP.replace(index.row(), value.toBool());
             emit dataChanged(index, index);
             return true;
             break;
         case WPMColumn::EnRS485:
             _enRS485.replace(index.row(), value.toBool());
             emit dataChanged(index, index);
             return true;
             break;
         case WPMColumn::ModbusAddresses:
             _modbusAddresses.replace(index.row(), value.toStringList());
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
        case WPMColumn::EnModbusTCP:
            return "Искать датчики\nModbus TCP";
            break;
        case WPMColumn::EnRS485:
            return "Искать датчики\nRS-485";
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


bool WorkPlacesModel::RecordingInAllIndicators(int row)
{
    if (row < 0 || row >= _recordingInAllIndicators.size())
    {
        return false;
    }
    return _recordingInAllIndicators.at(row);
}


void WorkPlacesModel::SetRecordingInAllIndicators(int active, bool en)
{
    QModelIndex index = QAbstractItemModel::createIndex(active, WPMColumn::RecordingInAllIndicators);
    setData(index, en);
}


bool WorkPlacesModel::EnModbusTCP(int row)
{
    if (row < 0 || row >= _enModbusTCP.size())
    {
        return false;
    }
    return _enModbusTCP.at(row);
}


void WorkPlacesModel::SetEnModbusTCP(int row, bool en)
{
    QModelIndex index = QAbstractItemModel::createIndex(row, WPMColumn::EnModbusTCP);
    setData(index, en);
}


bool WorkPlacesModel::EnRS485(int row)
{
    if (row < 0 || row >= _enRS485.size())
    {
        return false;
    }
    return _enRS485.at(row);
}


void WorkPlacesModel::SetEnRS485(int row, bool en)
{
    QModelIndex index = QAbstractItemModel::createIndex(row, WPMColumn::EnRS485);
    setData(index, en);
}


QStringList WorkPlacesModel::ModbusAddresses(int row)
{
    if (row < 0 || row >= _modbusAddresses.size())
    {
        return QStringList();
    }
    return _modbusAddresses.at(row);
}


void WorkPlacesModel::SetModbusAddresses(int row, QStringList sl)
{
    QModelIndex index = QAbstractItemModel::createIndex(row, WPMColumn::ModbusAddresses);
    setData(index, sl);
}


QString WorkPlacesModel::GetUuid(int row)
{
    if (row < 0 || row >= _uuids.size())
    {
        return "";
    }
    return _uuids.at(row);
}




void WorkPlacesModel::AddRecord
(
    QString name
    , bool recInAll
    , bool enModbusTCP
    , bool enRS485
    , QStringList modbusAddresses
    , QString uuid
)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _workPlacesNames.push_back(name);
    _recordingInAllIndicators.push_back(recInAll);
    _enModbusTCP.push_back(enModbusTCP);
    _enRS485.push_back(enRS485);
    _modbusAddresses.push_back(modbusAddresses);
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
    _enModbusTCP.removeAt(row);
    _enRS485.removeAt(row);
    _modbusAddresses.removeAt(row);
    _uuids.removeAt(row);
    endRemoveRows();
}
