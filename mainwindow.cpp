#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modbustable.h"
#include "init.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QModbusRtuSerialSlave>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTime>
#include <QSize>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    modbusDevice(new QModbusRtuSerialMaster(this)),
    modbusGoldenDevice(new QModbusRtuSerialMaster(this)),
    lmRs485ModbuDevice(new QModbusRtuSerialMaster(this)),
    index(0),
    modbusBase(new ModbusBase()),
    lcModbusDevice(nullptr),
    ui(new Ui::MainWindow),
    flagSwitch(0),
    tableIndex(0)
    //init (new Init)
{
    ui->setupUi(this);
    serialAlarmInit();
    modbusBase->ui = this->ui;
    initModbusSlave();

    connect(this, &MainWindow::IOAdjustSignal, this, &MainWindow::IOAdjustSlot);
    ui->modelNameComBox->setCurrentIndex(3);

    // ui->tabWidget->setTabEnabled(0, false);
    // ui->tabWidget->setTabEnabled(1, false);
    // ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->setStyleSheet("QTabBar::tab:disabled {width: 0; color: transparent;}");

    setWindowTitle("v2.1.0");
}

MainWindow::~MainWindow()
{
    if (modbusDevice != nullptr) {
        modbusDevice->disconnect();
        delete modbusDevice;
    }

    if (modbusGoldenDevice != nullptr) {
        modbusGoldenDevice->disconnect();
        delete modbusGoldenDevice;
    }

    if (lmRs485ModbuDevice != nullptr) {
        lmRs485ModbuDevice->disconnect();
        delete lmRs485ModbuDevice;
    }

    if (lcModbusDevice != nullptr) {
        lcModbusDevice->disconnect();
        delete lcModbusDevice;
    }

    delete ui;
}

void MainWindow::initModbusSlave()
{
    lcModbusDevice = new QModbusRtuSerialSlave(this);

    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 10 });
    reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
    reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 10 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 10 });

    lcModbusDevice->setMap(reg);

    if(!lcModbusDevice->setData(QModbusDataUnit::HoldingRegisters, 0, 188))
        statusBar()->showMessage(tr("Could not set data: ") + lcModbusDevice->errorString(), 5000);
}

void MainWindow::on_connectPushButton_clicked()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
            modbusDevice->disconnectDevice();
            ui->connectPushButton->setText(tr("Connect"));
            ui->versionLineEdit->clear();
    } else {
            modelName = ui->modelNameComBox->currentText();
            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->serialComboBox->currentText());
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            modbusDevice->setTimeout(1000);
            modbusDevice->setNumberOfRetries(1);
            if (modbusDevice->connectDevice())
                ui->connectPushButton->setText(tr("Disconnect"));

            // modbusBase->readRegisters(SNAddr, SNEntries, modbusDevice, &(modbusBase->handleSNGoldenSample));
            // _sleep();

            if (modbusBase->GoldenSampleCheck) {
                modbusDevice->disconnectDevice();
                ui->connectPushButton->setText(tr("Connect"));
                QMessageBox::information(nullptr, "Connect Error", "Please check Golden Sample Again!!!");
                modbusBase->GoldenSampleCheck = false;
            }
    }

   modbusBase->readRegisters(VersionAddress, VersionEntries, modbusDevice, &(modbusBase->handleReadVersion));
   _sleep();
}

