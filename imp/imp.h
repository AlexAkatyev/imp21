#ifndef IMP_H
#define IMP_H

#include <QWidget>
#include <QSet>
#include <map>

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

    static QString MessageTitle();

signals:
    // Сигнал включения в список нового датчика
    void sigAddDetect();
    // Сигналы о закрытии пользовательких окон
    void sigCloseIndicatorWindows();
    void sigCloseSettingsWindows();
    void sigCloseGeneralWindow();
    void sigFindDetect();

public slots:
    void RaiseIndicators();

private slots:
    void findDetect(); // Поиск датчиков
    void indicateFindCOMDetect(); // Индикация в главном окне хода поиска USB датчиков
    void reWriteDetectsToTable();
    void changeActiveStatusToTable();
    // Создание нового индикатора, можно задать имя датчика
    void createNewIndicator(QString);
    //Создание множества окон индикаторов для заполнения всего экрана
    void fillScreenWithIndicators();
    //Расстановка окон индикаторов попорядку
    void composeOpenWindowsInOrder();
    // Удаление индикатора из множества
    void deleteIndicator(int);
    // Отработка команды: Помощь/Справка
    void showHelp();
    // чтение настроек
    void setIniFindModbusTCP(bool en);
    void setIniFindModbus485(bool en);
    void setRecordingInAllIndicators(bool en);
    void setModbusAdresses(QString adresses);
    void openWorkPlacesEditor();
    void updateCbWorkPlaces();
    void workPlaceChanged();

private:

    // Указатель на виджет qml в окне класса
    QQuickWidget* _pQuickUi;
    // Указатели на объекты QML виджета
    QObject* _pitWin; // указатель на свойства для списка датчиков
    QObject* _ppbFind; // ход поиска датчиков
    QObject* _ptextComment; // статус поиска датчиков
    QObject* _pbtFind; // кнопка поиска датчиков
    QObject* _pbtIndicator; // кнопка открытия нового индикатора

    // переопределение события изменения окна
    void resizeEvent(QResizeEvent*);

    QTimer* _indicateTimer; // Для отображения поиска датчиков
    // Для паузы после запуска программы перед поиском датчиков
    QTimer *TimerBeforeFound;
    QTimer* _timerUpdaterActiveStatus;

    // Объект и файл для хранения установок главного окна
    bool LoadSettingsGeneral(); // Загрузка параметров из файла установок
    void SaveSettingsGeneral(); // Сохранение параметров установок в файл
    void linkIni();
    QSet<int> _useIndicators; // Множество используемых индикаторов
    std::vector<Indicator*> _indicators;
    bool _flagRunIndicators; // Флаг необходимости запускать индикаторы после запуска программы
    QString _uuid; // uuid текущего рабочего места

    void createIndicator(int index, ImpAbstractDetect* baseDetect, bool defOption);
    void createScreenIndicators(QList<int> indexList, ImpAbstractDetect* baseDetect = nullptr);

    // Количество найденых датчиков
    int _uiCounter; // для отображения прогресса поиска датчиков
    int _uiLength;
    uint _uiModbusAddressCounter; // Счетчик применяется при индикации поиска датчиков в IndicateFindRS485Detect
    uint _uiModbusAddress; // Счетчик адресов Modbus

    std::vector<ImpAbstractDetect*> _detects;

    // Описания для поиска датчиков в COM-портах
    void FindDetects(void); // Поиск USB датчиков

    // Переопределение закрытия главного окна
    void closeEvent(QCloseEvent*);

    void fillUseIndicators();
    void runIndicators();
};


#endif // IMP_H
