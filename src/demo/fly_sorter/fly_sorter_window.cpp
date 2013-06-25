#include "fly_sorter_window.hpp"
#include <QMessageBox>
#include <iostream>
#include <list>

// Public Methods
// ----------------------------------------------------------------------------

FlySorterWindow::FlySorterWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    connectWidgets();
}

// Private Slots
// ----------------------------------------------------------------------------

//void FlySorterWindow::connectPushButtonClicked()
//{
//    CameraFinder cameraFinder;
//    CameraPtrList cameraPtrList;  
//
//    // Get list guids for all cameras found
//    try
//    { 
//        cameraPtrList = cameraFinder.createCameraPtrList();
//    }
//    catch (bias::RuntimeError &runtimeError)
//    {
//        QString msgTitle("Camera Enumeration Error");
//        QString msgText("Camera enumeration failed:\n\nError ID: ");
//        msgText += QString::number(runtimeError.id());
//        msgText += QString("\n\n");
//        msgText += QString::fromStdString(runtimeError.what());
//        QMessageBox::critical(0, msgTitle,msgText);
//        return;
//    }
//
//    // If no cameras found - error
//    if (cameraPtrList.empty()) 
//    {
//        QString msgTitle("Camera Enumeration Error");
//        QString msgText("No cameras found");
//        QMessageBox::critical(0, msgTitle,msgText);
//        return;
//    }
//
//    // Grab first camera
//}


void FlySorterWindow::startPushButtonClicked()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}


void FlySorterWindow::httpOutputCheckBoxChanged(int state)
{
    std::cout << __PRETTY_FUNCTION__ << ", state = " << state << std::endl;
}

// Private Methods
// ----------------------------------------------------------------------------

void FlySorterWindow::connectWidgets()
{

    connect(
            startPushButtonPtr_,
            SIGNAL(clicked()),
            this,
            SLOT(startPushButtonClicked())
           );

    connect(
            httpOutputCheckBoxPtr_,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(httpOutputCheckBoxChanged(int))
           );
}


