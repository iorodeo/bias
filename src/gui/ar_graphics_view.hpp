#ifndef BIAS_AR_GRAPHICS_VIEW_HPP
#define BIAS_AR_GRAPHICS_VIEW_HPP
#include <QGraphicsView>

namespace bias
{
    class ARGraphicsView : public QGraphicsView
    {
        public:

            ARGraphicsView(QWidget *parent=0);
            virtual int heightForWidth(int w) const;
    };
} // namepsace bias

#endif // #ifndef BIAS_AR_GRAPHICS_VIEW_HPP
