#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusbase.h"
#include "init.h"

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QStandardItemModel>
#include <QModbusClient>
#include <QModbusServer>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
    class MainWindow;
}

typedef struct adjustIOStruct{
    int readAdjustAddr;
    int gap;
    int setValue;
    int readValue;
    int direction;
}adjustIOStruct;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void serialAlarmInit();
    void initModbusSlave();
    void _sleep(unsigned int = 2000);
    bool checkFlag(int);
    void setBitMapValue(QVector<quint16>&);
    void checkIO(QString ,int, int,QModbusClient*, int, QModbusClient*, int );
    void checkIO2(QString ,int, int,QModbusClient*, int, QModbusClient*, int );
    void checkIO5(QString ,int, int,QModbusClient*, int, QModbusClient*, int );
    void loraRFFinish();
    void FirstTest();
    void SecondTest();
    void prepareAndSendModbus(int addr, int entry, QString cnt, QString msg);
    void prepareAndSendModbus(int addr, int entry, int cnt, QString msg);
    int setUILabelInfo(QLabel*);
    void setUILabelInfoEachTIme(QLabel *);
    void sl102Connect();
    void mbusTestStart();

    int mbusDefault(Init);
    int cellularDefault(Init);
    int mqttDefault(Init);
    int mbusMeterDefault(Init);
    QVector<quint16> meterHeadModbusUnit(QString, int , quint16 , QString , int);
    int mbusMeterTagDefault(Init);
    void mbusLoadModelSN();
    void mbusLoadDefaultStart();

    int mbusSerialCheck();
    int mbusCellularCheck();
    int mbusMqttCheck();
    int mbusMeterCheck();
    int mbusMeterTagCheck();
    void mbusCheckStart();
    QString testResult;
signals:
    void IOAdjustSignal(adjustIOStruct&);

private slots:
    void serialAlarmTask();
    void on_connectPushButton_clicked();
    void on_connectGoldenPushButton_clicked();
    void on_connectSl102PushButton_clicked();
    void on_functionStartPushButton_clicked();
    void on_tabWidget_tabBarClicked(int index);
    void on_resetPushButton_clicked();
    void on_LMUSBCom_clicked();
    void on_LCUSBCom_clicked();
    void on_LMRS485Com_clicked();
    void on_LCRS485Com_clicked();
    void on_LoRaStart_clicked();
    void on_clearPushButton_clicked();
    void on_savePushButton_clicked();
    void IOAdjustSlot(adjustIOStruct&);
    void on_stopPushButton_clicked();
    void on_finishedCheckBox_clicked();
    void on_functionSl102StartPushButton_clicked();
    void on_setPushButton_clicked();
    void on_resetSL102PushButton_clicked();
    void on_mbusModeComboBox_currentIndexChanged(int index);
    void on_mbusPushButton_clicked();
    void on_testOperationCheckBox_clicked();
    void on_formatFlashPushButton_clicked();

private:
    Ui::MainWindow *ui;
    // Init *init;
    QVector<QString> serialInfoVector;
    ModbusBase *modbusBase;
    QModbusClient *modbusDevice; // also use as lm usb serial in lora test
    QModbusClient *modbusGoldenDevice; // also use as lc usb serial in lora test
    QModbusClient *lmRs485ModbuDevice;
    QModbusServer *lcModbusDevice;
    int index;
    int flagSwitch;
    int canNotFixFlag;
    QString modelName;
    quint8 tableIndex;
};

#endif // MAINWINDOW_H