void MainWindow::on_connectGoldenPushButton_clicked()
{
    if (!modbusGoldenDevice)
        return;

    statusBar()->clearMessage();

    if (modbusGoldenDevice->state() == QModbusDevice::ConnectedState) {
            modbusGoldenDevice->disconnectDevice();
            ui->connectGoldenPushButton->setText(tr("Connect"));
    } else {
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->serialGoldenComboBox->currentText());
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            modbusGoldenDevice->setTimeout(1000);
            modbusGoldenDevice->setNumberOfRetries(1);
            if (modbusGoldenDevice->connectDevice()) {
                ui->connectGoldenPushButton->setText(tr("Disconnect"));
                ui->connectGoldenPushButton->setDisabled(true);
            }

            modbusBase->readRegisters(SNAddr, SNEntries, modbusGoldenDevice, &(modbusBase->handleSNGoldenSample));
            _sleep();
            if (!modbusBase->GoldenSampleCheck) {
                modbusGoldenDevice->disconnectDevice();
                ui->connectGoldenPushButton->setText(tr("Connect"));
                ui->connectGoldenPushButton->setEnabled(true);
                QMessageBox::information(nullptr, "Connect Error", "Please check Golden Sample Again!!!");
                modbusBase->GoldenSampleCheck = false;
            }
     }
}

void MainWindow::serialAlarmInit()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        serialInfoVector.push_back(info.portName());
        ui->serialComboBox->addItem(info.portName());
        ui->serialGoldenComboBox->addItem(info.portName());
        ui->LC485Combox->addItem(info.portName());
        ui->LM485ComBox->addItem(info.portName());
        ui->lmUsbCombox->addItem(info.portName());
        ui->LCUsbComBox->addItem(info.portName());
        ui->rstCombox->addItem(info.portName());

        // add for sl102
        ui->serialSl102ComboBox->addItem(info.portName());
    }

    QTimer *serialAlarm = new QTimer();
    serialAlarm->stop();
    serialAlarm->setInterval(1000);
    connect(serialAlarm, SIGNAL(timeout()), this, SLOT(serialAlarmTask()));
    serialAlarm->start();
}

void MainWindow::serialAlarmTask()
{
    QVector<QString> tmp;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        tmp.push_back(info.portName());
    }

    if (tmp != serialInfoVector) {
        serialInfoVector.clear();
        if(modbusDevice->state() == QModbusDevice::UnconnectedState) {
            ui->rstCombox->clear();
            ui->serialComboBox->clear();
            ui->LCUsbComBox->clear();

            // set for sl102
            ui->serialSl102ComboBox->clear();
        }

        if(modbusGoldenDevice->state() == QModbusDevice::UnconnectedState) {
            ui->serialGoldenComboBox->clear();
            ui->lmUsbCombox->clear();
        }

        if (lmRs485ModbuDevice->state() == QModbusDevice::UnconnectedState)
            ui->LM485ComBox->clear();

        if (lcModbusDevice->state() == QModbusDevice::UnconnectedState)
            ui->LC485Combox->clear();

        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
           serialInfoVector.push_back(info.portName());
           if(modbusDevice->state() == QModbusDevice::UnconnectedState) {
                ui->serialComboBox->addItem(info.portName());
                ui->LCUsbComBox->addItem(info.portName());
                ui->rstCombox->addItem(info.portName());
                ui->serialSl102ComboBox->addItem(info.portName());
           }

           if(modbusGoldenDevice->state() == QModbusDevice::UnconnectedState) {
                ui->serialGoldenComboBox->addItem(info.portName());
                ui->lmUsbCombox->addItem(info.portName());
           }

           if (lmRs485ModbuDevice->state() == QModbusDevice::UnconnectedState)
               ui->LM485ComBox->addItem(info.portName());

           if (lcModbusDevice->state() == QModbusDevice::UnconnectedState)
                ui->LC485Combox->addItem(info.portName());
        }
    }
}

void MainWindow::setBitMapValue(QVector<quint16> &val)
{
    for (int i = 0; i < 16; i++) {
        if (i == 0){
            val.push_back(0x82);
        } else {
            val.push_back(0);
        }
    }
}

