#ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
#define BIAS_TEST_GUI_MAINWINDOW_HPP

#include <QMainWindow>
#include "ui_main_window.h"
#include "camera.hpp"
#include "image_pool.hpp"

class QTimer;
class QThread;
class ImageGrabber;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);
        bool haveCamera();

    protected:
        void resizeEvent(QResizeEvent *event);
        void closeEvent(QCloseEvent *event);

    private slots:
        void startButtonClicked();
        void stopButtonClicked();
        void timerUpdate();
        void testTimerUpdate();

    private:

        bool haveCamera_;

        QTimer *timer_;
        QTimer *testTimer_;

        ImageGrabber *imageGrabber_;

        bool havePixmap_;
        QPixmap pixmapOriginal_;

        bias::CameraPtr cameraPtr_;
        unsigned int counter_;
        ImagePool imagePool_;

        void initialize();
        void connectWidgets();
        void createCamera();
        void updateImageLabel();



       
}; // class MainWindow


#endif // #ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
