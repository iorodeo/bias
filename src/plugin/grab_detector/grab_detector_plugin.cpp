#include "grab_detector_plugin.hpp"
#include "image_label.hpp"
#include <QtDebug>
#include <QTimer>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <sstream>
#include <iostream>

namespace bias
{
    // Public static variables 
    // ------------------------------------------------------------------------
    int GrabDetectorPlugin::DEFAULT_XPOS = 300;
    int GrabDetectorPlugin::DEFAULT_YPOS = 200;
    int GrabDetectorPlugin::DEFAULT_WIDTH = 50;
    int GrabDetectorPlugin::DEFAULT_HEIGHT = 200;

    int GrabDetectorPlugin::DEFAULT_TRIGGER_ARMED = false;
    int GrabDetectorPlugin::DEFAULT_TRIGGER_THRESHOLD = 100;
    int GrabDetectorPlugin::DEFAULT_TRIGGER_FILTER_SIZE = 7; // must be odd

    int GrabDetectorPlugin::DEFAULT_LIVEPLOT_UPDATE_DT = 75;
    double GrabDetectorPlugin::DEFAULT_LIVEPLOT_TIME_WINDOW = 10.0; 
    double GrabDetectorPlugin::DEFAULT_LIVEPLOT_SIGNAL_WINDOW = 255.0;

    // Public Methods
    // ------------------------------------------------------------------------
    
