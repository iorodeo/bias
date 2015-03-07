#include "grab_detector_plugin.hpp"
#include "image_label.hpp"
#include <QtDebug>
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

    // Public Methods
    // ------------------------------------------------------------------------
    
    GrabDetectorPlugin::GrabDetectorPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr) : BiasPlugin(parentPtr)
    {
        imageLabelPtr_ = imageLabelPtr;
        setupUi(this);
        connectWidgets();
        initialize();
    }

    void GrabDetectorPlugin::processFrame(StampedImage frame)
    {
        int medianFilterSize = getMedianFilter();
        int threshold = getThreshold();
        bool found = false;
        double signalMin; 
        double signalMax;

        cv::Mat workingImage = frame.image.clone();
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

        qDebug() << signalMax << threshold << found;

        acquireLock();
        currentImage_ = workingImage;
        signalMin_ = signalMin;
        signalMax_ = signalMax;
        found_ = found;
        frameCount_ = frame.frameCount;
        releaseLock();
    }

    cv::Mat GrabDetectorPlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImage = currentImage_.clone();
        int signalMin = signalMin_;
        int signalMax = signalMax_;
        bool found = found_;
        int frameCount = frameCount_;
        releaseLock();

        cv::Rect boxRect = getDetectionBoxCv();
        cv::Scalar boxColor(0,0,255);
        int boxLineWidth = 3;

        std::stringstream minMaxStream;
        std::stringstream foundStream;
        minMaxStream << "frame: " << frameCount << ", min: " << signalMin << ", max: " << signalMax;
        if (found)
        {
            foundStream << "object found";
        }
        //std::cout << minMaxStream.str()  << ", " << foundStream.str() << std::endl;

        cv::Mat currentImageBGR;
        cv::cvtColor(currentImage, currentImageBGR, CV_GRAY2BGR);
        cv::rectangle(currentImageBGR, boxRect,boxColor, boxLineWidth);
        cv::putText(currentImageBGR, foundStream.str(), cv::Point(1050,30),CV_FONT_HERSHEY_SIMPLEX, 1.0, boxColor,2);
        cv::putText(currentImageBGR, minMaxStream.str(), cv::Point(8,30),CV_FONT_HERSHEY_SIMPLEX, 1.0, boxColor,2);

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
        triggerArmed_ = false;

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
        livePlotPtr -> xAxis -> setRange(0, 5);
        livePlotPtr -> yAxis -> setRange(0,255);
        livePlotPtr -> xAxis -> setLabel("time (sec)");
        livePlotPtr -> replot();

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
            qDebug() << box;
            setDetectionBox(box);
        }
    }

}
