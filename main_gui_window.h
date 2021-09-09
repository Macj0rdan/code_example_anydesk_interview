#ifndef MAIN_GUI_WINDOW_H
#define MAIN_GUI_WINDOW_H

#include <QMainWindow>
#include <aoi_network.h>
#include <jsonReader.h>
#include <QDebug>
#include <QDateTime>
#include <QCalendar>
#include <QHash>
#include <QList>
#include <QProgressBar>
#include <QApplication>
#include <QProcess>
#include <QCloseEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <Windows.h>
#include <QTimer>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class main_gui_window; }
QT_END_NAMESPACE

class main_gui_window : public QMainWindow
{
    Q_OBJECT

public:
    main_gui_window(QWidget *parent = nullptr);
    ~main_gui_window();


private slots:

    void on_start_button_clicked();

    void checkAndDisplayData(QByteArray data);

    void checkAndDisplayDataBadPanels(QByteArray data);

    void on_line_edit_lot_number_editingFinished();

    void getProductSummary();

    void getBadPanels();

    void on_line_edit_lot_number_returnPressed();

    QList<QString> getDate(QString dataString);

    void resetSearch();

    void disableSearchWidgets(bool state);

    void closeEvent(QCloseEvent *event);

    void adminMode();

    void clearLineEditLotNumber();

    void modifyLoadingBarStatus(QString bar_color, int percentage);

private:


    QString lotNum = "";

    Ui::main_gui_window *ui;

    aoi_network aoi_net;

    int searchCounter = 0;  // Counter to see if we searched through all 3 lines.
                            // If left out then this leads to race conditons

    QString noResultsMessage = "This is a bad result!";

    QString APP_NAME = "Removed_App_name";

    bool ADMIN_MODE = false;

    QString maxAmountDisplayed = "3000";

    QHash<int, QString> machineHash = {
        {0, "removed_name1"},
        {1, "removed_name2"},
        {2, "removed_name3"}
    };

};
#endif // MAIN_GUI_WINDOW_H
