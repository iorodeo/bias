#ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
#define BIAS_TEST_GUI_MAINWINDOW_HPP

#include <QMainWindow>
#include "ui_main_window.h"
#include "camera.hpp"
#include "image_pool.hpp"

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
        void testTimerUpdate();

    private:

        QTimer *timer_;

        bool havePixmap_;
        QPixmap pixmapOriginal_;

        bias::CameraPtr cameraPtr_;
        unsigned int counter_;
        ImagePool imagePool_;

        void initialize();
        void connectWidgets();
        void createCamera();
        void updateImageLabel();

        void resizeEvent(QResizeEvent *event);

        QTimer *testTimer_;

       
}; // class MainWindow


#endif // #ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
