#ifndef GRAB_DETECTOR_PLUGIN_HPP
#define STAMPEPE_PLUGIN_HPP
#include "ui_grab_detector_plugin.h"
#include "bias_plugin.hpp"

namespace bias
{
    class GrabDetectorPlugin : public BiasPlugin, public Ui::GrabDetectorPluginDialog
    {
        Q_OBJECT

        public:

            GrabDetectorPlugin(QWidget *parent=0);

        private:
            int dummy;

    };

}

#endif
