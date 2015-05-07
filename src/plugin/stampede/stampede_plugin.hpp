#ifndef STAMPEDE_PLUGIN_HPP
#define STAMPEDE_PLUGIN_HPP
#include "ui_stampede_plugin.h"
#include "stampede_plugin_config.hpp"
#include "bias_plugin.hpp"
#include "rtn_status.hpp"

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
            virtual void setActive(bool value);
            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();
            virtual QString getName();
            virtual QString getDisplayName();
            virtual RtnStatus runCmdFromMap(QVariantMap cmdMap,bool showErrorDlg=true);


        protected:

            StampedePluginConfig config_;

            void initialize();
            void connectWidgets();
            void updateConfigEditText();

        private slots:
            void onTimerDurationChanged(unsigned long duration);


    };

}


#endif
