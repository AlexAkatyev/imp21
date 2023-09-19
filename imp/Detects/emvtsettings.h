#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QWidget>

class QQuickWidget;
class ImpAbstractDetect;


// ImpAbstractDetectSettings
class EmVTSettings : public QWidget
{
    Q_OBJECT
public:
    explicit EmVTSettings(ImpAbstractDetect* parent, int imageCode = 0);

private slots:

private:
    // Включение переменных виджета QML
    // Указатель на виджет qml в окне класса
    QQuickWidget* _quickUi;

    int _imageCode;

    ImpAbstractDetect* _detect;
    // переопределение события изменения окна
    void resizeEvent(QResizeEvent*);

    // Кодировщик для датчика
    QTextCodec* _codec;

    void fillQmlWidget();   // Заполнение пользовательского окна "Установки" данными датчика
};

#endif // SETTINGS_H
