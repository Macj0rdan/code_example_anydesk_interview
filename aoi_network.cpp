#include "aoi_network.h"

aoi_network::aoi_network(QObject *parent) : QObject(parent)
{
    connect(qnam, &QNetworkAccessManager::finished,this,&aoi_network::readData); //when this network manager is "finished" it "emits" its network reply to the slot readData()
    connect(qnamBadPanels, &QNetworkAccessManager::finished,this,&aoi_network::readDataBadPanels);
}

void aoi_network::makeRequest(QString endpointRequest)
{
    qnam->get(QNetworkRequest(QUrl(endpointRequest)));
}

void aoi_network::makeRequestBadPanels(QString endpointRequest)
{
    qnamBadPanels->get(QNetworkRequest(QUrl(endpointRequest)));
}


void aoi_network::readData(QNetworkReply *reply){
    QByteArray request_data;
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!= 200){
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
        emit(dataReadReady(request_data)); // emitting signal here for test purposes and on error
        return;  // stop when reply doesnt have status code 200/ok
    }
    request_data = reply->readAll();

//    qInfo() << "Make request signal returned -> emiting data";
    emit(dataReadReady(request_data));
}


void aoi_network::readDataBadPanels(QNetworkReply *reply){
    QByteArray request_data;
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!= 200){
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
        emit(dataReadReadyBadPanels(request_data)); // emitting signal here for test purposes and on error
        return;  // stop when reply doesnt have status code 200/ok
    }
    request_data = reply->readAll();

//    qInfo() << "Make request signal returned -> emiting data";
    emit(dataReadReadyBadPanels(request_data));
}
