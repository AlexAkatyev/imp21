#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

class QLabel;
class ImpSettings;
class QListWidget;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = 0, int V_MAJOR = 0, int V_MINOR = 0, int V_PATCH = 0, ImpSettings* _settings = 0);

private:
    ImpSettings* _settings;

    void saveAdresses(QListWidget* lw);
};



#endif // ABOUT_H
