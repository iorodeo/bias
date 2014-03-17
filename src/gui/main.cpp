#include <list>
#include <QApplication>
#include <QSharedPointer>
#include <QMessageBox>
#include "camera_window.hpp"
#include "camera_facade.hpp"
#include <iostream>


// ------------------------------------------------------------------------
// TO DO ... temporary main function. Currently just opens a camera
// window for each camera found attached to the system.
// ------------------------------------------------------------------------
int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    bias::GuidList guidList;
    bias::CameraFinder cameraFinder;
    std::list<QSharedPointer<bias::CameraWindow>> windowPtrList;

    // Get list guids for all cameras found
    try
    { 
        guidList = cameraFinder.getGuidList();
    }
    catch (bias::RuntimeError &runtimeError)
    {
        QString msgTitle("Camera Enumeration Error");
        QString msgText("Camera enumeration failed:\n\nError ID: ");
        msgText += QString::number(runtimeError.id());
        msgText += QString("\n\n");
        msgText += QString::fromStdString(runtimeError.what());
        QMessageBox::critical(0, msgTitle,msgText);
        return 0;
    }

    // If no cameras found - error
    if (guidList.empty()) 
    {
        QString msgTitle("Camera Enumeration Error");
        QString msgText("No cameras found");
        QMessageBox::critical(0, msgTitle,msgText);
        return 0;
    }

    // Open camera window for each camera 
    unsigned int camCnt;
    unsigned int numCam = guidList.size();
    QRect baseGeom;
    QRect nextGeom;
    bias::GuidList::iterator guidIt;
    for (guidIt=guidList.begin(), camCnt=0; guidIt!=guidList.end(); guidIt++, camCnt++)
    {
        bias::Guid guid = *guidIt;
        QSharedPointer<bias::CameraWindow> windowPtr(new bias::CameraWindow(guid, camCnt, numCam));
        windowPtr -> show();
        if (camCnt==0)
        {
            baseGeom = windowPtr -> geometry();
        }
        else
        {
            nextGeom.setX(baseGeom.x() + 40*camCnt);
            nextGeom.setY(baseGeom.y() + 40*camCnt);
            nextGeom.setWidth(baseGeom.width());
            nextGeom.setHeight(baseGeom.height());
            windowPtr -> setGeometry(nextGeom);
        }
        windowPtrList.push_back(windowPtr);
    }
    return app.exec();
}

