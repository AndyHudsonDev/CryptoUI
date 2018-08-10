#include "mainwindow.h"
#include "stream.h"
#include "view.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // view marketModel;
    stream s;
    MainWindow w;
    QObject::connect(&w, SIGNAL(StartClicked()), &s, SLOT(Start()));
    QObject::connect(&s, SIGNAL(DataUpdate(std::string)), &w, SLOT(marketUpdate(std::string)));

    w.show();

    return a.exec();
}
