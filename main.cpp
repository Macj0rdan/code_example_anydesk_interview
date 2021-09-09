#include "main_gui_window.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("name_removed.ico"));
    main_gui_window w;
    w.show();
    return a.exec();
}
