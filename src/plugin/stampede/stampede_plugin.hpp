#ifndef STAMPEDE_PLUGIN_HPP
#define STAMPEDE_PLUGIN_HPP
#include "ui_stampede_plugin.h"
#include "bias_plugin.hpp"

namespace bias
{
    class StampedePlugin : public BiasPlugin, public Ui::StampedePluginDialog
    {
        Q_OBJECT

        public:

            StampedePlugin(QWidget *parent=0);

        protected:
            void initialize();

    };

}


#endif
