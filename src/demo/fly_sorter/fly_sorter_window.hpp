#ifndef FLY_SORTER_WINDOW_HPP
#define FLY_SORTER_WINDOW_HPP

#include "ui_fly_sorter_window.h"
#include "parameters.hpp"
#include "image_grabber.hpp"
#include <memory>
#include <QMainWindow>
#include <QPointer>
#include <opencv2/core/core.hpp>

class QThreadPool;
class ImageGrabber;

class FlySorterWindow : public QMainWindow, private Ui::FlySorterWindow
{
    Q_OBJECT

    public:

        FlySorterWindow(QWidget *parent=0);

    signals:

        void stopCapture();

    private slots:

        void startPushButtonClicked();
        void httpOutputCheckBoxChanged(int state);
        void newImage(ImageData imageData);

    private:

        bool running_;
        FlySorterParam param_;
        QPointer<QThreadPool> threadPoolPtr_;
        QPointer<ImageGrabber> imageGrabberPtr_;
        QPixmap previewPixmapOrig_;
        QPixmap thresholdPixmapOrig_;

        void connectWidgets();
        void initialize();
        void updateImageLabels();

};

#endif // #ifndef FLY_SORTER_WINDOW_HPP
