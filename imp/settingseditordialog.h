#ifndef SETTINGSEDITORDIALOG_H
#define SETTINGSEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsEditorDialog;
}

class SettingsEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsEditorDialog(QWidget *parent = nullptr);
    ~SettingsEditorDialog();

private slots:
    void on_closeButton_clicked();

private:
    Ui::SettingsEditorDialog *ui;
};

#endif // SETTINGSEDITORDIALOG_H
