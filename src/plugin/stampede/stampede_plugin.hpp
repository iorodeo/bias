#ifndef STAMPEDE_PLUGIN_HPP
#define STAMPEPE_PLUGIN_HPP
#include "ui_stampede_plugin.h"

namespace bias
{
    class StampedePlugin : public QDialog, public Ui::StampedePluginDialog
    {
        Q_OBJECT

        public:

            StampedePlugin(QWidget *parent=0);

        private:
            int dummy;

    };

}


#endif
