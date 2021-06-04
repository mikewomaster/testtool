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

quint16 baudrateValue(int baudrate)
{
    if (baudrate == 9600)
        return 0;
    else if (baudrate == 4800)
        return 1;
    else if (baudrate == 2400)
        return 2;
    else if (baudrate == 1200)
        return 3;
    else if (baudrate == 300)
        return 4;
}

quint16 parityValue(QString parity)
{
    if (parity == "no")
        return 0;
    else if (parity == "even")
        return 1;
    else if (parity == "odd")
        return 2;
}

QVector<quint16> mbusPara(Init init)
{
    QVector<quint16> values;
    int baudrate = init.mbusArray[0].toObject()["baudrate"].toInt();
    values.push_back(baudrateValue(baudrate));

    int databits = init.mbusArray[0].toObject()["databits"].toInt();
    values.push_back(databits);

    int stopbits = init.mbusArray[0].toObject()["stopbits"].toInt();
    values.push_back(stopbits);

    QString parity = init.mbusArray[0].toObject()["parity"].toString();
    values.push_back(parityValue(parity));

    int interval = init.mbusArray[0].toObject()["interval"].toInt();
    values.push_back(interval);

    return values;
}

int MainWindow::mbusDefault(Init init)
{
    modbusBase->writeRegisters(mbusStart, 5, mbusPara(init), modbusDevice);
    if (checkFlag(modbusBase->flag) == false) {
        canNotFixFlag = FallFlag;
        modbusBase->flag = FallFlag;
        return 1;
    }
    return 0;
}

typedef struct contentAddress {
    QString str;
    int value;
    int addr;
    int entry;
}ContentAddress;

int MainWindow::cellularDefault(Init init)
{
    ContentAddress CCA[4];

    QString apn = init.cellularArray[0].toObject()["apn"].toString();
    CCA[0].str = apn;
    CCA[0].addr = cellularAPNAddress;

    QString username = init.cellularArray[0].toObject()["username"].toString();
    CCA[1].str = username;
    CCA[1].addr = cellularUserName;

    QString password = init.cellularArray[0].toObject()["password"].toString();
    CCA[2].str = password;
    CCA[2].addr = cellularPassWord;

    QString plmn = init.cellularArray[0].toObject()["plmn"].toString();
    CCA[3].str = plmn;
    CCA[3].addr = cellularPLMN;

    for (int i = 0; i < 4; i++) {
        modbusBase->writeRegisters(CCA[i].addr, EightEntries, CCA[i].str, modbusDevice);
        if (checkFlag(modbusBase->flag) == false) {
            canNotFixFlag = FallFlag;
            modbusBase->flag = FallFlag;
            return 1;
        }
        _sleep();
    }

    return 0;
}

