#ifndef VIBRATION_EVENT_LABEL_HPP
#define VIBRATION_EVENT_LABEL_HPP

#include "base_event_label.hpp"

namespace bias
{

    class VibrationEventLabel : public BaseEventLabel
    {

        Q_OBJECT

        public:

            VibrationEventLabel(QWidget *parent=0);

    };

}

#endif
