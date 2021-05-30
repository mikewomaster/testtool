#include "mainwindow.h"
#include "init.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QDateTime time = QDateTime::currentDateTime();
    int timeT = time.toTime_t();
    if (timeT <= 1624198378)
    {
        QString str = "You are using testing Version " + QString::number(1621593042 - timeT) + " seconds remained";
        // QMessageBox::information(NULL, "INFO", str, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        QMessageBox::information(NULL, "INFO",  "You are using testing Version. License Expired!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return -1;
    }

    return a.exec();
}






