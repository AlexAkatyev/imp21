#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

class QLabel;
class ImpSettings;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = 0, int V_MAJOR = 0, int V_MINOR = 0, int V_PATCH = 0, ImpSettings* _settings = 0);

private:
    ImpSettings* _settings;
};



#endif // ABOUT_H
