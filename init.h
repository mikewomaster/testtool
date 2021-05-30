#ifndef INIT_H
#define INIT_H

#include <QCoreApplication>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

class Init
{
public:
    Init(short type = 0);
    void initEnvironmentJSON(short type = 0);
    QJsonArray cellularArray;
    QJsonArray mqttArray;
    QJsonArray configArray;
    QJsonArray meterArray;
    QJsonArray meterTagArray;
    QJsonArray mbusArray;
    int Size;
};

#endif // INIT_H
