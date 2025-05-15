#ifndef WORKPLACESMODEL_H
#define WORKPLACESMODEL_H

#include <QAbstractItemModel>

class WorkPlacesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    WorkPlacesModel(QObject* parent);
    QStringList WorkPlacesNames();
    bool RecordingInAllIndicators(int row);
    void SetRecordingInAllIndicators(int row, bool en);
    bool EnModbusTCP(int row);
    void SetEnModbusTCP(int row, bool en);
    bool EnRS485(int row);
    void SetEnRS485(int row, bool en);
    QStringList ModbusAddresses(int row);
    void SetModbusAddresses(int row, QStringList sl);
    QStringList Indicators(int row);
    void SetIndicators(int row, QStringList sl);
    QString GetUuid(int row);
    void AddRecord
        (
            QString name
            , bool recInAll
            , bool enModbusTCP
            , bool enRS485
            , QStringList modbusAddresses
            , QStringList indicators
            , QString uuid
        );
    void RemoveRecord(int row);

private:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QStringList _workPlacesNames;
    QList<bool> _recordingInAllIndicators;
    QList<bool> _enModbusTCP;
    QList<bool> _enRS485;
    QStringList _uuids;
    QList<QStringList> _modbusAddresses;
    QList<QStringList> _indicatorsList;
};

#endif // WORKPLACESMODEL_H
