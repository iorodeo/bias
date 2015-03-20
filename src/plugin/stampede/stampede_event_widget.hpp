#ifndef STAMPEDE_EVENT_WIDGET_HPP
#define STAMPEDE_EVENT_WIDGET_HPP

#include "ui_stampede_event_widget.h"
#include <QPointer>
#include <QHBoxLayout>
#include <QSplitter>

namespace bias
{

    class StampedeEventWidget : public QWidget, public Ui::StampedeEventForm
    {
        Q_OBJECT

        public:

            static const double DEFAULT_DURATION; 
            static int tmp_count;

            StampedeEventWidget(QWidget *parentPtr=0);
            void setNameText(QString name);
            QString getNameText();
            void setToolTipMsg(QString msg);
            
         signals:
            void timelineMoved(double timePos, int index);

        protected:

            QPointer<QHBoxLayout> timelineLayoutPtr_;
            QPointer<QSplitter> timelineSplitterPtr_;
            double duration_;
            QColor color_;

            void initialize();

        private slots:
            void timelineSplitterMoved(int pos, int index);


    };

}


#endif
