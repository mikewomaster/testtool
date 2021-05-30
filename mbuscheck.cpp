#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modbustable.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>

static Init jsonValue(1);

int switchBaudRateValue(quint16 value)
{
    if (value == 0)
        return 9600;
    else if (value == 1)
        return 4800;
    else if (value == 2)
        return 2400;
    else if (value == 3)
        return 1200;
    else
        return 300;
}

QString switchParityValue(quint16 val)
{
    if (val == 0)
        return "no";
    else if (val == 1)
        return  "even";
    else if (val == 2)
        return "odd";
}

int checkSerialResult(const QModbusDataUnit unit)
{
    if (jsonValue.mbusArray[0].toObject()["baudrate"].toInt() != switchBaudRateValue(unit.value(0)))
        return 1;

    if (jsonValue.mbusArray[0].toObject()["databits"].toInt() != unit.value(1))
        return 1;

    if (jsonValue.mbusArray[0].toObject()["stopbits"].toInt() != unit.value(2))
        return 1;

    if (jsonValue.mbusArray[0].toObject()["parity"].toString() != switchParityValue(unit.value(3)))
        return 1;

    if (jsonValue.mbusArray[0].toObject()["interval"].toInt() != unit.value(4))
        return 1;

    return 0;
}

void ModbusBase::handleSerialReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        if (checkSerialResult(unit)) {
            flag == RaiseFlag;
            return;
        }

        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

int MainWindow::mbusSerialCheck()
{
    modbusBase->readRegisters(mbusStart, 5, modbusDevice, &(modbusBase->handleSerialReadReady));

    if (checkFlag(modbusBase->flag) == false) {
        modbusBase->flag = FallFlag;
        ui->resultText->append("check mbus serial paramets fail!");
        return 1;
    }

    ui->resultText->append("check mbus serial paramets success!");
    return 0;
}

typedef struct contentAddress {
    QString str;
    int value;
    int addr;
    int entry;
}ContentAddress;

static int cellularCheck;
static ContentAddress CCA[4];

void ModbusBase::hanleCellularCheckReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        QString s;
        for (uint i = 0; i < unit.valueCount(); i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;

            if ((unit.value(i) & 0x00ff) == 0x00) {
                break;
            }

            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }

        if (s.isEmpty() || s != CCA[cellularCheck].str){
            flag = RaiseFlag;
        }else {
            flag = FallFlag;
        }

        ui->resultText->append("Read: " + s);
        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        flag = RaiseFlag;
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        flag = RaiseFlag;
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

int MainWindow::mbusCellularCheck()
{
    QString apn = jsonValue.cellularArray[0].toObject()["apn"].toString();
    CCA[0].str = apn;
    CCA[0].addr = cellularAPNAddress;

    QString username = jsonValue.cellularArray[0].toObject()["username"].toString();
    CCA[1].str = username;
    CCA[1].addr = cellularUserName;

    QString password = jsonValue.cellularArray[0].toObject()["password"].toString();
    CCA[2].str = password;
    CCA[2].addr = cellularPassWord;

    QString plmn = jsonValue.cellularArray[0].toObject()["plmn"].toString();
    CCA[3].str = plmn;
    CCA[3].addr = cellularPLMN;

    for(cellularCheck = 0; cellularCheck < 4; cellularCheck++) {
        modbusBase->readRegisters(CCA[cellularCheck].addr, 8, modbusDevice, &(modbusBase->hanleCellularCheckReadReady));
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = FallFlag;
            ui->resultText->append("check mbus cellular paramets fail!");
            return 1;
        }

        ui->resultText->append("check mbus cellular paramets success!");
        return 0;
    }
}

static int mqttCheck;
static ContentAddress MCA[8];

