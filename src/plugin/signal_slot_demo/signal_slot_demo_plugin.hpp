#ifndef SIGNAL_SLOT_DEMO_HPP
#define SIGNAL_SLOT_DEMO_HPP
#include "ui_signal_slot_demo_plugin.h"
#include "bias_plugin.hpp"
#include <QPointer>
#include <QVector>
#include <QList>

class QTimer;

namespace cv
{
    class Mat;
}


namespace bias
{

    class ImageLabel;



    class SignalSlotDemoPlugin : public BiasPlugin, public Ui::SignalSlotDemoPluginDialog
    {
        Q_OBJECT

        public:

            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;

            SignalSlotDemoPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr=0);
            virtual void reset();
            virtual void stop();

            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();

            virtual QString getName();
            virtual QString getDisplayName();

        signals:

        protected:

            QPointer<ImageLabel> imageLabelPtr_;

            void connectWidgets();
            void initialize();

        private slots:


    };
}
#endif
