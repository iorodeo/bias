#ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
#define BIAS_TEST_GUI_MAINWINDOW_HPP

#include <memory>
#include <QPointer>
#include <QSharedPointer>
#include <QMainWindow>
#include "ui_main_window.h"
#include "camera_facade_fwd.hpp"

class QTimer;
class QThreadPool;

namespace bias 
{

    class ImageGrabber;
    class ImageDispatcher;
    struct StampedImage;
    template <class T> class LockableQueue;

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

            bool haveCamera_;
            bool havePixmap_;

            QPixmap pixmapOriginal_;

            std::shared_ptr<Camera> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            QPointer<QThreadPool> threadPoolPtr_;
            QPointer<QTimer> imageDisplayTimerPtr_;

            QPointer<ImageGrabber> imageGrabber_;
            QPointer<ImageDispatcher> imageDispatcher_;

            void initialize();
            void connectWidgets();
            void createCamera();
            void updatePixmap();
            void updateImageLabel();


    }; // class MainWindow

}

#endif // #ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