void ModbusBase::handleMqttCheckNumberReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        int result;
        if (unit.valueCount() == 1)
            result = unit.value(0);
        else if (unit.valueCount() == 2) {
            result = unit.value(0);
            result = (result << 16) + unit.value(1);
        }

        if (result != MCA[mqttCheck].value){
            flag = RaiseFlag;
        }else {
            flag = FallFlag;
        }

        ui->resultText->append("Read: " + QString::number(result) + "fail.");
        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        flag = RaiseFlag;
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        flag = RaiseFlag;
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void ModbusBase::handleMqttCheckStringReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        QString s;
        for (uint i = 0; i < unit.valueCount(); i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;

            if ((unit.value(i) & 0x00ff) == 0x00) {
                break;
            }

            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }

        if (s.isEmpty() || s != MCA[mqttCheck].str){
            flag = RaiseFlag;
        }else {
            flag = FallFlag;
        }

        ui->resultText->append("Read: " + s);
        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        flag = RaiseFlag;
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        flag = RaiseFlag;
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

int MainWindow::mbusMqttCheck()
{
    QString server = jsonValue.mqttArray[0].toObject()["server"].toString();
    MCA[0].str = server;
    MCA[0].value = 0;
    MCA[0].addr = mqttIPAddress;
    MCA[0].entry = mqttIPEntries;

    int port = jsonValue.mqttArray[0].toObject()["port"].toInt();
    MCA[1].str = "";
    MCA[1].value = port;
    MCA[1].addr = mqttPortAddress;
    MCA[1].entry = mqttPortEntries;

    QString publish = jsonValue.mqttArray[0].toObject()["pubtopical"].toString();
    MCA[2].str = publish;
    MCA[2].value = 0;
    MCA[2].addr = mqttTopicAddress;
    MCA[2].entry = mqttTopicEntreis;

    QString sub = jsonValue.mqttArray[0].toObject()["subtopical"].toString();
    MCA[3].str = sub;
    MCA[3].value = 0;
    MCA[3].addr = MQTTSubTopicAddress;
    MCA[3].entry = MQTTSubTopicEntries;

    QString clientID = jsonValue.mqttArray[0].toObject()["clientid"].toString();
    MCA[4].str = clientID;
    MCA[4].value = 0;
    MCA[4].addr = mqttClinetIDAddress;
    MCA[4].entry = mqttClinetIDEntries;

    QString username = jsonValue.mqttArray[0].toObject()["usernmae"].toString();
    MCA[5].str = username;
    MCA[5].value = 0;
    MCA[5].addr = mqttUserAddress;
    MCA[5].entry = mqttUserEntries;

    QString password = jsonValue.mqttArray[0].toObject()["password"].toString();
    MCA[6].str = password;
    MCA[6].value = 0;
    MCA[6].addr = mqttPassWordAddress;
    MCA[6].entry = mqttPassWordEntries;

    int interval = jsonValue.mqttArray[0].toObject()["interval"].toInt();
    MCA[7].str = "";
    MCA[7].value = interval;
    MCA[7].addr = mqttIntervalAddress;
    MCA[7].entry = mqttIntervalEntries;

    for (mqttCheck = 0; mqttCheck < 7; mqttCheck ++) {
        if (mqttCheck == 0 || mqttCheck == 7)
            modbusBase->readRegisters(MCA[mqttCheck].addr, MCA[mqttCheck].entry, modbusDevice, &(modbusBase->handleMqttCheckNumberReadReady));
        else
            modbusBase->readRegisters(MCA[mqttCheck].addr, MCA[mqttCheck].entry, modbusDevice, &(modbusBase->handleMqttCheckStringReadReady));

        _sleep();
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = FallFlag;
            ui->resultText->append("check mqtt cellular paramets fail!");
            return 1;
        }
        ui->resultText->append("check mqtt cellular done!");
    }

    ui->resultText->append("check mqtt parameters successful!");
    return 0;
}

static ContentAddress MeCA[3];

