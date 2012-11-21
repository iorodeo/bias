#ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
#define BIAS_TEST_GUI_MAINWINDOW_HPP

#include "ui_main_window.h"
#include <QMainWindow>
#include "camera.hpp"

class QTimer;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);

    private slots:
        void startButtonClicked();
        void stopButtonClicked();
        void timerUpdate();

    private:

        QTimer *timer_;

        QPixmap pixmapScaled_; 
        QPixmap pixmapOriginal_;

        bias::CameraPtr cameraPtr_;
        unsigned int counter_;

        void initialize();
        void connectWidgets();
        void createCamera();
        void updateImageLabel();

        void resizeEvent(QResizeEvent *event);
       
}; // class MainWindow


#endif // #ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
