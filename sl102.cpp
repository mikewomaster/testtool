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

void MainWindow::on_connectSl102PushButton_clicked()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
            modbusDevice->disconnectDevice();
            ui->connectSl102PushButton->setText(tr("Connect"));
            ui->versionSl102LineEdit->clear();
    } else {
            modelName = ui->modelNameComBox->currentText();
            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->serialSl102ComboBox->currentText());
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            modbusDevice->setTimeout(1000);
            modbusDevice->setNumberOfRetries(1);
            if (modbusDevice->connectDevice())
                ui->connectSl102PushButton->setText(tr("Disconnect"));
    }

   modbusBase->readRegisters(VersionAddress, VersionEntries, modbusDevice, &(modbusBase->handleReadVersion));
   _sleep();
}

void MainWindow::prepareAndSendModbus(int addr, int entry, QString cnt, QString msg)
{
    if (!cnt.isEmpty()) {
        ui->resultText->append(msg);
        modbusBase->writeRegisters(addr, entry, cnt, modbusDevice);
    }
    _sleep(2000);
}

void MainWindow::prepareAndSendModbus(int addr, int entry, int val, QString msg)
{

    ui->resultText->append(msg);
    QVector<quint16> valueToVector;
    valueToVector.push_back(val);
    for (int i = 0; i < entry - 1; i++) {
        valueToVector.push_front(0); // confuse for big/little endian
    }

    modbusBase->writeRegisters(addr, entry, valueToVector, modbusDevice);
    _sleep(2000);
}

void MainWindow::setUILabelInfoEachTIme(QLabel * lb)
{
    if (checkFlag(modbusBase->flag) == false) {
        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
        lb->setText(msg);
    }
}

void MainWindow::setUILabelInfo(QLabel* lb)
{
    if (checkFlag(modbusBase->flag) == false) {
        canNotFixFlag = FallFlag;
        modbusBase->flag = 0;
        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
        lb->setText(msg);
        return;
    }
    QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
    lb->setText(msg);
}

