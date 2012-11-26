#ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
#define BIAS_TEST_GUI_MAINWINDOW_HPP

#include <QMainWindow>
#include "ui_main_window.h"
#include "camera.hpp"
#include "image_pool.hpp"

class QTimer;
class QThreadPool;
class ImageGrabber;
class ImageProcessor;

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
        void updateImageDisplay();

    private:

        unsigned int counter_;
        QTimer *imageDisplayTimer_;

        QThreadPool *threadPool_;

        bool haveCamera_;
        bias::CameraPtr cameraPtr_;
        bias::ImagePoolPtr imagePoolPtr_;

        ImageGrabber *imageGrabber_;
        ImageProcessor *imageProcessor_;

        bool havePixmap_;
        QPixmap pixmapOriginal_;

        void initialize();
        void connectWidgets();
        void createCamera();
        void updatePixmap();
        void updateImageLabel();



       
}; // class MainWindow


#endif // #ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