int MainWindow::mqttDefault(Init init)
{
    contentAddress MCA[8];
    QString server = init.mqttArray[0].toObject()["server"].toString();
    MCA[0].str = server;
    MCA[0].value = 0;
    MCA[0].addr = mqttIPAddress;
    MCA[0].entry = mqttIPEntries;

    int port = init.mqttArray[0].toObject()["port"].toInt();
    MCA[1].str = "";
    MCA[1].value = port;
    MCA[1].addr = mqttPortAddress;
    MCA[1].entry = mqttPortEntries;

    QString pubTopic = init.mqttArray[0].toObject()["pubtopical"].toString();
    MCA[2].str = pubTopic;
    MCA[2].value = 0;
    MCA[2].addr = mqttTopicAddress;
    MCA[2].entry = mqttTopicEntreis;

    QString subTopic = init.mqttArray[0].toObject()["subtopical"].toString();
    MCA[3].str = subTopic;
    MCA[3].value = 0;
    MCA[3].addr = MQTTSubTopicAddress;
    MCA[3].entry = MQTTSubTopicEntries;

    QString clientId = init.mqttArray[0].toObject()["clientid"].toString();
    MCA[4].str = clientId;
    MCA[4].value = 0;
    MCA[4].addr = mqttClinetIDAddress;
    MCA[4].entry = mqttClinetIDEntries;

    QString username = init.mqttArray[0].toObject()["username"].toString();
    MCA[5].str = username;
    MCA[5].value = 0;
    MCA[5].addr = mqttUserAddress;
    MCA[5].entry = mqttUserEntries;

    QString password = init.mqttArray[0].toObject()["password"].toString();
    MCA[6].str = password;
    MCA[6].value = 0;
    MCA[6].addr = mqttPassWordAddress;
    MCA[6].entry = mqttPassWordEntries;

    int interval = init.mqttArray[0].toObject()["interval"].toInt();
    MCA[7].str = "";
    MCA[7].value = interval;
    MCA[7].addr = mqttIntervalAddress;
    MCA[7].entry = mqttIntervalEntries;

    for (int i = 0; i < 8; i++) {
        if (i == 1)
        {
            modbusBase->writeRegisters(MCA[i].addr, MCA[i].value, modbusDevice);
            if (checkFlag(modbusBase->flag) == false) {
                canNotFixFlag = FallFlag;
                modbusBase->flag = FallFlag;
                return 1;
            }
        }
        else if (i == 7)
        {
            QVector<quint16> valuesVector;
            valuesVector.push_back(0x0000);
            valuesVector.push_back(MCA[i].value);
            modbusBase->writeRegisters(MCA[i].addr, 2, valuesVector, modbusDevice);
            if (checkFlag(modbusBase->flag) == false) {
                canNotFixFlag = FallFlag;
                modbusBase->flag = FallFlag;
                return 1;
            }
        }
        else
        {
            modbusBase->writeRegisters(MCA[i].addr, MCA[i].entry, MCA[i].str, modbusDevice);
            if (checkFlag(modbusBase->flag) == false) {
                canNotFixFlag = FallFlag;
                modbusBase->flag = FallFlag;
                return 1;
            }
        }

        _sleep();
    }

    return 0;
}

int addressModelType(QString str)
{
    if (str == "primary")
        return 1;
    else if (str == "secondary")
        return 2;
}

