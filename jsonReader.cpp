#include "jsonReader.h"

jsonReader::jsonReader(QObject *parent) : QObject(parent)
{

}

QString jsonReader::searchForLotNum(QByteArray json_data, QString lotNum)
{
    QList<QJsonObject> listOfResults;
    QStringList listOfLotNum;

    lotNum = lotNum.simplified();
    lotNum.replace(" ","");

    listOfLotNum = lotNum.split(QRegularExpression(",|\\."), Qt::SkipEmptyParts); // The QRegExp splits the input by either dots or commas so multiple numbers can be used
    qDebug() << listOfLotNum;
    QJsonArray jsonDoc = QJsonDocument().fromJson(json_data).array();  // API data to json array
    for (int i = 0; i < jsonDoc.size(); ++i) {

        QString lotNumFromAPI = jsonDoc.at(i).toObject().value("Lot").toString();
        lotNumFromAPI = lotNumFromAPI.simplified();
        lotNumFromAPI.replace(" ","");
        if(listOfLotNum.contains(lotNumFromAPI)) {
            listOfResults << jsonDoc.at(i).toObject();
        }
    }
    QString resultMessageString = returnResultMessage(listOfResults);
    return resultMessageString;
}

QString jsonReader::returnResultMessage(QList<QJsonObject> listOfResults)
{
    QString baseMessageGood = "Die Rückmeldenummer %1 hat keine schlechten LPs!<br>Produktname: %2<br>Gute Panels: %3<br>OK Operator: %4<br>Gesamtmenge: %5<br><br>";
    QString baseMessageBad = "Die Rückmeldenummer %1 hat noch %2 schlechte %3<br>Produktname: %4<br>Gute Panels: %5<br>OK Operator: %6<br>Gesamtmenge: %7<br><br>";
    QString resultMessage;
    // For the string concatenation we need "Bad", "Good", "Lot", "OK operator", "Product" and "Total"

    for (int i = 0; i < listOfResults.size(); ++i) {

        QJsonObject currentProduct = listOfResults.at(i);

        QString lotNum = currentProduct.value("Lot").toString();
        QString badPanels = currentProduct.value("Bad").toString();
        QString badPanelsRed = QString("<span style=\" font-family:'Arial,Arial'; font-size:13pt; color:#FF0000;font-weight:600\">%1</span>").arg(badPanels);
        QString goodPanels = currentProduct.value("Good").toString();
        QString okOperator = currentProduct.value("OK operator").toString();
        QString productName = currentProduct.value("Product").toString();
        QString totalAmount = currentProduct.value("Total").toString();

        if(badPanels == "0") { // If this string is "0" it means that the product contains no bad panels
            resultMessage.append(baseMessageGood.arg(lotNum, productName, goodPanels, okOperator,totalAmount));
        } else {
            QString panelString;
            if(badPanels == "1"){panelString = "LP";} else{panelString = "LPs";}

            resultMessage.append(baseMessageBad.arg(lotNum, badPanelsRed, panelString, productName, goodPanels, okOperator, totalAmount));

          }

    }
    return resultMessage;
}



