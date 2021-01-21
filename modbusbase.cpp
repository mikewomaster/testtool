#include "modbusbase.h"
#include "modbustable.h"
#include "mainwindow.h"

ModbusBase::ModbusBase():flag(0),
    GoldenSampleCheck(false)
{
}

void ModbusBase::writeRegisters(int addr, int num, QVector<quint16> newValues, QModbusClient * modbusDevice)
{
    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, num);

    writeUnit.setValues(newValues);

    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    getMainWindow()->statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                    flag = 1;
                    ui->resultText->append("Set Fail\n");
                } else if (reply->error() != QModbusDevice::NoError) {
                    getMainWindow()->statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                    flag = 1;
                    ui->resultText->append("Set Fail\n");
                } else {
                    ui->resultText->append("Set Successful\n");
                    flag = 0;
                    getMainWindow()->statusBar()->showMessage(tr("OK!"));
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        getMainWindow()->statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void ModbusBase::writeRegisters(int addr, int num, QString str, QModbusClient *modbusDevice)
{
    QVector<quint16> values;

    int i = 0;
    for (i = 0; i < str.size(); i++) {
        if ((i+1) % 2 == 0) {
            quint16 temp = str.at(i - 1).toLatin1();
            temp = (temp << 8) + str.at(i).toLatin1();
            values.push_back(temp);
        }
    }

    if (i % 2) {
        quint16 temp = str.at(i-1).toLatin1();
        temp = temp << 8;
        values.push_back(temp);
        i++;
    }

    if (addr != mqttTopicAddress && addr != MQTTSubTopicAddress) {
        for (i = (i / 2); i < num; i++) {
            values.push_back(0x0000);
        }
    }

    if (modbusDevice->state() == QModbusDevice::UnconnectedState ) {
        flag = 1;
        return;
    }
    // statusBar()->clearMessage();

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, num);

    writeUnit.setValues(values);
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    getMainWindow()->statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                        ui->resultText->append("Set Fail\n");
                        flag = 1;
                } else if (reply->error() != QModbusDevice::NoError) {
                    getMainWindow()->statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                        ui->resultText->append("Set Fail\n");
                        flag = 1;
                } else {
                    getMainWindow()->statusBar()->showMessage(tr("OK!"));
                    ui->resultText->append("Set Successful\n");
                    flag = 0;
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        getMainWindow()->statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void ModbusBase::handleReadAdjustValue()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        AdjustValue = unit.value(0);
        ui->resultText->append("Read: " + QString::number(AdjustValue));
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

void ModbusBase::handleReadAmper()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        AdjustValue = unit.value(0);
        ui->resultText->append("Read: " + QString::number(AdjustValue/100.0) + "A");
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

void ModbusBase::handleReadVoltage()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        AdjustValue = unit.value(0);
        ui->resultText->append("Read: " + QString::number(AdjustValue/100.0) + "V");
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

void ModbusBase::PowerReadReady(int times)
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    Init init;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        ui->resultText->append("Read Power: " + QString::number(unit.value(0)/100.0) + " W");

        int powerUnit = unit.value(0) / 100.0;
        if (times == 1 && \
            powerUnit >= init.configArray[0].toObject()["powerMin100"].toInt() && \
            powerUnit <= init.configArray[0].toObject()["powerMax100"].toInt())
            ui->resultText->append("Read Full Power Success.");
        else if (times == 2 && \
                 powerUnit >= init.configArray[0].toObject()["powerMin50"].toInt() && \
                 powerUnit <= init.configArray[0].toObject()["powerMax50"].toInt())
            ui->resultText->append("Read Half Power Success.");
        else if (times == 3 && \
                 powerUnit >= init.configArray[0].toObject()["powerMin0"].toInt() && \
                 powerUnit <= init.configArray[0].toObject()["powerMax0"].toInt())
            ui->resultText->append("Read Empty Power Success.");
        else {
            ui->resultText->append("Read Power Fail, Please Check CH Hardware.");
            flag = 1;
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

void ModbusBase::handleReadPowerFullValue()
{
    PowerReadReady(1);
}

void ModbusBase::handleReadPowerHalfValue()
{
    PowerReadReady(2);
}

void ModbusBase::handleReadPowerEmptyValue()
{
    PowerReadReady(3);
}

void ModbusBase::handleNBSIM()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        int simStatus = unit.value(0);

        if (simStatus == 0) {
            ui->resultText->append("SIM NOT INSERTED");
            flag = 1;
        }
        else if (simStatus == 1) {
            ui->resultText->append("SIM READY");
            flag = 0;
        }
        else if (simStatus == 2) {
            ui->resultText->append("SIM NOT READY");
            flag = 1;
        }
        else {
            flag = 1;
        }

        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        flag = 1;
        ui->resultText->append("Read Fail. ");
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        flag = 1;
        ui->resultText->append("Read Fail");
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void ModbusBase::handleNBSTATUS()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        int nbStatus = unit.value(0);

        if (nbStatus == 0) {
            ui->resultText->append("SIM ERR");
            flag = 1;
        }
        else if (nbStatus == 1) {
            ui->resultText->append("CELLUAR CONNECTED");
            flag = 0;
        }
        else if (nbStatus == 2) {
            ui->resultText->append("CELLULAR SEARCHING");
            flag = 1;
        }
        else if (nbStatus = 3) {
            ui->resultText->append("CELLULAR DENIED");
            flag = 1;
        }
        else {
            flag = 1;
        }

        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        flag = 1;
        ui->resultText->append("Read Fail. ");
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        flag = 1;
        ui->resultText->append("Read Fail");
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void ModbusBase::handleNBRSSI()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    Init init;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        short nbRSSI = unit.value(0);

        if (nbRSSI >= init.cellularArray[0].toObject()["rssiMin"].toInt()) {
            QString msg = "NB Read RSSI Value " + QString::number(nbRSSI) +"dBm";
            ui->resultText->append(msg);
            flag = 0;
            getMainWindow()->statusBar()->showMessage(tr("OK!"));
        } else {
            flag = 1;
            QString messageFail = reply->errorString();
            ui->resultText->append(messageFail);
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        flag = 1;
        ui->resultText->append("Read Fail");
        QString messageFail = reply->errorString();
        ui->resultText->append(messageFail);
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        flag = 1;
        ui->resultText->append("Read Fail");
        QString messageFail = reply->errorString();
        ui->resultText->append(messageFail);
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void ModbusBase::handleReadIOValue()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        IOValue = unit.value(0);
        ui->resultText->append("Read: " + QString::number(IOValue));
        getMainWindow()->statusBar()->showMessage(tr("OK!"));
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        IOValue = 0;
        ui->resultText->append("Read: " + QString::number(IOValue));
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        IOValue = 0;
        ui->resultText->append("Read: " + QString::number(IOValue));
        getMainWindow()->statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}

void ModbusBase::writeRegisters(int addr, int val, QModbusClient *modbusDevice)
{
    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, 1);

    writeUnit.setValue(0, val);
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {

                   getMainWindow()->statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                   ui->resultText->append("Set Fail 1");
                   flag = 1;
                } else if (reply->error() != QModbusDevice::NoError) {
                   ui->resultText->append("Set Fail 2");
                   getMainWindow()->statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                   flag = 1;
                } else {
                    ui->resultText->append("Set Success !");
                    flag = 0;
                    getMainWindow()->statusBar()->showMessage(tr("OK!"));
                }

                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        getMainWindow()->statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void ModbusBase::processReadUnit(QModbusDataUnit unit, QString str)
{
        if ((unit.value(0) >> 8 == 0xff))
                return;

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

        s == str ? flag = FallFlag : flag = RaiseFlag;
        ui->resultText->append("Read: " + s);
}

void ModbusBase::handleReadSN()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        processReadUnit(unit, cv.SN);
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

void ModbusBase::handleReadModelName()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        processReadUnit(unit, cv.modelName);
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

void ModbusBase::handleReadVersion()
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
        ui->versionLineEdit->setText(s);
        ui->versionSl102LineEdit->setText(s);
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


void ModbusBase::handleSNGoldenSample()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

        if ((unit.value(0) >> 8 == 0xff)) {
            GoldenSampleCheck = false;
            return;
        }

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

        qDebug() << s;

        if (s.contains("Golden"))
            GoldenSampleCheck = true;
        else
            GoldenSampleCheck = false;

    } else if (reply->error() == QModbusDevice::ProtocolError) {

    } else {

    }
    reply->deleteLater();
}

void ModbusBase::readRegisters(int addr, int num, int id, QModbusClient *modbusDevice, void (ModbusBase::*fn)())
{
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, num);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, id)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, fn);
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
           getMainWindow()->statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void ModbusBase::readRegisters(int addr, int num, QModbusClient *modbusDevice, void (ModbusBase::*fn)())
{
    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, addr, num);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, fn);
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
           getMainWindow()->statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

QMainWindow* ModbusBase::getMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return mainWin;
    return nullptr;
}


