#ifndef EMPTY_EVENT_LABEL_HPP
#define EMPTY_EVENT_LABEL_HPP

#include "base_event_label.hpp"

namespace bias
{

    class EmptyEventLabel : public BaseEventLabel 
    {

        Q_OBJECT

        public:

            EmptyEventLabel(QWidget *parent=0);

        protected:

            void initialize();

    };

}

#endif
