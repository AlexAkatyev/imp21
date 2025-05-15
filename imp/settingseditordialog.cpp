#include <QUuid>

#include "settingseditordialog.h"
#include "ui_settingseditordialog.h"
#include "workplacesmodel.h"

SettingsEditorDialog::SettingsEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsEditorDialog)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


SettingsEditorDialog::~SettingsEditorDialog()
{
    delete ui;
}


void SettingsEditorDialog::on_closeButton_clicked()
{
    close();
    deleteLater();
}


void SettingsEditorDialog::SetModel(QAbstractItemModel* model)
{
    ui->tableView->setModel(model);
}


void SettingsEditorDialog::on_addButton_clicked()
{
    WorkPlacesModel* model = static_cast<WorkPlacesModel*>(ui->tableView->model());
    model->AddRecord
            (
                "Новое рабочее место"
                , false
                , QUuid::createUuid().toString()
            );
}


void SettingsEditorDialog::on_removeButton_clicked()
{
    WorkPlacesModel* model = static_cast<WorkPlacesModel*>(ui->tableView->model());
    QModelIndex index = ui->tableView->currentIndex();
    if (index == QModelIndex())
    {
        return;
    }
    model->RemoveRecord(index.row());
}
