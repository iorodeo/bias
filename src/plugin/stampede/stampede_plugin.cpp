#include "stampede_plugin.hpp"

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    StampedePlugin::StampedePlugin(QWidget *parent) : BiasPlugin(parent)
    {
        setupUi(this);
        initialize();
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedePlugin::initialize()
    {
        vibrationEventWidgetPtr -> setNameText(QString("Vibration"));
        displayEventWidgetPtr -> setNameText(QString("Display"));

    }

}