void MainWindow::on_functionSl102StartPushButton_clicked()
{
    ui->resultText->clear();

    ui->cellularSl102Label->clear();
    ui->mqttSl102Label->clear();
    ui->ledControlSl102Label->clear();
    ui->nbConnectSl102Label->clear();
    ui->modelSL102Label->clear();
    ui->SNSl102Label->clear();

    Init init;

    ui->resultText->append("------------------------");
    ui->resultText->append("---Start Product Test---");

    QDateTime time = QDateTime::currentDateTime();
    QString StrCurrentTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
    ui->resultText->append(StrCurrentTime);

    /*
     * 1. reset
    */
    if (NeedResetFirst) {
        ui->resultText->append("Reset result:");
        modbusBase->writeRegisters(ResetEnableAddress, 1, modbusDevice);
        _sleep();
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = 0;
            return;
        }
    }

    /*
     *  2. model name
     *
     * QJsonObject arrObj = init->Array[index].toObject();
     * QString str = arrObj["Model Name"].toString();
     */
    if (ui->modelNameSl102CheckBox->isChecked()){
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
     *  3. SN
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
        ui->SNSl102LineEdit->clear();
    }

    // 4. led control
    if (ui->ledControlSl102CheckBox->isChecked()) {
        ui->resultText->append("------------------");
        int valueLedPercent = 100;
        prepareAndSendModbus(controlTestOnAddress, OneEntry, valueLedPercent, "set led on");
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }
        ui->resultText->append("Please wait 5s for collecting power data...");
        _sleep(6000);
        modbusBase->readRegisters(EnergyPowerAddress, OneEntry, modbusDevice, &(modbusBase->handleReadPowerFullValue));
        _sleep(2000);
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }

        modbusBase->readRegisters(EnergyVRMSAddress, OneEntry, modbusDevice, &(modbusBase->handleReadVoltage));
        _sleep(2000);
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }

        modbusBase->readRegisters(EnergyIRMSAddress, OneEntry, modbusDevice, &(modbusBase->handleReadAmper));
        _sleep(2000);
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }

        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }
        ui->resultText->append("------------------");

        ui->resultText->append("------------------");
        valueLedPercent = 50;
        prepareAndSendModbus(controlTestOnAddress, OneEntry, valueLedPercent, "set led-half on");
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }
        ui->resultText->append("Please wait 5s for collecting power data...");
        _sleep(6000);
        modbusBase->readRegisters(EnergyPowerAddress, OneEntry, modbusDevice, &(modbusBase->handleReadPowerHalfValue));
        _sleep(2000);
        modbusBase->readRegisters(EnergyVRMSAddress, OneEntry, modbusDevice, &(modbusBase->handleReadVoltage));
        _sleep(2000);
        modbusBase->readRegisters(EnergyIRMSAddress, OneEntry, modbusDevice, &(modbusBase->handleReadAmper));
        _sleep(2000);
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }
        ui->resultText->append("------------------");

        ui->resultText->append("------------------");
        valueLedPercent = 0;
        prepareAndSendModbus(controlTestOnAddress, OneEntry, valueLedPercent, "set led off");
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }
        ui->resultText->append("Please wait 5s for collecting power data...");
        _sleep(6000);
        modbusBase->readRegisters(EnergyPowerAddress, OneEntry, modbusDevice, &(modbusBase->handleReadPowerEmptyValue));
        _sleep(2000);
        modbusBase->readRegisters(EnergyVRMSAddress, OneEntry, modbusDevice, &(modbusBase->handleReadVoltage));
        _sleep(2000);
        modbusBase->readRegisters(EnergyIRMSAddress, OneEntry, modbusDevice, &(modbusBase->handleReadAmper));
        _sleep(2000);
        setUILabelInfoEachTIme(ui->ledControlSl102Label);
        if (ui->ledControlSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->ledControlSl102Label);
            return;
        }
        ui->resultText->append("------------------");

        setUILabelInfo(ui->ledControlSl102Label);
    }

    // 5. nb connect
    if (ui->nbConnectSl102CheckBox->isChecked()) {
        int nbConnectValue = 1;
        prepareAndSendModbus(NBConnTestAddress, OneEntry, nbConnectValue, "set nbiot on, please wait before check ");
        setUILabelInfoEachTIme(ui->nbConnectSl102Label);
        if (ui->nbConnectSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->nbConnectSl102Label);
            return;
        }
        QMessageBox::question(this, "NB Test", "Please wait before check nb connection status");

        ui->resultText->append("");
        for (int i = 0; i < init.configArray[0].toObject()["nbDelayTime"].toInt(); i++) {
            int j = init.configArray[0].toObject()["nbDelayTime"].toInt() - i;
            QString msg = "Remaing time: " + QString::number(j) + " ...";
            ui->resultText->insertPlainText(msg);
            _sleep(1000);

            QTextCursor txc(ui->resultText->textCursor());
            txc.movePosition(QTextCursor::StartOfLine);
            txc.select(QTextCursor::LineUnderCursor);
            txc.removeSelectedText();
        }

        modbusBase->readRegisters(NBSIMStatus, OneEntry, modbusDevice, &(modbusBase->handleNBSTATUS));
        _sleep(3500);
        setUILabelInfoEachTIme(ui->nbConnectSl102Label);
        if (ui->nbConnectSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->nbConnectSl102Label);
            return;
        }

        modbusBase->readRegisters(NBStatusAddress, OneEntry, modbusDevice, &(modbusBase->handleNBSIM));
        _sleep(3500);
        setUILabelInfoEachTIme(ui->nbConnectSl102Label);
        if (ui->nbConnectSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->nbConnectSl102Label);
            return;
        }

        modbusBase->readRegisters(NBRSSIAddress, OneEntry, modbusDevice, &(modbusBase->handleNBRSSI));
        _sleep(3500);
        setUILabelInfoEachTIme(ui->nbConnectSl102Label);
        if (ui->nbConnectSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->nbConnectSl102Label);
            return;
        }

        setUILabelInfo(ui->nbConnectSl102Label);
    }

    _sleep(5000);
    on_savePushButton_clicked();
}

