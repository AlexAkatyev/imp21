#ifndef WORKPLACESMODEL_H
#define WORKPLACESMODEL_H

#include <QAbstractItemModel>

class WorkPlacesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    WorkPlacesModel(QObject* parent);
    QStringList WorkPlacesNames();
    void AddRecord(QString name);
    void RemoveRecord(int row);

private:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QStringList _workPlacesNames;
    std::vector<QString> _uids;
    std::vector<QStringList> _indicatorsVec;
};

#endif // WORKPLACESMODEL_H
