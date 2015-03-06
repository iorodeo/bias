#ifndef IMAGE_LABEL_HPP
#define IMAGE_LABEL_HPP

#include <QLabel>

class QMouseEvent;
class QRect;

namespace bias
{

    class ImageLabel : public QLabel
    {
        Q_OBJECT

        public:

            ImageLabel(QWidget *parent=0);
            void setScaleFactor(float scale);
            float getScaleFactor();
            QRect getPixmapRect();

        signals:

            void selectBoxChanged(QRect box);

        protected:

            float scaleFactor_;
            bool boxInProgress_;
            QPoint boxBegin_;
            QPoint boxEnd_;

            QRect getSelectBoxRect();
            QPoint convertToPixmapCoord(QPoint posInLabel, int &ok);
            QPoint convertToUnscaledCoord(QPoint posInLabel, int &ok);

            void mousePressEvent(QMouseEvent *event);
            void mouseReleaseEvent(QMouseEvent *event);
            void mouseMoveEvent(QMouseEvent *event);
    };
}

#endif
