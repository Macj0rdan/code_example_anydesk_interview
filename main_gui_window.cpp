#include "main_gui_window.h"
#include "ui_main_gui_window.h"

main_gui_window::main_gui_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::main_gui_window)
{
    ui->setupUi(this);
    connect(&aoi_net,&aoi_network::dataReadReady,this,&main_gui_window::checkAndDisplayData); // Signal for return of api data
    connect(&aoi_net,&aoi_network::dataReadReadyBadPanels,this,&main_gui_window::checkAndDisplayDataBadPanels); // Signal for return of api data
    ui->date_select->setCurrentIndex(2);
    setWindowTitle(APP_NAME);
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this, SLOT(adminMode()));
    ui->line_edit_lot_number->setFocus();
}

main_gui_window::~main_gui_window()
{
    delete ui;
}


void main_gui_window::on_start_button_clicked()
{
    getProductSummary();
    getBadPanels();
}


void main_gui_window::checkAndDisplayData(QByteArray data)
{
    // this function gets called every time the network manager finishes and returns with api data
    QString dataString = jsonReader().searchForLotNum(data, lotNum);  // the lotnumber gets updated everytime the user finishes editing the lot number line edit
//    qDebug() << dataString;

    if(dataString.size() == 0 and searchCounter == 2 and ui->result_box->toPlainText() == ""){
        // We check if the data string is empty, the search went through all 3 lines and the box does not contain text
        // This means we found no results and we set the text to an error message and show a red progress bar
        ui->result_box->setText(noResultsMessage);
        int loadingBarPercentage = 100;
        modifyLoadingBarStatus("red", loadingBarPercentage);

    } else if (dataString.size() != 0){
        // If the data string contains any text append it to the result box
        ui->result_box->append(dataString);

    } if(ui->result_box->toPlainText() != noResultsMessage and searchCounter == 2) {
        // If the result box doesnt contain the noResultsMessage and we are done with the search show green progress bar
        //todo: Change this part so it doesnt need to check for the no results message
        int loadingBarPercentage = 100;
        modifyLoadingBarStatus("green", loadingBarPercentage);
    }
    searchCounter += 1;
    // this needs to be incremented here or we get a race condition because the loop that I use to search all 3 machines increments the counter before the search for even 1 is done
    if(ui->line_edit_lot_number->text() == "good"){
        ui->result_box->setText("This is a good result!");
        int loadingBarPercentage = 100;
        modifyLoadingBarStatus("green", loadingBarPercentage);
    }
    if(searchCounter == 3){
        searchCounter = 0;
        disableSearchWidgets(false);
        ui->line_edit_lot_number->setFocus();
    } // reset search counter and renable search widgets when work is done

}

void main_gui_window::checkAndDisplayDataBadPanels(QByteArray data)
{
    QStringList listOfResults;
    QJsonArray jsonDoc = QJsonDocument().fromJson(data).array();  // API data to json array
    // Looping through every single result as we need to display duplicates
    for (int i = 0; i < jsonDoc.size(); ++i) {

        QString badPanelID = jsonDoc.at(i).toObject().value("Panel ID Code").toString();
        if(!listOfResults.contains(badPanelID)){
            listOfResults.append(badPanelID);
            ui->resultBoxCodes->append(badPanelID);
        }

    }
}



void main_gui_window::on_line_edit_lot_number_editingFinished()
{
    lotNum = ui->line_edit_lot_number->text();
}

void main_gui_window::getProductSummary()
{
    if(lotNum.size() == 0 or searchCounter != 0){  // Text field for lot num is empty -> dont do anything
        return;
    }
    resetSearch(); // reset vars and fields
    disableSearchWidgets(true); // disable button so multiple searches arent possible
    // Set the progressbar to green and 50% to indicate something happened
    int loadingBarPercentage = 50;
    modifyLoadingBarStatus("green", loadingBarPercentage);


    QList<QString> dateList = this->getDate(this->ui->date_select->currentText());  // get date to use in link from date dropdown box
    QString dateMin = dateList[0];
    QString dateMax = dateList[1];
    QString lotNumSummaryUrl;
    for (int i = 0; i < 3; i++){
        lotNumSummaryUrl = "removed_link%1%2%3%4";
        QString machineId = machineHash.value(i);
        lotNumSummaryUrl = lotNumSummaryUrl.arg(maxAmountDisplayed, machineId, dateMin, dateMax);
        aoi_net.makeRequest(lotNumSummaryUrl);
        // Sends out 3 request for all 3 machines. Runs over different thread
    }
}

