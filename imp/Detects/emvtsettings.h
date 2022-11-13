#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QWidget>

class QQuickWidget;
class EmVTDetect;


class EmVTSettings : public QWidget
{
    Q_OBJECT
public:
    explicit EmVTSettings(EmVTDetect* parent);

private slots:

private:
    // Включение переменных виджета QML
    // Указатель на виджет qml в окне класса
    QQuickWidget* _quickUi;
    // Указатели на объекты QML виджета

    EmVTDetect* _detect;
    // переопределение события изменения окна
    void resizeEvent(QResizeEvent*);

    // Кодировщик для датчика
    QTextCodec* _codec;

    void fillQmlWidget();   // Заполнение пользовательского окна "Установки" данными датчика
};

#endif // SETTINGS_H
