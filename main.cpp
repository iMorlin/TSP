#include "cmainwinwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMainWinWidget w;
    w.resize(800, 800);
    w.show();

    return a.exec();
}