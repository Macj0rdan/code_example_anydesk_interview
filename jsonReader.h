#ifndef JSONREADER_H
#define JSONREADER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>
#include <QMapIterator>

class jsonReader : public QObject
{
    Q_OBJECT
public:
    explicit jsonReader(QObject *parent = nullptr);

    QString searchForLotNum(QByteArray json_data, QString lotNum);

private:
    QString returnResultMessage(QList<QJsonObject> listOfResults);

signals:

};

#endif // JSONREADER_H
