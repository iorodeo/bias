#include "grab_detector_plugin.hpp"
#include <QtDebug>

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
    
    GrabDetectorPlugin::GrabDetectorPlugin(QWidget *parent) : BiasPlugin(parent)
    {
        setupUi(this);
        connectWidgets();
        initialize();
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
                xPosSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(xPosSliderValueChanged(int))
               );

        connect(
                yPosSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(yPosSliderValueChanged(int))
               );

        connect(
                widthSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(widthSliderValueChanged(int))
               );

        connect(
                heightSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(heightSliderValueChanged(int))
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

        // Set detection box to default values
        xPosSliderPtr -> setValue(DEFAULT_XPOS);
        yPosSliderPtr -> setValue(DEFAULT_YPOS);
        widthSliderPtr -> setValue(DEFAULT_WIDTH);
        heightSliderPtr -> setValue(DEFAULT_HEIGHT);

        // Set trigger threshold and filter size (TEMPORARY)
        trigThresholdSpinBoxPtr -> setValue(DEFAULT_TRIGGER_THRESHOLD);
        trigMedianFilterSpinBoxPtr ->  setValue(DEFAULT_TRIGGER_FILTER_SIZE);

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


    void GrabDetectorPlugin::xPosSliderValueChanged(int value)
    {
        qDebug() << "xPos value changed: " << value;
    }


    void GrabDetectorPlugin::yPosSliderValueChanged(int value)
    {
        qDebug() << "yPos value changed: " << value;
    }


    void GrabDetectorPlugin::widthSliderValueChanged(int value)
    {
        qDebug() << "width value changed: " << value;
    }


    void GrabDetectorPlugin::heightSliderValueChanged(int value)
    {
        qDebug() << "height value changed: " << value;
    }


    void GrabDetectorPlugin::colorSelectPushButtonClicked()
    {
        qDebug() << "color select push buttton clicked";
    }


    void GrabDetectorPlugin::trigResetPushButtonClicked()
    {
        qDebug() << "trigger reset push button clicked";
    }

}
