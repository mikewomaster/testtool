#include "mainwindow.h"
#include "init.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}






