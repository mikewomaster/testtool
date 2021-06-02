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

void MainWindow::on_resetPushButton_clicked()
{
    // if (!modbusDevice || !modbusGoldenDevice)
        // return;

    // on_savePushButton_clicked();

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
        modbusBase->writeRegisters(ResetEnableAddress, 1, modbusDevice);
        _sleep(2000);
        ui->resultText->append(tr("Reset Dua com"));
        // modbusDevice->disconnectDevice();
        // ui->connectPushButton->setText("Connect");
    }
/*
    if (modbusGoldenDevice->state() == QModbusDevice::ConnectedState){
        if (ui->goldenKeepCheckBox->isChecked())
            return;

        modbusBase->writeRegisters(ResetEnableAddress, 1, modbusGoldenDevice);
        _sleep();
        ui->resultText->append(tr("Reset GoldenSample com"));
        modbusGoldenDevice->disconnectDevice();
        ui->connectGoldenPushButton->setText("Connect");
        ui->connectGoldenPushButton->setEnabled(true);
    }
 */

}

void MainWindow::checkIO5(QString str, int value, int gap, QModbusClient *devWrite, int writeAddr, QModbusClient *devRead, int readAddr)
{
    if (!modbusBase->flag) {

        ui->resultText->append(str);
        modbusBase->writeRegisters(writeAddr, value, devWrite);
        _sleep(2000);
        if (checkFlag(modbusBase->flag) == false) {
            return;
        }

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        int temp1 = modbusBase->IOValue;

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        int temp2 = modbusBase->IOValue;

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        int temp3 = modbusBase->IOValue;

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        int temp4 = modbusBase->IOValue;

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        int temp5 = modbusBase->IOValue;

        int temp = (temp1 + temp2 + temp3 + temp4 + temp5) / 5;
        modbusBase->IOValue = temp;

        if ((modbusBase->IOValue > (value-gap)) && (modbusBase->IOValue) < (value+gap)) {
            modbusBase->flag = 0;
            ui->resultText->append("successful");
            return;
        } else {
            modbusBase->flag = 1;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("fail");
            ui->resultText->append(msg);
        }
        // modbusBase->IOValue = 0;
    }
}

void MainWindow::checkIO2(QString str, int value, int gap, QModbusClient* devWrite, int writeAddr, QModbusClient* devRead, int readAddr)
{
    if (!modbusBase->flag) {
        ui->resultText->append(str);
        modbusBase->writeRegisters(writeAddr, value, devWrite);
        _sleep();
        if (checkFlag(modbusBase->flag) == false) {
            return;
        }

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        if ((modbusBase->IOValue > (value-gap)) && (modbusBase->IOValue) < (value+gap)) {
            modbusBase->flag = 0;
        } else {
            modbusBase->flag = 1;
        }
    }
}

void MainWindow::checkIO(QString str, int value, int gap, QModbusClient* devWrite, int writeAddr, QModbusClient* devRead, int readAddr)
{
    if (!modbusBase->flag) {
        ui->resultText->append(str);
        modbusBase->writeRegisters(writeAddr, value, devWrite);
        _sleep();
        if (checkFlag(modbusBase->flag) == false) {
            return;
        }

        modbusBase->readRegisters(readAddr, IOEntries, devRead, &(modbusBase->handleReadIOValue));
        _sleep();
        if ((modbusBase->IOValue > (value-gap)) && (modbusBase->IOValue) < (value+gap)) {
            modbusBase->flag = 0;
            ui->resultText->append("successful");
            return;
        } else {
            modbusBase->flag = 1;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("fail");
            ui->resultText->append(msg);
        }
        // modbusBase->IOValue = 0;
    }
}

void MainWindow::IOAdjustSlot(adjustIOStruct& adjustStruct)
{
    modbusBase->readRegisters(adjustStruct.readAdjustAddr, UnitNumber, modbusDevice, &(modbusBase->handleReadAdjustValue));
    _sleep();
    int value;

    int times = (abs(adjustStruct.readValue - adjustStruct.setValue) / (adjustStruct.gap*10)) + 1;

    if (adjustStruct.readValue > (adjustStruct.setValue + adjustStruct.gap))
        value = modbusBase->AdjustValue - (adjustStruct.direction) * Step * times;
    else
        value = modbusBase->AdjustValue + (adjustStruct.direction) * Step * times;

    if ((value > MaxRefVal) || (value < MinRefVal)) {
        canNotFixFlag = RaiseFlag;
        return;
    } else {
        modbusBase->writeRegisters(adjustStruct.readAdjustAddr, value, modbusDevice);
        _sleep();
    }
}

