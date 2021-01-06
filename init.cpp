#include "init.h"
#include "para.h"

Init::Init()
{
}

void Init::initEnvironmentJSON()
{
    QFile file("config.json");
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
        Array =  obj["Machine"].toArray();
        Size = Array.count();

    #if BUG_ON
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
