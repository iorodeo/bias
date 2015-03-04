#ifndef DISPLAY_EVENT_LABEL_HPP
#define DISPLAY_EVENT_LABEL_HPP

#include "base_event_label.hpp"

namespace bias
{

    class DisplayEventLabel : public BaseEventLabel 
    {

        Q_OBJECT

        public:

            DisplayEventLabel(QWidget *parent=0);

    };

}

#endif
