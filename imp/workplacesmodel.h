#ifndef WORKPLACESMODEL_H
#define WORKPLACESMODEL_H

#include <QAbstractItemModel>

class WorkPlacesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    WorkPlacesModel(QObject* parent);
    QStringList WorkPlacesNames();
    bool RecordingInAllIndicators(int active);
    void SetRecordingInAllIndicators(int active, bool en);
    QList<bool> RecordingInAllIndicatorsArr();
    bool EnRS485(int active);
    void SetEnRS485(int active, bool en);
    QList<bool> EnRS485Arr();
    QStringList Uuids();
    void AddRecord
        (
            QString name
            , bool recInAll
            , bool enRS485
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
    QList<bool> _enRS485;
    QStringList _uuids;
    std::vector<QStringList> _indicatorsVec;
};

#endif // WORKPLACESMODEL_H