void MainWindow::_sleep(unsigned int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool MainWindow::checkFlag(int flag)
{
    if (flag) {
        QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("!!!STOP!!!");
        ui->resultText->append(msg);
        return false;
    } else {
        return true;
    }
}

void MainWindow::on_LMUSBCom_clicked()
{
    if (!modbusGoldenDevice)
        return;

    statusBar()->clearMessage();

    if (modbusGoldenDevice->state() == QModbusDevice::ConnectedState) {
            modbusGoldenDevice->disconnectDevice();
            ui->LMUSBCom->setText(tr("Connect set"));
     } else {
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lmUsbCombox->currentText());
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            modbusGoldenDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            modbusGoldenDevice->setTimeout(5000);
            modbusGoldenDevice->setNumberOfRetries(1);
            if (modbusGoldenDevice->connectDevice()) {
                QVector<quint16> val;
                for (int i = 0; i < 16; i++) {
                    if (i == 0){
                        val.push_back(0x82);
                    } else {
                        val.push_back(0);
                    }
                }
                modbusBase->writeRegisters(netBitMapAddr, netBitMapEntires, val, modbusGoldenDevice);
                ui->LMUSBCom->setText(tr("Disconnect"));
            }
     }
}

void MainWindow::on_LCUSBCom_clicked()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
            modbusDevice->disconnectDevice();
            ui->LCUSBCom->setText(tr("Connect set"));
     } else {
            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->LCUsbComBox->currentText());
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            modbusDevice->setTimeout(5000);
            modbusDevice->setNumberOfRetries(1);
            if (modbusDevice->connectDevice()) {
                QVector<quint16> val;
                for (int i = 0; i < 16; i++) {
                    if (i == 0){
                        val.push_back(0x80);
                    } else {
                        val.push_back(0);
                    }
                }
                modbusBase->writeRegisters(netBitMapAddr, netBitMapEntires, val, modbusDevice);
                ui->LCUSBCom->setText(tr("Disconnect"));
            }
     }
}

void MainWindow::on_LMRS485Com_clicked()
{
    if (!lmRs485ModbuDevice)
        return;

    statusBar()->clearMessage();

    if (lmRs485ModbuDevice->state() == QModbusDevice::ConnectedState) {
            lmRs485ModbuDevice->disconnectDevice();
            ui->LMRS485Com->setText(tr("Connect"));
     } else {
            lmRs485ModbuDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->LM485ComBox->currentText());
            lmRs485ModbuDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            lmRs485ModbuDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            lmRs485ModbuDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            lmRs485ModbuDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            lmRs485ModbuDevice->setTimeout(1000);
            lmRs485ModbuDevice->setNumberOfRetries(1);
            if (lmRs485ModbuDevice->connectDevice())
                ui->LMRS485Com->setText(tr("Disconnect"));
     }
}

void MainWindow::on_LCRS485Com_clicked()
{
    if (!lcModbusDevice)
        return;

    statusBar()->clearMessage();

    if (lcModbusDevice->state() == QModbusDevice::ConnectedState) {
            lcModbusDevice->disconnectDevice();
            ui->LCRS485Com->setText(tr("Connect"));
     } else {
            lcModbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->LC485Combox->currentText());
            lcModbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud9600);
            lcModbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
            lcModbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
            lcModbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
            lcModbusDevice->setServerAddress(8);
            if (lcModbusDevice->connectDevice())
                ui->LCRS485Com->setText(tr("Disconnect"));
     }
}

