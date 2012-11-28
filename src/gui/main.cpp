#include <list>
#include <iostream>
#include <QApplication>
#include <QSharedPointer>

#include "camera_window.hpp"
#include "camera_facade.hpp"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    bias::CameraFinder cameraFinder;
    bias::GuidList guidList = cameraFinder.getGuidList();
    std::list<QSharedPointer<bias::CameraWindow>> windowPtrList; // Not the best way

    if (guidList.empty()) 
    {
        std::cout << "No cameras found - exiting" << std::endl << std::flush;
        return 0;

    }
    else
    {
        bias::GuidList::iterator guidIt;
        for (guidIt=guidList.begin(); guidIt!=guidList.end(); guidIt++)
        {
            bias::Guid guid = *guidIt;
            QSharedPointer<bias::CameraWindow> windowPtr(new bias::CameraWindow(guid));
            windowPtr -> show();
            windowPtrList.push_back(windowPtr);
        }
        return app.exec();
    }
}

