#ifndef IMAGE_LABEL_HPP
#define IMAGE_LABEL_HPP

#include "camera_facade_fwd.hpp"
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

            void setFlipVert(bool flipVert);
            void setFlipHorz(bool flipHorz);
            void setImageRotation(ImageRotationType imageRot);
            bool getFlipVert();
            bool getFlipHorz();
            ImageRotationType getImageRotation();
            void setOrientation(bool flipVert, bool flipHorz, ImageRotationType imageRot);

        public slots:
            void setPixmap(const QPixmap &pixmap);
            void setPixmapWithFrameCount(const QPixmap &pixmap, unsigned long frameCount);

        signals:

            void selectBoxChanged(QRect box);

        protected:

            bool flipVert_;
            bool flipHorz_;
            ImageRotationType imageRot_;
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