QVector<quint16> MainWindow::meterHeadModbusUnit(QString str1, int entry1, quint16 number, QString str2, int entry2)
{
    QVector<quint16> values;
    int i = 0;
    for (i = 0; i < str1.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str1.at(i - 1).toLatin1();
            temp = (temp << 8) + str1.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2 && i) {
        quint16 temp = str1.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < entry1; i++) {
        values.push_back(0x0000);
    }

    // push back int
    values.push_back(number);

    // push back value2
    i = 0;
    for (i = 0; i < str2.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str2.at(i - 1).toLatin1();
            temp = (temp << 8) + str2.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2 && i) {
        quint16 temp = str2.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    for (i = (i / 2); i < entry2; i++)
    {
        values.push_back(0x0000);
    }

    return values;
}

int MainWindow::mbusMeterDefault(Init init)
{
    for (int i = 0; i < init.meterArray.size(); i++) {
        int times = init.meterArray[i].toObject()["number"].toInt();

        QString model = init.meterArray[i].toObject()["model"].toString();
        quint16 type = addressModelType(init.meterArray[i].toObject()["addressModel"].toString());
        QString address = init.meterArray[i].toObject()["address"].toString();

        ui->resultText->append("set: " + model + " " + QString::number(type) + " " + address);
        QVector<quint16> valueArray = meterHeadModbusUnit(model, EightEntries, type, address, EightEntries);
        modbusBase->writeRegisters(meterModelBaseAddress + (times - 1) * meterGap, 17, valueArray, modbusDevice);

        _sleep();
    }
}

int MainWindow::mbusMeterTagDefault(Init init)
{
    for (int i = 0; i < init.meterTagArray.size(); i++) {
        int times = init.meterTagArray[i].toObject()["numberOfMeter"].toInt();
        int tagTimes = init.meterTagArray[i].toObject()["numberOfTag"].toInt();

        QString attribute = init.meterTagArray[i].toObject()["attributeName"].toString();
        quint16 index = init.meterTagArray[i].toObject()["dataIndex"].toInt();
        QString magnitue = init.meterTagArray[i].toObject()["magnitude"].toString();

        ui->resultText->append("set: " + attribute + " " + QString::number(index) + " " + magnitue);
        QVector<quint16> valueArray = meterHeadModbusUnit(attribute, 4, index, magnitue, 4);
        modbusBase->writeRegisters(meterTagBaseAddress + (times - 1) * meterGap + (tagTimes - 1) * 9, 9, valueArray, modbusDevice);

        _sleep();
    }
}

void MainWindow::mbusLoadModelSN()
{
    /*
     *  1. model name
     *
     * QJsonObject arrObj = init->Array[index].toObject();
     * QString str = arrObj["Model Name"].toString();
     */
    if (ui->modelNameSl102CheckBox->isChecked()) {
         if (ui->modelNameSl102ComBox->currentText() == "") {
             QMessageBox::question(this, "Model Name", "Please Check ModelName which has no value!!!");
             return;
         }

        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
         if (1) {
            QString str = ui->modelNameSl102ComBox->currentText();
            ui->resultText->append("Model Name: " + str);
            modbusBase->cv.modelName = str;
            modbusBase->writeRegisters(ModelNameAddr, ModelNameEntires, str, modbusDevice);
            _sleep(2000);
            if (checkFlag(modbusBase->flag) == false) {
                modbusBase->flag = FallFlag;
                ui->modelSL102Label->setText(msg);
                return;
            } else {
                modbusBase->readRegisters(ModelNameAddr, ModelNameEntires, modbusDevice, &(modbusBase->handleReadModelName));
                _sleep(2000);
                if (checkFlag(modbusBase->flag) == false) {
                    modbusBase->flag = FallFlag;
                    ui->modelSL102Label->setText(msg);
                    return;
                }
                msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
                ui->modelSL102Label->setText(msg);
            }
         } else {
             ui->modelSL102Label->setText(msg);
         }
     }

    /*
     *  2. SN
     *  arrObj = init->Array[index].toObject();
     *  str = arrObj["SN"].toString();
     */
    if (ui->SNSl102CheckBox->isChecked()) {
        if (ui->SNSl102LineEdit->text() == "") {
            QMessageBox::question(this, "SN", "Please Check SN which has no value!!!");
            return;
        }

        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
        if (1) {
            QString str = ui->SNSl102LineEdit->text();
            ui->resultText->append("SN: " + str);
            modbusBase->cv.SN = str;
            modbusBase->writeRegisters(SNAddr, SNEntries, str, modbusDevice);
            _sleep(2000);
            if (checkFlag(modbusBase->flag) == false) {
                ui->SNSl102Label->setText(msg);
                modbusBase->flag = 0;
                return;
            } else {
                modbusBase->readRegisters(SNAddr, SNEntries, modbusDevice, &(modbusBase->handleReadSN));
                _sleep(2000);
                if (checkFlag(modbusBase->flag) == false) {
                    ui->SNSl102Label->setText(msg);
                    modbusBase->flag = FallFlag;
                    return;
                }
                msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
                ui->SNSl102Label->setText(msg);
            }
        } else {
            ui->SNSl102Label->setText(msg);
        }
        // ui->SNSl102LineEdit->clear();
    }
}

void MainWindow::mbusLoadDefaultStart()
{
    Init init(1);

    int res;

    // ui->resultText->append("----------------------------");
    // mbusLoadModelSN();

    ui->resultText->append("mbus write parameters start");
    if (mbusDefault(init))
        return;
    ui->resultText->append("----------------------------");

    ui->resultText->append("cellular write parameters start");
    if (cellularDefault(init))
        return;
    ui->resultText->append("----------------------------");

    ui->resultText->append("mqtt write parameters start");
    if (res = mqttDefault(init))
        return;
    ui->resultText->append("----------------------------");

    ui->resultText->append("mbus meter write parameters start");
    if (mbusMeterDefault(init))
        return;
    ui->resultText->append("----------------------------");

    ui->resultText->append("mbus meter tag parameters start");
    if (mbusMeterTagDefault(init))
        return;
    ui->resultText->append("----------------------------");

    QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
    ui->mbusLoadLabel->setText(msg);
}