void ModbusBase::handleMeterReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        QString s;
        for (uint i = 0; i < 8; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        if (s != MeCA[0].str){
            ui->resultText->append("Read: " + s + " Faile!");
            flag = RaiseFlag;
        } else {
            ui->resultText->append("Read: " + s + " Done!");
        }
        s.clear();

        if (unit.value(8) != MeCA[1].value){
            ui->resultText->append("Read: " + QString::number(unit.value(8)) + " Faile!");
            flag = RaiseFlag;
        } else {
            ui->resultText->append("Read: " + QString::number(unit.value(8)) + " Done!");
        }

        for (uint i = 9; i < 16; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        s.remove(' ');
        if (s != MeCA[2].str){
            ui->resultText->append("Read: " + s + " Faile!");
            flag = RaiseFlag;
        } else {
            ui->resultText->append("Read: " + s + " Done!");
        }
        s.clear();

        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

int switchModelToInt(QString type)
{
    if (type == "primary")
        return 1;
    else if (type == "secondary")
        return 2;
}

int MainWindow::mbusMeterCheck()
{
    for (int i = 0; i < jsonValue.meterArray.size(); i++) {
        int times = jsonValue.meterArray[i].toObject()["number"].toInt();
        MeCA[0].str = jsonValue.meterArray[i].toObject()["model"].toString();
        MeCA[1].value = switchModelToInt(jsonValue.meterArray[i].toObject()["addressModel"].toString());
        MeCA[2].str = jsonValue.meterArray[i].toObject()["address"].toString();

        modbusBase->readRegisters(meterModelBaseAddress + meterGap * times, 17, modbusDevice, &(modbusBase->handleMeterReadReady));
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = FallFlag;
            ui->resultText->append("check meter parameters paramets fail!");
            return 1;
        }
        _sleep();
    }

    ui->resultText->append("check meter parameters successful!");
    return 0;
}

static ContentAddress MtCA[3];

void ModbusBase::handleMeterTagReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        QString s;
        for (uint i = 0; i < 4; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        if (s != MtCA[0].str) {
            ui->resultText->append("Read: " + s + "fail.");
            flag = RaiseFlag;
        }
        s.clear();

        if (unit.value(4) != MtCA[1].value) {
            ui->resultText->append("Read: " + QString::number(unit.value(4)) + "fail.");
            flag = RaiseFlag;
        }

        for (uint i = 5; i < 8; i++) {
            if ((unit.value(i) >> 8) == 0x00)
                break;
            s[2*i] = unit.value(i) >> 8;
            if ((unit.value(i) & 0x00ff) == 0x00)
                break;
            s[(2*i) +1] = unit.value(i) & 0x00ff;
        }
        s.remove('\"');
        s.remove(' ');
        if (s != MtCA[2].str) {
            ui->resultText->append("Read: " + s + "fail.");
            flag = RaiseFlag;
        }
        s.clear();

        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

int MainWindow::mbusMeterTagCheck()
{
    for (int i = 0; i < jsonValue.meterTagArray.size(); i++){
        int meterNo = jsonValue.meterArray[i].toObject()["numberOfMeter"].toInt();
        int meterTagNo = jsonValue.meterArray[i].toObject()["numberOfTag"].toInt();

        MtCA[0].str = jsonValue.meterArray[i].toObject()["attributeName"].toString();
        MtCA[1].value = jsonValue.meterArray[i].toObject()["dataIndex"].toInt();
        MtCA[2].str = jsonValue.meterArray[i].toObject()["attributeName"].toString();

        modbusBase->readRegisters(meterTagBaseAddress + meterGap * meterNo + (9 * meterTagNo), 9, modbusDevice, &(modbusBase->handleMeterTagReadReady));
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = FallFlag;
            ui->resultText->append("check meter tag parameters paramets fail!");
            return 1;
        }
        _sleep();
    }

    ui->resultText->append("check meter tag parameters successful!");
    return 0;
}

void MainWindow::mbusCheckStart()
{
    ui->resultText->append("mbus read&check parameters start");

    if(mbusSerialCheck())
        return;
    _sleep();

    if (mbusCellularCheck())
        return
    _sleep();

    if (mbusMqttCheck())
        return;
    _sleep();

    if(mbusMeterCheck())
        return;
    _sleep();

    if (mbusMeterTagCheck())
        return;
    _sleep();
}
