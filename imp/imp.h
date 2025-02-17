#ifndef IMP_H
#define IMP_H

#include <QWidget>
#include <QSet>

class Indicator;
class QQuickWidget;
class ImpAbstractDetect;

class Imp : public QWidget
{
    Q_OBJECT

public:
    Imp(QWidget* parent = nullptr);
    QStringList DetectNames();
    ImpAbstractDetect* DetectAtId(int id);
    ImpAbstractDetect* DetectAtName(QString idName);

signals:
    // Сигнал включения в список нового датчика
    void sigAddDetect();
    // Сигналы о закрытии пользовательких окон
    void sigCloseIndicatorWindows();
    void sigCloseSettingsWindows();
    void sigCloseGeneralWindow();
    void sigFindDetect();


private slots:
    void findDetect(); // Поиск датчиков
    void indicateFindCOMDetect(); // Индикация в главном окне хода поиска USB датчиков
    void reWriteDetectsToTable();
    void changeActiveStatusToTable();
    // Создание класса для работы с установками датчика - в качестве параметра - серийный номер датчика
    void createNewSettings(QString);
    // Создание нового индикатора, можно задать имя датчика
    void createNewIndicator(QString);
    // Удаление индикатора из множества
    void deleteIndicator(int);
    // Отработка команды: Помощь/О программе
    void showAbout();
    // Отработка команды: Помощь/Справка
    void showHelp();

private:

    // Указатель на виджет qml в окне класса
    QQuickWidget* pQuickUi;
    // Указатели на объекты QML виджета
    QObject* pitWin; // указатель на свойства для списка датчиков
    QObject* ppbFind; // ход поиска датчиков
    QObject* ptextComment; // статус поиска датчиков
    QObject* pbtFind; // кнопка поиска датчиков
    QObject* pbtIndicator; // кнопка открытия нового индикатора

    // переопределение события изменения окна
    void resizeEvent(QResizeEvent*);

    QTimer* _indicateTimer; // Для отображения поиска датчиков
    // Для паузы после запуска программы перед поиском датчиков
    QTimer *TimerBeforeFound;
    QTimer* _timerUpdaterActiveStatus;

    // Объект и файл для хранения установок главного окна
    bool LoadSettingsGeneral(); // Загрузка параметров из файла установок
    void SaveSettingsGeneral(); // Сохранение параметров установок в файл
    QSet<int> _useIndicators; // Множество используемых индикаторов
    std::vector<Indicator*> _indicators;
    bool _flagRunIndicators; // Флаг необходимости запускать индикаторы после запуска программы

    void createIndicator(int index, ImpAbstractDetect* baseDetect, bool defOption);

    // Количество найденых датчиков
    int _uiCounter; // для отображения прогресса поиска датчиков
    int _uiLength;
    uint uiModbusAddressCounter; // Счетчик применяется при индикации поиска датчиков в IndicateFindRS485Detect
    uint uiModbusAddress; // Счетчик адресов Modbus

    std::vector<ImpAbstractDetect*> _detects;

    // Описания для поиска датчиков в COM-портах
    void FindDetects(void); // Поиск USB датчиков

    // Переопределение закрытия главного окна
    void closeEvent(QCloseEvent*);
};


#endif // IMP_H
