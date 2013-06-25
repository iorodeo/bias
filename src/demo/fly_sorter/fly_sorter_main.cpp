#include <QApplication>
#include <iostream>
#include "fly_sorter_window.hpp"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    FlySorterWindow *windowPtr = new FlySorterWindow();
    windowPtr -> show();
    return app.exec();
}


