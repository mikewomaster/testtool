#include "init.h"
#include "para.h"

Init::Init()
{
    initEnvironmentJSON();
}

#define BUG_ON 0
void Init::initEnvironmentJSON()
{
    QFile file("config2.json");
    if(!file.open(QIODevice::ReadWrite)) {
        QMessageBox::information(nullptr, "File", "Can't find config.json.");
        return;
    }

    QByteArray ba = file.readAll();
    QJsonParseError e;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ba, &e);

    if(e.error == QJsonParseError::NoError && !jsonDoc.isNull()) {
        QJsonObject obj;
        obj = jsonDoc.object();
        cellularArray =  obj["cellular"].toArray();
        mqttArray = obj["mqtt"].toArray();

    #if BUG_ON
        qDebug() << cellularArray[0].toObject()["username"].toString();
        qDebug() << cellularArray[0].toObject()["password"].toString();
        qDebug() << mqttArray[0].toObject()["username"].toString();
        qDebug() << mqttArray[0].toObject()["password"].toString();
        qDebug() << mqttArray[0].toObject()["port"].toInt();

        for(int i = 0; i < Array.count(); i++) {
           QJsonObject arrObj= Array[i].toObject();
           qDebug()<<arrObj["Model Name"].toString();
           qDebug()<<arrObj["SN"].toString();
           qDebug()<<arrObj["SF"].toInt();
           qDebug()<<arrObj["CR"].toString();
           qDebug()<<arrObj["Power"].toInt();
           qDebug()<<arrObj["Frequency"].toDouble();
           qDebug()<<arrObj["Bandwidth"].toInt();
        }
    #endif
    } else {
        QMessageBox::information(nullptr, "File", "Please Check config file again.");
    }

    file.close();
}
