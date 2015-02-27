#include "grab_detector_plugin.hpp"
#include <QtDebug>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    GrabDetectorPlugin::GrabDetectorPlugin(QWidget *parent) : BiasPlugin(parent)
    {
        setupUi(this);
    }

}
