#ifndef STAMPEDE_PLUGIN_HPP
#define STAMPEDE_PLUGIN_HPP
#include "ui_stampede_plugin.h"
#include "bias_plugin.hpp"

namespace cv
{ 
    class Mat;
}

namespace bias
{
    class StampedePlugin : public BiasPlugin, public Ui::StampedePluginDialog
    {
        Q_OBJECT

        public:
            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;

            StampedePlugin(QWidget *parent=0);
            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();

            virtual QString getName();
            virtual QString getDisplayName();

            virtual RtnStatus runCmdFromMap(QVariantMap cmdMap,bool showErrorDlg=true);

        protected:
            void initialize();

        private slots:

    };

}


#endif
