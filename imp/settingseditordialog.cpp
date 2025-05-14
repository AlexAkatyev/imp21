#include "settingseditordialog.h"
#include "ui_settingseditordialog.h"

SettingsEditorDialog::SettingsEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsEditorDialog)
{
    ui->setupUi(this);
}


SettingsEditorDialog::~SettingsEditorDialog()
{
    delete ui;
}


void SettingsEditorDialog::on_closeButton_clicked()
{
    close();
}
