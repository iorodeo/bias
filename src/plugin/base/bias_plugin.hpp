#ifndef BIAS_PLUGIN_HPP
#define BIAS_PLUGIN_HPP
#include <QDialog>
#include <QWidget>

namespace bias
{
    class BiasPlugin : public QDialog 
    {
        Q_OBJECT

        public:

            BiasPlugin(QWidget *parent=0);
            static bool pluginsEnabled();
            static void setPluginsEnabled(bool value);

        protected:

            static bool pluginsEnabled_;

    };

}


#endif 


