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
    Init();
    void initEnvironmentJSON();
    QJsonArray Array;
    int Size;
};

#endif // INIT_H
