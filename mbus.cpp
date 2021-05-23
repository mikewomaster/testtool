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
    sl102Connect();
    modbusBase->readRegisters(systemModeAddress, OneEntry, modbusDevice, &modbusBase->handleReadMode);
}

void MainWindow::on_mbusModeComboBox_currentIndexChanged(int index)
{
    modbusBase->writeRegisters(systemModeAddress, index, modbusDevice);
    _sleep();
    modbusBase->readRegisters(systemModeAddress, OneEntry, modbusDevice, &modbusBase->handleReadMode);
}

void MainWindow::on_testOperationCheckBox_stateChanged(int arg1){}

void MainWindow::mbusTestStart()
{
    // if (modbusDevice->state() == QModbusDevice::UnconnectedState) {
        // ui->resultText->append("Please Connect Serial Firstly!");
        // return;
    // }

    ui->resultText->clear();

    ui->mbusCellularLabel->clear();
    ui->mbusMQTTLabel->clear();
    ui->mbusTestLabel->clear();
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

    // 4. battery voltage test
    modbusBase->readRegisters(mBusVoltageAddress, OneEntry, modbusDevice, &modbusBase->handleReadBatteryVoltage);
    if (checkFlag(modbusBase->flag) == false) {
        modbusBase->flag = FallFlag;
        return;
    }

    // 5. nb
    if (ui->baseStationCheckBox->isChecked()) {
        // set nb configure
        QString nbAPN = init.cellularArray[0].toObject()["apn"].toString();
        QString nbUserName = init.cellularArray[0].toObject()["username"].toString();
        QString nbPassWord = init.cellularArray[0].toObject()["password"].toString();
        QString nbPlmn = init.cellularArray[0].toObject()["plmn"].toString();

        modbusBase->writeRegisters(cellularAPNAddress, EightEntries, nbAPN, modbusDevice);
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = 0;
            return;
        }
        _sleep(2000);

        modbusBase->writeRegisters(cellularUserName, EightEntries, nbUserName, modbusDevice);
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = 0;
            return;
        }
        _sleep(2000);

        modbusBase->writeRegisters(cellularPassWord, EightEntries, nbPassWord, modbusDevice);
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = 0;
            return;
        }
        _sleep(2000);

        modbusBase->writeRegisters(cellularPLMN, EightEntries, nbPlmn, modbusDevice);
        if (checkFlag(modbusBase->flag) == false) {
            modbusBase->flag = 0;
            return;
        }
        _sleep(2000);

        // nb connect
        int nbConnectValue = 1;
        prepareAndSendModbus(NBConnTestAddress, OneEntry, nbConnectValue, "set nbiot on, please wait before check ");
        setUILabelInfoEachTIme(ui->mbusCellularLabel);
        if (ui->mbusCellularLabel->text().contains("FAIL")) {
            setUILabelInfo(ui->mbusCellularLabel);
            return;
        }
        QMessageBox::question(this, "NB Test", "Please wait before check nb connection status");
        ui->resultText->append("");

        int nbCheckTime = init.cellularArray[0].toObject()["nbCheckTime"].toInt();
        int nbDelayTime = init.cellularArray[0].toObject()["nbDelayTime"].toInt();

        int checkTime = 0;
        for (int i = 0; i < nbDelayTime; i ++) {
            checkTime ++;
            if (nbCheckTime % checkTime == 0) {
                modbusBase->readRegisters(NBSIMStatus, OneEntry, modbusDevice, &(modbusBase->handleNBSTATUS));
                _sleep(2000);
                setUILabelInfoEachTIme(ui->mbusCellularLabel);
                if (ui->mbusCellularLabel->text().contains("FAIL")) {
                    setUILabelInfo(ui->mbusCellularLabel);
                    return;
                }

                modbusBase->readRegisters(NBStatusAddress, OneEntry, modbusDevice, &(modbusBase->handleNBSIM));
                _sleep(2000);
                setUILabelInfoEachTIme(ui->mbusCellularLabel);
                if (ui->mbusCellularLabel->text().contains("FAIL")) {
                    setUILabelInfo(ui->mbusCellularLabel);
                    return;
                }

                modbusBase->readRegisters(NBRSSIAddress, OneEntry, modbusDevice, &(modbusBase->handleNBRSSI));
                _sleep(2000);
                setUILabelInfoEachTIme(ui->mbusCellularLabel);
                if (ui->mbusCellularLabel->text().contains("FAIL")) {
                    setUILabelInfo(ui->mbusCellularLabel);
                    return;
                }
            }

            if (modbusBase->mbusCellularStatus) {
                modbusBase->mbusCellularStatus = false;
                break;
            }

            if (checkTime >= nbCheckTime)
                checkTime == 0;

            QString msg = "Remaing time: " + QString::number(nbCheckTime - checkTime) + " ...";
            ui->resultText->insertPlainText(msg);
            QTextCursor txc(ui->resultText->textCursor());
            txc.movePosition(QTextCursor::StartOfLine);
            txc.select(QTextCursor::LineUnderCursor);
            txc.removeSelectedText();

            _sleep(1000);
        }
        setUILabelInfo(ui->mbusCellularLabel);
    }

    // m-bus
    // m-bus set
    ui->resultText->append("Set meter parameters ... ");

    QString meterModel = init.meterArray[0].toObject()["model"].toString();
    quint16 meterAddressMode = init.meterArray[0].toObject()["addressModel"].toInt();
    quint16 meterAddress = init.meterArray[0].toObject()["address"].toInt();

    modbusBase->writeRegisters(meterModelBaseAddress, EightEntries, meterModel, modbusDevice);
    if (checkFlag(modbusBase->flag) == false) {
        modbusBase->flag = 0;
        return;
    }
    _sleep(2000);

    modbusBase->writeRegisters(addressModeBaseAddress, meterAddressMode, modbusDevice);
    if (checkFlag(modbusBase->flag) == false) {
        modbusBase->flag = 0;
        return;
    }
    _sleep(2000);

    modbusBase->writeRegisters(meterAddressBaseAddress, meterAddress, modbusDevice);
    if (checkFlag(modbusBase->flag) == false) {
        modbusBase->flag = 0;
        return;
    }
    _sleep(2000);

    for (int j = 0; j < 5; j++) {
      ui->resultText->append(" ");

      QString msg = "Remaing time: " + QString::number(j) + " ...";
      ui->resultText->insertPlainText(msg);
      QTextCursor txc(ui->resultText->textCursor());
      txc.movePosition(QTextCursor::StartOfLine);
      txc.select(QTextCursor::LineUnderCursor);
      txc.removeSelectedText();
      _sleep(1000);
    }

    // m-bus check
    modbusBase->readRegisters(meterPollSN, EightEntries, modbusDevice, &(modbusBase->handleMeterPoll));
    _sleep(2000);
    setUILabelInfoEachTIme(ui->mbusTestLabel);
    if (ui->mbusTestLabel->text().contains("FAIL")) {
        setUILabelInfo(ui->mbusTestLabel);
        return;
    }

    modbusBase->readRegisters(meterPollManu, EightEntries, modbusDevice, &(modbusBase->handleMeterPoll));
    _sleep(2000);
    setUILabelInfoEachTIme(ui->mbusTestLabel);
    if (ui->mbusTestLabel->text().contains("FAIL")) {
        setUILabelInfo(ui->mbusTestLabel);
        return;
    }


    modbusBase->readRegisters(meterStatus, OneEntry, modbusDevice, &(modbusBase->handleMeterPollStatus));
    _sleep(2000);
    if (setUILabelInfo(ui->mbusCellularLabel))
        return;

    _sleep(1000);
    on_savePushButton_clicked();
}