void MainWindow::loraRFFinish()
{
    if (!ui->rfLMUsbCheckBox->isChecked()) {
        if (modbusGoldenDevice->state() == QModbusDevice::ConnectedState)
            modbusBase->writeRegisters(ResetEnableAddress, 1, modbusGoldenDevice);
        _sleep(2000);
        on_LMUSBCom_clicked();
    }

    if (!ui->rfLCUsbCheckBox->isChecked()) {
        if (modbusDevice->state() == QModbusDevice::ConnectedState)
           modbusBase->writeRegisters(ResetEnableAddress, 1, modbusDevice);
        _sleep(2000);
        on_LCUSBCom_clicked();
    }

    if (!ui->rfLMRSCheckBox->isChecked())
        on_LMRS485Com_clicked();

    if (!ui->rfLCRScheckBox->isChecked())
        on_LCRS485Com_clicked();
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    Q_UNUSED(index);

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
        modbusDevice->disconnectDevice();
        ui->connectPushButton->setText("Connect");
        ui->LMUSBCom->setText("Connect Set");
        ui->connectSl102PushButton->setText("Connect");
        ui->resetPushButton->setText("Reset");
    }

    if (modbusGoldenDevice->state() == QModbusDevice::ConnectedState) {
        modbusGoldenDevice->disconnectDevice();
        ui->connectGoldenPushButton->setText("Connect");
        ui->LCUSBCom->setText("Connect Set");
        ui->connectGoldenPushButton->setDisabled(false);
    }

    if (lmRs485ModbuDevice->state() == QModbusDevice::ConnectedState) {
        lmRs485ModbuDevice->disconnectDevice();
        ui->LMRS485Com->setText("Connect");
    }

    if (lcModbusDevice->state() == QModbusDevice::ConnectedState) {
        lcModbusDevice->disconnectDevice();
        ui->LCRS485Com->setText("Connect");
    }

    ui->voLabel->clear();
    ui->vi1Label->clear();
    ui->vi2Label->clear();
    ui->ioLabel->clear();
    ui->ii1Label->clear();
    ui->ii2Label->clear();
    ui->PWM1Label->clear();
    ui->PWM2Label->clear();
}

void MainWindow::on_clearPushButton_clicked()
{
    ui->resultText->clear();
}

void MainWindow::on_savePushButton_clicked()
{
    QString saveContent = ui->resultText->toPlainText();

    QDateTime time = QDateTime::currentDateTime();
    QString StrCurrentTime = time.toString("_yyyyMMddhhmmss");
    QString fileName = ui->SNLineEdit->text() + StrCurrentTime + ".txt";

    QFile file(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.write(saveContent.toUtf8());
    file.close();
    ui->resultText->append("save file " + fileName + " successful");
}

void MainWindow::on_stopPushButton_clicked()
{
    modbusBase->flag = RaiseFlag;
    canNotFixFlag = RaiseFlag;
    QString msg = QString("<font color=\"#FF0000\"> %1 </font>\n").arg("Please wait 20s before stop.");
    ui->resultText->append(msg);
}

void MainWindow::on_finishedCheckBox_clicked()
{
    if(ui->finishedCheckBox->isChecked())
    {
        ui->modelNameCheckBox->setChecked(false);
        ui->SNCheckBox->setChecked(true);
        ui->modelNameComBox->setCurrentText("");
        /*
            ui->voutCheckBox->setChecked(false);
            ui->vIn1CheckBox->setChecked(false);
            ui->vIn2CheckBox->setChecked(false);
            ui->AOCheckBox->setChecked(false);
            ui->AI1CheckBox->setChecked(false);
            ui->AI2CheckBox->setChecked(false);
            ui->PWMCheckBox1->setChecked(false);
            ui->PWMCheckBox2->setChecked(false);
        */
    } else {
        ui->modelNameCheckBox->setChecked(true);
        ui->SNCheckBox->setChecked(false);
        /*
            ui->voutCheckBox->setChecked(true);
            ui->vIn1CheckBox->setChecked(true);
            ui->vIn2CheckBox->setChecked(true);
            ui->AOCheckBox->setChecked(true);
            ui->AI1CheckBox->setChecked(true);
            ui->AI2CheckBox->setChecked(true);
            ui->PWMCheckBox1->setChecked(true);
            ui->PWMCheckBox2->setChecked(true);
        */
    }
}

void MainWindow::on_resetSL102PushButton_clicked()
{
    if (!modbusDevice)
        return;

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
        modbusBase->writeRegisters(ResetEnableAddress, 1, modbusDevice);
        _sleep(2000);
        ui->resultText->append(tr("Reset Dua com"));
    }
}