void main_gui_window::getBadPanels()
{

    lotNum = ui->line_edit_lot_number->text().simplified();
    lotNum.replace(" ","");
    QStringList listOfLotNum;
    listOfLotNum = lotNum.split(QRegularExpression(",|\\."), Qt::SkipEmptyParts); // The QRegExp splits the input by either dots or commas so multiple numbers can be used

    for (int i = 0; i < 3; i++){ // This is the loop for the 3 machines. We need an inner loop for every single lot number
        QString machineId = machineHash.value(i);

        for (int i = 0; i < listOfLotNum.size(); i++){ // This inner loops sends a request to the API for every single lot number in search bar
            QString badPanelsUrl = "removed_link%1%2%3%4";
            badPanelsUrl = badPanelsUrl.arg(maxAmountDisplayed, machineId, listOfLotNum.value(i));
            qDebug() << badPanelsUrl;
            aoi_net.makeRequestBadPanels(badPanelsUrl);
            // Sends out 3 request for all 3 machines. Runs over different thread
        }
    }
}


void main_gui_window::on_line_edit_lot_number_returnPressed()
{
    lotNum = ui->line_edit_lot_number->text(); // on editing finished for the line edit is not fast enough so we need to press return twice in some cases
    getProductSummary();
    getBadPanels();
}


QList<QString> main_gui_window::getDate(QString dataString)
{
    // This is a bit unwieldy but required as the API requires specific date formats
    // We are returning a date hashmap here with start and end of the specified timeframe
    QString startOfDayHours = "%2000%3A00%3A00";
    QString endOfDayHours   = "%2023%3A59%3A59";
    
    QDateTime timeNow = QDateTime().currentDateTime();
    
    QDateTime thirtyDaysAgo = timeNow.addDays(-30);
    QDateTime threeHundredSixtyFiveDaysAgo = timeNow.addDays(-365);
    
    QString currentDay = timeNow.toString("dd");
    QString yesterday = timeNow.addDays(-1).toString("dd");
    
    QString currentMonth = timeNow.toString("MM");
    QString lastMonth = timeNow.addMonths(-1).toString("MM");
    
    QString currentYear = timeNow.toString("yyyy");
    QString lastYear = timeNow.addYears(-1).toString("yyyy");
    
    QHash<QString, QList<QString> > dateHashMap = {
        // date format output is YYYY-MM-DD HH:MM:SS
        {"Dieser Monat",
         {
             QString("%1-%2-01").arg(currentYear, currentMonth),
             QString("%1-%2-%3").arg(currentYear, currentMonth, QString::number(QCalendar().daysInMonth(currentMonth.toInt(), currentYear.toInt())))
         }},
        {"Letzter Monat",
         {
             QString("%1-%2-01").arg(currentYear, lastMonth),
             QString("%1-%2-%3").arg(currentYear, lastMonth, QString::number(QCalendar().daysInMonth(lastMonth.toInt(), currentYear.toInt())))
         }},
        {"Letzten 30 Tage",
         {
             QString("%1-%2-%3%4").arg(thirtyDaysAgo.toString("yyyy"),thirtyDaysAgo.toString("MM"), thirtyDaysAgo.toString("dd"), startOfDayHours),
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, currentDay, endOfDayHours)
         }},
        {"Heute",
         {
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, currentDay, startOfDayHours),
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, currentDay, endOfDayHours)
         }},
        {"Gestern",
         {
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, yesterday, startOfDayHours),
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, yesterday, endOfDayHours)
         }},
        {"Dieses Jahr",
         {
             QString("%1-01-01").arg(currentYear),
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, currentDay, endOfDayHours)
         }},
        {"Letztes Jahr",
         {
             QString("%1-01-01").arg(lastYear),
             QString("%1-12-31").arg(lastYear)
         }},
        {"Letzten 365 Tage",
         {
             QString("%1-%2-%3%4").arg(threeHundredSixtyFiveDaysAgo.toString("yyyy"),threeHundredSixtyFiveDaysAgo.toString("MM"), threeHundredSixtyFiveDaysAgo.toString("dd"), startOfDayHours),
             QString("%1-%2-%3%4").arg(currentYear, currentMonth, currentDay, endOfDayHours)
         }}
        
    };
    return dateHashMap.value(dataString);
}

