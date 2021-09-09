#ifndef AOI_NETWORK_H
#define AOI_NETWORK_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class aoi_network : public QObject
{
    Q_OBJECT
public:
    explicit aoi_network(QObject *parent = nullptr);
    void makeRequest(QString endpointRequest);  //Defining the function that makes the request
    void makeRequestBadPanels(QString endpointRequest);

signals:
    void dataReadReady(QByteArray);  // We emit this signal when the data from the API call comes back. Basically a ping that tells us something happend.
    void dataReadReadyBadPanels(QByteArray);

public slots:
    void readData(QNetworkReply *reply);
    // This is something that can recieve data. Through the connect function this slot will recieve data when the network manager "finishes"
    // Then we do something with the reply we got from the website and "emit" the signal dataReadReady containing the return data
    // Over in the main window class we connect the dataReadReady signal with the function that displays the data in the GUI
    // 1 -> we make a request with makeRequest
    //      main_gui_window::on_start_button_clicked calls the function aoi_network::makeRequest
    // 2 -> we define what happens when that request is done with "connect" by connecting it to the slot that we want to use.
    //      In this case we say "When qnam finishes give aoi_network::readData the data
    // 3 -> readData reads the data and emits it over the signal dataReadReady
    // 4 -> Over in the main window class we have connected aoi_network::dataReadReady with main_gui_window::displayData
    //      This means that when aoi_network::dataReadReady signal gets emit, main_gui_window::displayData recieves the data
    // 5 -> main_gui_window::displayData changes the type to string and displays it in the GUI

    void readDataBadPanels(QNetworkReply *reply);

private:
        QNetworkAccessManager *qnam = new QNetworkAccessManager(this);
        QNetworkAccessManager *qnamBadPanels = new QNetworkAccessManager(this);

};

#endif // AOI_NETWORK_H
