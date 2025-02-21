#ifndef INDICATOR_H
#define INDICATOR_H


#include <QWidget>

class Imp;
class IndSettings;
class QQuickWidget;
class ImpAbstractDetect;
class FormulaNode;

// Максимальная длина истории измерений для усреднения
#define LEN_MAD 2000

class Indicator : public QWidget
{
    Q_OBJECT
public:
    Indicator(QWidget* parent,         // родитель
              int identificator,       // Номер индикатора
              ImpAbstractDetect* baseDetect = nullptr,
              bool defOptions = false);    // Ссылка на датчик
    int Id();
    static std::vector<int> GetDefaultSize();

signals:
    void sigCloseIndicator(int); // Закрытие окна индикатора, передается номер индикатора - для родительского окна
    void sigCloseMyIndicator(void); // Сигнал о закрытии окна дочерним окнам
    void sigDataPressed(); // нажатие на кнопку DATA

public slots:
    void CloseMyIndicator(void); // Закрытие окна по команде от внешних объектов
    void RunButtonRelease(); // Отработать нажатие в другом индикаторе или своего датчика

private:
    // преобразование измерений из единиц измерния датчиков в единицы измерения пользователя
    // 0 - без преобразования
    // 1 - из мкм в мм
    // 2 - из мкм в inch
    // 3 - из мкм/м в угловые секунды
    enum TransToUnit
    {
      UnitMKM = 0
      , UnitMM = 1
      , UnitInch = 2
      , UnitAngleSec = 3
    };

    Imp* _parent;
    int _idIndicator; // Номер индикатора
    std::vector<QString> _measuredLogs;

    // Включение переменных виджета QML
    // Указатель на виджет qml в окне класса
    QQuickWidget* _quickUi;
    // Указатели на объекты QML виджета

    // Страница кнопки ФОРМУЛА +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tfFactor1; // Множитель первого датчика
    QObject* _cbListDetect1; // Первый датчик
    QObject* _tfIncert1; // Сдвиг первого датчика
    QObject* _tfFactor2; // Множитель второго датчика
    QObject* _cbListDetect2; // Второй датчик
    QObject* _tfIncert2; // Сдвиг второго датчика
    QObject* _tfDivider; // Общий делитель формулы
    QObject* _tfName; // Имя индикатора

    // Страница кнопки ШКАЛА +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tfUnitPoint; // Цена деления шкалы
    QObject* _tfNumberCharPoint; // Количество точек после запятой

    // Страница кнопки ДОПУСК ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tfHiLimit; // Верхний допуск
    QObject* _tfLoLimit; // Нижний допуск
    QObject* _tfPriemka; // ПРиемочная граница

    // Страница кнопки РЕЖИМ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tfPeriod; // Период усреднения значений
    QObject* _tfSumPoint; // Максимальное количество точек графика
    QObject* _rbHandMode; // Выбор ручного режима
    QObject* _rbAutoMode; // Выбор автоматического режима
    QObject* _tfStatPeriod; // Период измерения, мс

    // Страница кнопки СТАТИСТИКА ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tStatChart; // Указатель на график

    // Входные данные для индикатора +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _inputIndicator;



    // переопределение события изменения окна
    void resizeEvent(QResizeEvent*);
    // переопределение события закрытия окна (нажатием на крестик)
    void closeEvent(QCloseEvent*);

    // Расчет показаний датчиков по формуле
    float calculateResult();
    float calculateChannel(int number);
    void enableSetZero();
    float getMeasForTransform(float mr);

    // Флаг готовности функции
    bool _formulaComlete;
    // Входные параметры для формулы
    double _scale1;
    double _scale2;
    ImpAbstractDetect* _detect1;
    ImpAbstractDetect* _detect2;
    double _increment1;
    double _increment2;
    float _divider;
    std::vector<float> _zeroShifts;
    TransToUnit _transGauge;
    FormulaNode* _complexFormula;

    // Диапазон измерения датчиков
    int iRange1; // датчик 1
    int iRange2; // датчик 2
    // Таймер для контроля работы датчиков
    QTimer* ptimerControl;
    int _periodMean; // Время усреднения показаний датчиков
    int _lenMean; // Количество периодов для усреднения
    float flHistoryMean[LEN_MAD]; // Массив для усреднения

    QTimer* _timerWatchDog;  // Таймер чтения показаний датчиков для контроля работоспособноси

    QString _autoSaveFile;

    // Объект для хранения установок окна
    IndSettings* _settings;
    // сохранение настроек
    void loadSettingsWindow(); // Загрузка параметров окна
    bool loadSettingsIndicator(bool defOptions); // Загрузка параметров из файла установок
    void saveSettingsIndicator(); // Сохранение параметров установок в файл

    QStringList createListByDetect1();
    QStringList createListByDetect2();
    int currentIndex1ByName(QString nameDetect);
    int currentIndex2ByName(QString nameDetect);

    // Базовый размер QML виджета
    QSize baseSizeQML;
    QSize sizeDesktop;

    void setWorkIndicators();
    void setTitle(QString indicatorName);
    void setWindowTitleMain(QString indicatorName);
    QString _userTitle;

private slots:
    void changeIndication(void); // Изменение способа показаний
    void changeLimit(void); // Изменение допусков на индикаторе
    void changedTransGauge();
    void getDivisionValue(void); // Определение цены деления
    void setFormula(void); // Установка новой формулы
    void setZeroShifts(); // Установка 0 датчиков
    // Вывод от датчиков на индикатор
    void updateResult(void);
    // Определение, какой датчик не работает
    void watchDogControl(void); // Чтение результатов измерения датчиков для контроля работоспособноси
    void createComplexFormula(QString inputText);

    // Для работы с виджетом QML
    void setComboListDetect(void); // отработка сигнала об изменении списка датчиков от главного окна
    void setWindowName(void);

    // печать графика
    void printChart();
    void saveChartToCSV();
    void saveChartToXLS();

    // запись в файл
    void saveMeas(void); // сохранение измерений
    void saveToCSV(QString fileName);
    void saveToXLS(QString fileName);
    void selectAutoSaveFile();
    void autoSaveToXLSX();

    // сообщение в систему
    void sendWmAppMessage();
};

#endif // INDICATOR_H