void main_gui_window::resetSearch()
{
    // Function to reset all the variables and fields for a new search
    ui->result_box->clear();
    ui->resultBoxCodes->clear();
    searchCounter = 0;
}

void main_gui_window::disableSearchWidgets(bool state)
{
    // disable widgets use for search to stop more then one search from running
    ui->start_button->setDisabled(state);
    ui->line_edit_lot_number->setDisabled(state);
}

void main_gui_window::closeEvent (QCloseEvent *event)
{
    if((GetKeyState(VK_CONTROL)& 0x8000) or ADMIN_MODE) // if admin mode is turned on or ctrl is held down the programm exits without question
    {
        qDebug() << "Exiting because control is pressed";
        exit(0); // Close window instead of showing question box
    }

    qDebug() << "close event";
    // Most of this is to translate buttons to german
    // Todo: Do this over the translate feature of QT
    QPushButton *yesButtonGerman = new QPushButton(tr("Ja"), this);
    QPushButton *noButtonGerman = new QPushButton(tr("Nein"), this);
    QMessageBox exitEventMessageBox;
    exitEventMessageBox.addButton(yesButtonGerman, QMessageBox::YesRole);
    exitEventMessageBox.addButton(noButtonGerman, QMessageBox::NoRole);
    exitEventMessageBox.setText("Programm schließen?");
    exitEventMessageBox.setWindowTitle(" ");

    int exitMessageBoxExec = exitEventMessageBox.exec();
    if (exitMessageBoxExec) {
        event->ignore();
    } else {
        event->accept();
    }
    delete yesButtonGerman;
    delete noButtonGerman;
}

void main_gui_window::adminMode()
{
    // function to enable admin mode for more features
    QString adminKeyphrase = "qwertz#";
    QString clearPhrase = "clear";
    QString colorTestPhrase = "color";
    QString lineEditLotNumText = ui->line_edit_lot_number->text();
    int TIME_BEFORE_TEXTBOX_CLEAR = 1000;  // milliseconds

    if((lineEditLotNumText == adminKeyphrase) &! (ADMIN_MODE)){
        ADMIN_MODE = true;
        ui->line_edit_lot_number->setText("Admin mode enabled");

    }
    if(ADMIN_MODE)
    {
        if((lineEditLotNumText == clearPhrase)){
            ui->result_box->clear();
            ui->resultBoxCodes->clear();
            ui->line_edit_lot_number->clear();
    }
        if(lineEditLotNumText == colorTestPhrase)
        {
            modifyLoadingBarStatus("blue", 23);
            qDebug() << "Loading Bar modified";
        }
    }
    else {ADMIN_MODE = false;} // disable admin mode if phrase is not in lot edit
    QTimer::singleShot(TIME_BEFORE_TEXTBOX_CLEAR, this, SLOT(clearLineEditLotNumber())); // timer to clear textbox at the end of the admin function

}

void main_gui_window::clearLineEditLotNumber()
{
    ui->line_edit_lot_number->clear();
}

void main_gui_window::modifyLoadingBarStatus(QString bar_color, int percentage)
{
    // Function to change loading bar
    QString colorCode = QString("background-color: %1;").arg(bar_color);
    ui->search_progress_bar->setStyleSheet("QProgressBar::chunk {" +
                                           colorCode +
                                           "border: 1px red;"
                                           "border-radius: 1px; }"
                                           "QProgressBar {"
                                           "background-color: grey;"
                                           "border: 0px grey;"
                                           "border-radius: 0px;}");
    ui->search_progress_bar->setValue(percentage);
}