void MainWindow::FirstTest()
{
    if (ui->voutCheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO5("Volatage output 5v test: ", 5000, 10, modbusDevice, voltageOutputAddr+voltageOutputChannel, modbusGoldenDevice, voltageInputAddr+voltageInputChannelOne);

        while (modbusBase->flag) {
            adjustIOStruct unitStruct ={voutRef, 10, 5000, modbusBase->IOValue, -1};
            emit(IOAdjustSignal(unitStruct));

            if (canNotFixFlag == RaiseFlag) {
                canNotFixFlag = FallFlag;
                break;
            }
            checkIO5("", 5000, 10, modbusDevice, voltageOutputAddr+voltageOutputChannel, modbusGoldenDevice, voltageInputAddr+voltageInputChannelOne);
        }

        if (checkFlag(modbusBase->flag) == false) {
            canNotFixFlag = FallFlag;
            modbusBase->flag = 0;

            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->voLabel->setText(msg);

            return;
        }

        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->voLabel->setText(msg);
    }

    if (ui->vIn1CheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO5("Voltage input one 10v test:  ", 10000, 10, modbusGoldenDevice, voltageOutputAddr+voltageOutputChannel, modbusDevice, voltageInputAddr+voltageInputChannelOne);

        while (modbusBase->flag) {
            adjustIOStruct unitStruct ={vinRef, 10, 10000, modbusBase->IOValue, 1};
            emit(IOAdjustSignal(unitStruct));

            if (canNotFixFlag == RaiseFlag) {
                canNotFixFlag = FallFlag;
                break;
            }
            checkIO5("Voltage input one 10v test: ", 10000, 10, modbusGoldenDevice, voltageOutputAddr+voltageOutputChannel, modbusDevice, voltageInputAddr+voltageInputChannelOne);
        }

        if (checkFlag(modbusBase->flag) == false) {
            canNotFixFlag = FallFlag;
            modbusBase->flag = 0;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->vi1Label->setText(msg);
            return;
        }
        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->vi1Label->setText(msg);
    }

    if (ui->vIn2CheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO5("Voltage input two 10v test:  ", 10000, 10, modbusGoldenDevice, voltageOutputAddr+voltageOutputChannel, modbusDevice, voltageInputAddr+voltageInputChannelTwo);

        while (modbusBase->flag) {
            adjustIOStruct unitStruct ={vin2Ref, 10, 10000, modbusBase->IOValue, 1};
            emit(IOAdjustSignal(unitStruct));

            if (canNotFixFlag == RaiseFlag) {
                canNotFixFlag = FallFlag;
                break;
            }
            checkIO5("Voltage input two 10v test: ", 10000, 10, modbusGoldenDevice, voltageOutputAddr+voltageOutputChannel, modbusDevice, voltageInputAddr+voltageInputChannelTwo);
        }

        if (checkFlag(modbusBase->flag) == false) {
            canNotFixFlag = FallFlag;
            modbusBase->flag = 0;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->vi2Label->setText(msg);
            return;
        }
        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->vi2Label->setText(msg);
    }

    if(ui->AOCheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO5("Current Output 15mA test: ", 15000, 15, modbusDevice, currentOutputAddr+currentOutputChannel, modbusGoldenDevice, currentInputAddr+currentInputChannelOne);
        while (modbusBase->flag) {
            adjustIOStruct unitStruct ={ioutRef, 15, 15000, modbusBase->IOValue, -1};
            emit(IOAdjustSignal(unitStruct));

            if (canNotFixFlag == RaiseFlag) {
                canNotFixFlag = FallFlag;
                break;
            }
            checkIO5("Current Output 15mA test: ", 15000, 15, modbusDevice, currentOutputAddr+currentOutputChannel, modbusGoldenDevice, currentInputAddr+currentInputChannelOne);
        }

        if (checkFlag(modbusBase->flag) == false) {
            canNotFixFlag = FallFlag;
            modbusBase->flag = 0;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->ioLabel->setText(msg);
            return;
        }
        QString msg = QString ("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->ioLabel->setText(msg);
    }

    if (ui->AI1CheckBox->isChecked()) {
        if(QMessageBox::Yes == QMessageBox::question(this, "Current Switch", "Please Switch Current 1"))
            flagSwitch = RaiseFlag;
        else
            flagSwitch = FallFlag;

        if(flagSwitch) {
            ui->resultText->append("************************");
            checkIO5("Current Input one 15mA test: ", 15000, 15, modbusGoldenDevice, currentOutputAddr+currentOutputChannel, modbusDevice, currentInputAddr+currentInputChannelOne);
            while (modbusBase->flag) {
                adjustIOStruct unitStruct ={iinRef, 15, 15000, modbusBase->IOValue, 1};
                emit(IOAdjustSignal(unitStruct));

                if (canNotFixFlag == RaiseFlag) {
                    canNotFixFlag = FallFlag;
                    break;
                }
                checkIO5("Current Input one 15mA test: ", 15000, 15, modbusGoldenDevice, currentOutputAddr+currentOutputChannel, modbusDevice, currentInputAddr+currentInputChannelOne);
            }

            if (checkFlag(modbusBase->flag) == false) {
                canNotFixFlag = FallFlag;
                modbusBase->flag = 0;
                QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
                ui->ii1Label->setText(msg);
                return;
            }
            QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
            ui->ii1Label->setText(msg);
            flagSwitch = FallFlag;
        }
    }

    if (ui->AI2CheckBox->isChecked()) {
        if(QMessageBox::Yes == QMessageBox::question(this, "Current Switch", "Please Switch Current 2"))
            flagSwitch = 1;
        else
            flagSwitch = 0;

        if(flagSwitch) {
            ui->resultText->append("************************");
            checkIO5("Current Input two 15mA test: ", 15000, 15, modbusGoldenDevice, currentOutputAddr+currentOutputChannel, modbusDevice, currentInputAddr+currentInputChannelTwo);
            while (modbusBase->flag) {
                adjustIOStruct unitStruct ={iin2Ref, 15, 15000, modbusBase->IOValue, 1};
                emit(IOAdjustSignal(unitStruct));

                if (canNotFixFlag == RaiseFlag) {
                    canNotFixFlag = FallFlag;
                    break;
                }
                checkIO5("Current Input two 15mA test: ", 15000, 15, modbusGoldenDevice, currentOutputAddr+currentOutputChannel, modbusDevice, currentInputAddr+currentInputChannelTwo);
            }

            if (checkFlag(modbusBase->flag) == false) {
                canNotFixFlag = FallFlag;
                modbusBase->flag = 0;
                QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
                ui->ii2Label->setText(msg);
                return;
            }
            QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
            ui->ii2Label->setText(msg);
            flagSwitch = 0;
        }
    }
}

void MainWindow::SecondTest()
{
    if (ui->voutCheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO2("Volatage output 5v test: ", 5000, 1000, modbusDevice, voltageOutputAddr+voltageOutputChannel, modbusGoldenDevice, voltageInputAddr+voltageInputChannelOne);
        if (modbusBase->flag == RaiseFlag) {
            modbusBase->flag = FallFlag;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->voLabel->setText(msg);
            return;
        }

        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->voLabel->setText(msg);
    }

    if (ui->vIn1CheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO2("Voltage input one 10v test:  ", 10000, 1000, modbusGoldenDevice, voltageOutputAddr+voltageOutputChannel, modbusDevice, voltageInputAddr+voltageInputChannelOne);

        if (modbusBase->flag == RaiseFlag) {
            modbusBase->flag = FallFlag;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->vi1Label->setText(msg);
            return;
        }

        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->vi1Label->setText(msg);
    }

    if (ui->vIn2CheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO2("Voltage input two 10v test:  ", 10000, 1000, modbusGoldenDevice, voltageOutputAddr+voltageOutputChannel, modbusDevice, voltageInputAddr+voltageInputChannelTwo);

        if (modbusBase->flag == RaiseFlag) {
            modbusBase->flag = FallFlag;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->vi2Label->setText(msg);
            return;
        }

        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->vi2Label->setText(msg);
    }

    if(ui->AOCheckBox->isChecked()) {
        ui->resultText->append("************************");
        checkIO2("Current Output 15mA test: ", 15000, 1500, modbusDevice, currentOutputAddr+currentOutputChannel, modbusGoldenDevice, currentInputAddr+currentInputChannelOne);

        if (modbusBase->flag == RaiseFlag) {
            modbusBase->flag = FallFlag;
            QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
            ui->ioLabel->setText(msg);
            return;
        }

        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
        ui->ioLabel->setText(msg);
    }

    if (ui->AI1CheckBox->isChecked()) {
        if(QMessageBox::Yes == QMessageBox::question(this, "Current Switch", "Please Switch Current 1"))
            flagSwitch = RaiseFlag;
        else
            flagSwitch = FallFlag;

        if(flagSwitch) {
            ui->resultText->append("************************");
            checkIO2("Current Input one 15mA test: ", 15000, 1500, modbusGoldenDevice, currentOutputAddr+currentOutputChannel, modbusDevice, currentInputAddr+currentInputChannelOne);
            if (modbusBase->flag == RaiseFlag) {
                modbusBase->flag = FallFlag;
                QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
                ui->ii1Label->setText(msg);
                return;
            }

            QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
            ui->ii1Label->setText(msg);
            flagSwitch = FallFlag;
        }
    }

    if (ui->AI2CheckBox->isChecked()) {
        if(QMessageBox::Yes == QMessageBox::question(this, "Current Switch", "Please Switch Current 2"))
            flagSwitch = 1;
        else
            flagSwitch = 0;

        if(flagSwitch) {
            ui->resultText->append("************************");
            checkIO2("Current Input two 15mA test: ", 15000, 1500, modbusGoldenDevice, currentOutputAddr+currentOutputChannel, modbusDevice, currentInputAddr+currentInputChannelTwo);
            if (modbusBase->flag == RaiseFlag) {
                modbusBase->flag = FallFlag;
                QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
                ui->ii2Label->setText(msg);
                return;
            }

            QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
            ui->ii2Label->setText(msg);
            flagSwitch = FallFlag;
            flagSwitch = 0;
        }
    }
}

void MainWindow::on_functionStartPushButton_clicked()
{
    ui->resultText->clear();

    ui->voLabel->clear();
    ui->vi1Label->clear();
    ui->vi2Label->clear();
    ui->ioLabel->clear();
    ui->ii1Label->clear();
    ui->ii2Label->clear();
    ui->PWM1Label->clear();
    ui->PWM2Label->clear();

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
     *
     */
    if (ui->modelNameCheckBox->isChecked()){
         if (ui->modelNameComBox->currentText() == "") {
             QMessageBox::question(this, "Model Name", "Please Check ModelName which has no value!!!");
             return;
         }

        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
         if (1) {
            QString str = ui->modelNameComBox->currentText();
            ui->resultText->append("Model Name: " + str);
            modbusBase->cv.modelName = str;
            modbusBase->writeRegisters(ModelNameAddr, ModelNameEntires, str, modbusDevice);
            _sleep(2000);
            if (checkFlag(modbusBase->flag) == false) {
                modbusBase->flag = FallFlag;
                ui->modelLabel->setText(msg);
                return;
            } else {
                modbusBase->readRegisters(ModelNameAddr, ModelNameEntires, modbusDevice, &(modbusBase->handleReadModelName));
                _sleep(2000);
                if (checkFlag(modbusBase->flag) == false) {
                    modbusBase->flag = FallFlag;
                    ui->modelLabel->setText(msg);
                    return;
                }
                msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
                ui->modelLabel->setText(msg);
            }
         } else {
             ui->modelLabel->setText(msg);
         }
     }

    /*
     *
     *  3. SN
     *  arrObj = init->Array[index].toObject();
     *  str = arrObj["SN"].toString();
     *
     */
    if (ui->SNCheckBox->isChecked()) {
        if (ui->SNLineEdit->text() == "") {
            QMessageBox::question(this, "SN", "Please Check SN which has no value!!!");
            return;
        }

        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
        if (1) {
            QString str = ui->SNLineEdit->text();
            ui->resultText->append("SN: " + str);
            modbusBase->cv.SN = str;
            modbusBase->writeRegisters(SNAddr, SNEntries, str, modbusDevice);
            _sleep(2000);
            if (checkFlag(modbusBase->flag) == false) {
                ui->SNLabel->setText(msg);
                modbusBase->flag = 0;
                return;
            } else {
                modbusBase->readRegisters(SNAddr, SNEntries, modbusDevice, &(modbusBase->handleReadSN));
                _sleep(2000);
                if (checkFlag(modbusBase->flag) == false) {
                    ui->SNLabel->setText(msg);
                    modbusBase->flag = FallFlag;
                    return;
                }
                msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("PASS");
                ui->SNLabel->setText(msg);
            }
        } else {
            ui->SNLabel->setText(msg);
        }
        ui->SNLineEdit->clear();
    }

    // IO
    if (ui->finishedCheckBox->isChecked())
        SecondTest();
    else
        FirstTest();

    if (ui->PWMCheckBox1->isChecked()){
        if(QMessageBox::Yes == QMessageBox::question(this, "Current Switch", "Please Check and Switch PWM 1"))
            flagSwitch = 1;
        else
            flagSwitch = 0;

        if (flagSwitch) {
            if (!modbusBase->flag) {
               ui->resultText->append("************************");
               ui->resultText->append("PWM 1:");
               QVector<quint16> pwmVal;
               pwmVal.push_back(1000);
               pwmVal.push_back(50);

               modbusBase->writeRegisters(PWMOutputAddr+(PWMOne*PWMIOEntries), PWMIOEntries, pwmVal, modbusDevice);
               _sleep();
               if (checkFlag(modbusBase->flag) == false) {
                   modbusBase->flag = 0;
                   QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
                   ui->PWM1Label->setText(msg);
                   return;
               }
               QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("Set OK");
               ui->PWM1Label->setText(msg);
           }
           flagSwitch = FallFlag;
       }
    }

    if (ui->PWMCheckBox2->isChecked()){
        if(QMessageBox::Yes == QMessageBox::question(this, "Current Switch", "Please Check and Switch PWM 2"))
            flagSwitch = 1;
        else
            flagSwitch = 0;

        if (flagSwitch) {
            if (!modbusBase->flag) {
               ui->resultText->append("************************");
               ui->resultText->append("PWM 2:");
               QVector<quint16> pwmVal;
               pwmVal.push_back(2000);
               pwmVal.push_back(50);

               modbusBase->writeRegisters(PWMOutputAddr+(PWMTwo*PWMIOEntries), PWMIOEntries, pwmVal, modbusDevice);
               _sleep();
               if (checkFlag(modbusBase->flag) == false) {
                   modbusBase->flag = 0;
                   QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
                   ui->PWM2Label->setText(msg);
                   return;
               }
               QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("Set OK");
               ui->PWM2Label->setText(msg);
            }
          flagSwitch = 0;
      }
    }

    ui->resultText->append("************************");
    QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("Basic Test Done");
    on_resetPushButton_clicked();
    ui->resultText->append(msg);
}

void MainWindow::on_LoRaStart_clicked()
{
    // read --serial<modubs master> -- lm  ---lora rf --- lc -- serial <modubs slave> -- <increase max timeout>
    ui->resultText->append("************************");
    ui->resultText->append("Test LoRa RF and Serial:");
    ui->LoRalabel->clear();

    for (int i = 0; i < 3; i++) {
        modbusBase->readRegisters(0, 1, 8, lmRs485ModbuDevice, &(modbusBase->handleReadIOValue));
        _sleep(2000);
    }

    if (modbusBase->IOValue == 188) {
        QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("SUCCESS");
        ui->resultText->append(msg);
        ui->LoRalabel->setText(msg);
    } else {
        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("FAIL");
        ui->resultText->append(msg);
        ui->LoRalabel->setText(msg);
    }

    ui->resultText->append("Test Done");

    // set modbus lm id with only id 2
    ui->resultText->append("************************");
    QString msg = QString("<font color=\"#2E8B57\"> %1 </font>\n").arg("LoRa-Serial Test Done");
    ui->resultText->append(msg);
    // on_resetPushButton_clicked();
    loraRFFinish();
    on_savePushButton_clicked();

    modbusBase->IOValue = 0;
}