void MainWindow::on_setPushButton_clicked()
{
    Init init;

    // cellular config
    if (ui->cellularSl102CheckBox->isChecked()){
        QString strOfCellular = init.cellularArray[0].toObject()["apn"].toString();
        prepareAndSendModbus(APNAddress, APNEntries, strOfCellular, "set apn of cellular:");
        setUILabelInfoEachTIme(ui->cellularSl102Label);
        if (ui->cellularSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->cellularSl102Label);
            return;
        }

        strOfCellular = init.cellularArray[0].toObject()["username"].toString();
        prepareAndSendModbus(UserAddress, UserEntries, strOfCellular, "set username of cellular:");
        setUILabelInfoEachTIme(ui->cellularSl102Label);
        if (ui->cellularSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->cellularSl102Label);
            return;
        }

        strOfCellular = init.cellularArray[0].toObject()["password"].toString();
        prepareAndSendModbus(NBPwdAddress, NBPwdEntries, strOfCellular, "set password of cellular:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->cellularSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->cellularSl102Label);
            return;
        }

        strOfCellular = init.cellularArray[0].toObject()["plmn"].toString();
        prepareAndSendModbus(NBPLMNAddress, NBPLMNEntries, strOfCellular, "set plmn of cellular:");
        setUILabelInfoEachTIme(ui->cellularSl102Label);
        if (ui->cellularSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->cellularSl102Label);
            return;
        }

        setUILabelInfo(ui->cellularSl102Label);
    }

    // mqtt config
    if (ui->mqttSl102CheckBox->isChecked()) {
        QString strOfMqtt = init.mqttArray[0].toObject()["server"].toString();
        prepareAndSendModbus(mqttIPAddress, mqttIPEntries, strOfMqtt, "set ip server of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
         }

        int numOfMqtt = init.mqttArray[0].toObject()["port"].toInt();
        prepareAndSendModbus(mqttPortAddress, mqttPortEntries, numOfMqtt, "set port of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        strOfMqtt = init.mqttArray[0].toObject()["pubtopical"].toString();
        prepareAndSendModbus(mqttTopicAddress, mqttTopicEntreis, strOfMqtt, "set publish topical of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        strOfMqtt = init.mqttArray[0].toObject()["subtopical"].toString();
        prepareAndSendModbus(MQTTSubTopicAddress, MQTTSubTopicEntries, strOfMqtt, "set sublical topical of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        strOfMqtt = init.mqttArray[0].toObject()["clientid"].toString();
        prepareAndSendModbus(mqttClinetIDAddress, mqttClinetIDEntries, strOfMqtt, "set client id of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        strOfMqtt = init.mqttArray[0].toObject()["username"].toString();
        prepareAndSendModbus(mqttUserAddress, mqttUserEntries, strOfMqtt, "set username of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        strOfMqtt = init.mqttArray[0].toObject()["password"].toString();
        prepareAndSendModbus(mqttPassWordAddress, mqttPassWordEntries, strOfMqtt, "set password of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        int numOfInterval = init.mqttArray[0].toObject()["interval"].toInt();
        prepareAndSendModbus(mqttIntervalAddress, mqttIntervalEntries, numOfInterval, "set interval of mqtt:");
        setUILabelInfoEachTIme(ui->mqttSl102Label);
        if (ui->mqttSl102Label->text().contains("FAIL")) {
            setUILabelInfo(ui->mqttSl102Label);
            return;
        }

        setUILabelInfo(ui->mqttSl102Label);
    }

    _sleep(5000);
    on_savePushButton_clicked();
}
