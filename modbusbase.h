#ifndef MODBUSWRITEBASE_H
#define MODBUSWRITEBASE_H

#include "modbustable.h"
#include "ui_mainwindow.h"
#include "comparevalue.h"
#include "init.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QWidget>

class ModbusBase : public QObject
{
public:
    ModbusBase();
    Ui::MainWindow *ui;
    compareValue cv;
    int flag;
    int IOValue;
    int AdjustValue;
    bool GoldenSampleCheck;
    bool mbusCellularStatus;

    void writeRegisters(int, int, QModbusClient *);
    void writeRegisters(int, int, QString, QModbusClient *);
    void writeRegisters(int, int, QVector<quint16>, QModbusClient *);
    void readRegisters(int, int, QModbusClient *, void (ModbusBase::*fn)());
    void readRegisters(int, int, int, QModbusClient *, void (ModbusBase::*fn)());
private:
    void processReadUnit(QModbusDataUnit, QString);
    QMainWindow* getMainWindow();
private slots:
    void handleReadSN();
    void handleReadModelName();
    void handleReadVersion();
    void handleReadIOValue();
    void PowerReadReady(int times);
    void handleReadVoltage();
    void handleReadAmper();
    void handleReadPowerFullValue();
    void handleReadPowerHalfValue();
    void handleReadPowerEmptyValue();
    void handleReadAdjustValue();
    void handleSNGoldenSample();
    void handleNBSIM();
    void handleNBRSSI();
    void handleNBSTATUS();
    void handleReadMode();
    void handleReadBatteryVoltage();
    void handleMeterPoll();
    void handleMeterPollStatus();
    void handleSerialReadReady();
    void hanleCellularCheckReadReady();
    void handleMqttCheckNumberReadReady();
    void handleMqttCheckStringReadReady();
    void handleMeterReadReady();
    void handleMeterTagReadReady();
};

#endif // MODBUSWRITEBASE_H
