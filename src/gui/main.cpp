#include <iostream>
#include <QApplication>

#include "main_window.hpp"


int main (int argc, char *argv[])
{

    QApplication app(argc, argv);
    MainWindow win;
    if (win.haveCamera())
    {
        std::cout << "have camera" << std::endl ;
        win.show();
        return app.exec();
    }
    else
    {
        std::cout << "No cameras found - exiting" << std::endl << std::flush;
        return 0;
    }
}

