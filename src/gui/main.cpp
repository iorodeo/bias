#include <iostream>
#include <QApplication>

#include "camera_window.hpp"


int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    bias::CameraWindow cameraWindow;
    if (cameraWindow.haveCamera())
    {
        std::cout << "have camera" << std::endl ;
        cameraWindow.show();
        return app.exec();
    }
    else
    {
        std::cout << "No cameras found - exiting" << std::endl << std::flush;
        return 0;
    }
}

