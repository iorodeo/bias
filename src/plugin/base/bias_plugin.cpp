#include "bias_plugin.hpp"
#include <QtDebug>

namespace bias
{

    // Pulbic
    // ------------------------------------------------------------------------

    BiasPlugin::BiasPlugin(QWidget *parent) : QDialog(parent) { }


    // Protected
    // ------------------------------------------------------------------------

    bool BiasPlugin::pluginsEnabled_ = false;

    bool BiasPlugin::pluginsEnabled()
    {
        return pluginsEnabled_;
    }

    void BiasPlugin::setPluginsEnabled(bool value)
    {
        pluginsEnabled_ = value;
    }


}