    GrabDetectorPlugin::GrabDetectorPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr) : BiasPlugin(parentPtr)
    {
        imageLabelPtr_ = imageLabelPtr;
        setupUi(this);
        connectWidgets();
        initialize();
    }

    void GrabDetectorPlugin::processFrames(QList<StampedImage> frameList)
    {
        int medianFilterSize = getMedianFilter();
        int threshold = getThreshold();
        bool found = false;
        double signalMin; 
        double signalMax;

        StampedImage latestFrame = frameList.back();
        frameList.clear();

        cv::Mat workingImage = latestFrame.image.clone();
        cv::Rect boxRect = getDetectionBoxCv();
        cv::Mat roiImage = workingImage(boxRect);
        cv::medianBlur(roiImage,roiImage,medianFilterSize);
        cv::minMaxLoc(roiImage,&signalMin,&signalMax);
        if (signalMax > threshold)
        {
            found = true;
        }
        else
        { 
            found = false;
        }
        acquireLock();
        currentImage_ = workingImage;
        signalMin_ = signalMin;
        signalMax_ = signalMax;
        found_ = found;
        frameCount_ = latestFrame.frameCount;
        livePlotTimeVec_.append(latestFrame.timeStamp);
        livePlotSignalVec_.append(signalMax);
        releaseLock();

        //qDebug() << signalMax << threshold << found;
    }

    cv::Mat GrabDetectorPlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImage = currentImage_;
        int signalMin = signalMin_;
        int signalMax = signalMax_;
        bool found = found_;
        int frameCount = frameCount_;
        releaseLock();

        cv::Rect boxRect = getDetectionBoxCv();
        cv::Scalar boxColor(0,0,255);
        int boxLineWidth = 3;

        std::stringstream foundStream;
        if (found)
        {
            foundStream << "object found";
        }
        cv::Mat currentImageBGR;
        cv::cvtColor(currentImage, currentImageBGR, CV_GRAY2BGR);
        cv::rectangle(currentImageBGR, boxRect,boxColor, boxLineWidth);

        double fontScale = 1.0;
        int thickness = 2;
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(foundStream.str(), CV_FONT_HERSHEY_SIMPLEX, fontScale, thickness, &baseline);
        cv::Point textPoint(currentImage.cols/2 - textSize.width/2, textSize.height+baseline);
        cv::putText(currentImageBGR, foundStream.str(), textPoint, CV_FONT_HERSHEY_SIMPLEX, fontScale, boxColor,thickness);
        return currentImageBGR;
    }


    cv::Rect GrabDetectorPlugin::getDetectionBoxCv()
    {
        QRect box = getDetectionBox();
        cv::Rect boxCv(box.x(), box.y(), box.width(), box.height());
        return boxCv;
    }

    QRect GrabDetectorPlugin::getDetectionBox()
    {
        QRect box = QRect( 
                xPosSpinBoxPtr   -> value(),
                yPosSpinBoxPtr   -> value(),
                widthSpinBoxPtr  -> value(),
                heightSpinBoxPtr -> value()
                );

        return box;
    }

    void GrabDetectorPlugin::setDetectionBox(QRect box)
    {
        xPosSpinBoxPtr -> setValue(box.x());
        yPosSpinBoxPtr -> setValue(box.y());
        widthSpinBoxPtr -> setValue(box.width());
        heightSpinBoxPtr -> setValue(box.height());
    }


    int GrabDetectorPlugin::getThreshold()
    {
        return trigThresholdSpinBoxPtr -> value();
    }


    int GrabDetectorPlugin::getMedianFilter()
    {
        return trigMedianFilterSpinBoxPtr -> value();
    }

    // Protected Methods
    // ------------------------------------------------------------------------
    
    void GrabDetectorPlugin::connectWidgets()
    {
        connect(
                comPortComboBoxPtr,
                SIGNAL(currentIndexChanged(QString)),
                this,
                SLOT(comPortComboBoxIndexChanged(QString))
                );

        connect(
                connectPushButtonPtr,
                SIGNAL(pressed()),
                this,
                SLOT(connectPushButtonPressed())
               );

        connect(
                connectPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(connectPushButtonClicked())
               );

        connect(
                levelDblSpinBoxPtr,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(levelDblSpinBoxValueChanged(double))
               );

        connect(
                durationDblSpinBoxPtr,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(durationDblSpinBoxValueChanged(double))
               );

        connect(
                colorSelectPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(colorSelectPushButtonClicked())
               );

        connect(
                trigResetPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(trigResetPushButtonClicked())
               );

        connect(
                imageLabelPtr_,
                SIGNAL(selectBoxChanged(QRect)),
                this,
                SLOT(detectionBoxChanged(QRect))
               );

    }


    void GrabDetectorPlugin::initialize()
    {
        found_ = false;
        signalMax_ = 0.0;
        signalMin_ = 0.0;
        frameCount_ = 0;

        livePlotUpdateDt_ = DEFAULT_LIVEPLOT_UPDATE_DT;
        livePlotTimeWindow_ = DEFAULT_LIVEPLOT_TIME_WINDOW;
        livePlotSignalWindow_ = DEFAULT_LIVEPLOT_SIGNAL_WINDOW;

        // Add Dummy com port items for testing
        for (int i=1; i<=5; i++)
        {
            QString comPortStr = QString("COM%1").arg(i);
            comPortComboBoxPtr -> addItem(comPortStr);
        }


        // Set trigger threshold and filter size (TEMPORARY)
        trigThresholdSpinBoxPtr -> setValue(DEFAULT_TRIGGER_THRESHOLD);
        trigMedianFilterSpinBoxPtr ->  setValue(DEFAULT_TRIGGER_FILTER_SIZE);

        // Setup live plot
        livePlotPtr -> addGraph();
        livePlotPtr -> addGraph();
        livePlotPtr -> graph(0) -> setPen(QPen(QColor(0,0,255,255),1.5));
        livePlotPtr -> graph(1) -> setPen(QPen(QColor(100,100,100,255),2.0));
        livePlotPtr -> xAxis -> setRange(-livePlotTimeWindow_,0);
        livePlotPtr -> yAxis -> setRange(0,livePlotSignalWindow_);
        livePlotPtr -> xAxis -> setLabel("time (sec)");
        livePlotPtr -> replot();

        // Setup plot update timer
        livePlotUpdateTimerPtr_ = new QTimer(this);
        connect(livePlotUpdateTimerPtr_, SIGNAL(timeout()), this, SLOT(updateLivePlotOnTimer()));
        livePlotUpdateTimerPtr_ -> start(livePlotUpdateDt_);

    }


    // Private Slots
    // ------------------------------------------------------------------------
    
    void GrabDetectorPlugin::comPortComboBoxIndexChanged(QString text)
    {
        qDebug() << "com port changed: " << text;
    }


    void GrabDetectorPlugin::connectPushButtonPressed()
    {
        qDebug() << "connect button pressed";
    }


    void GrabDetectorPlugin::connectPushButtonClicked()
    {
        qDebug() << "connect button clicked";
    }


    void GrabDetectorPlugin::levelDblSpinBoxValueChanged(double value)
    {
        qDebug() << "level value chagned: " << value;
    }


    void GrabDetectorPlugin::durationDblSpinBoxValueChanged(double value)
    {
        qDebug() << "duration value changed: " << value;
    }


    void GrabDetectorPlugin::colorSelectPushButtonClicked()
    {
        qDebug() << "color select push buttton clicked";
    }


    void GrabDetectorPlugin::trigResetPushButtonClicked()
    {
        qDebug() << "trigger reset push button clicked";
    }


    void GrabDetectorPlugin::detectionBoxChanged(QRect box)
    {
        if (isActive() && pluginsEnabled())
        {
            setDetectionBox(box);
        }
    }

    void GrabDetectorPlugin::updateLivePlotOnTimer()
    {
        if (livePlotTimeVec_.empty())
        {
            return;
        }

        acquireLock();
        double lastTime = livePlotTimeVec_.last();
        double firstTime = livePlotTimeVec_.first();

        if (lastTime < firstTime)
        {
            livePlotTimeVec_.clear();
            livePlotSignalVec_.clear();
            releaseLock();
            return;
        }

        while (lastTime - firstTime > livePlotTimeWindow_)
        {
            livePlotTimeVec_.pop_front();
            livePlotSignalVec_.pop_front();
            firstTime = livePlotTimeVec_.first();
        }

        double threshold = double(getThreshold());
        QVector<double> threshSignalVec = {threshold, threshold};
        QVector<double> threshTimeVec;
        if (lastTime < livePlotTimeWindow_)
        {
            double windowStartTime= -livePlotTimeWindow_ + lastTime;
            livePlotPtr -> xAxis -> setRange(windowStartTime,lastTime);
            threshTimeVec = QVector<double>({windowStartTime, lastTime});

        }
        else
        {
            livePlotPtr -> xAxis -> setRange(firstTime, lastTime);
            threshTimeVec = QVector<double>({firstTime, lastTime});
        }

        livePlotPtr -> graph(0) -> setData(livePlotTimeVec_,livePlotSignalVec_);

        livePlotPtr -> graph(1) -> setData(threshTimeVec, threshSignalVec);
        livePlotPtr -> replot();
        releaseLock();

    }

}
