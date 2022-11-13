#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = 0, int V_MAJOR = 0, int V_MINOR = 0, int V_PATCH = 0);

private:
    QLabel* about;
    QLabel* copyright;
    QLabel* developed;
    QLabel* support;
    QLabel* thanks;
    QLabel* version;
    QLabel* webAdress;
};



#endif // ABOUT_H
