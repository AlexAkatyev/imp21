#ifndef SETTINGSEDITORDIALOG_H
#define SETTINGSEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsEditorDialog;
}

class QAbstractItemModel;

class SettingsEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsEditorDialog(QWidget *parent = nullptr);
    ~SettingsEditorDialog();
    void SetModel(QAbstractItemModel* model);

private slots:
    void on_closeButton_clicked();

    void on_addButton_clicked();

    void on_removeButton_clicked();

private:
    Ui::SettingsEditorDialog *ui;
};

#endif // SETTINGSEDITORDIALOG_H
