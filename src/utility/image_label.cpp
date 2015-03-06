#include "image_label.hpp"
#include <QtDebug>
#include <QMouseEvent>
#include <QPointer>
#include <QPixmap>
#include <QApplication>
#include <QStyle>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------

    ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
    {
        boxInProgress_ = false;
        boxBegin_ = QPoint(0,0);
        boxEnd_ = QPoint(0,0);
    }


    void ImageLabel::setScaleFactor(float scaleFactor)
    {
        scaleFactor_ = scaleFactor;
    }


    float ImageLabel::getScaleFactor()
    {
        return scaleFactor_;
    }


    QRect ImageLabel::getPixmapRect()
    {
        QRect pixmapRect;
        const QPixmap *pixmapPtr = pixmap();
        if (pixmapPtr != 0)
        {
            QRect labelRect =  contentsRect();
            QSize pixmapSize = pixmapPtr -> size();
            QRect alignedRect = QStyle::alignedRect(
                    QApplication::layoutDirection(), 
                    QFlag(alignment()), 
                    pixmapSize,
                    labelRect
                    );
            pixmapRect = alignedRect.intersected(labelRect);
        }
        return pixmapRect;
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    
    QPoint ImageLabel::convertToPixmapCoord(QPoint posInLabel, int &ok)
    {
        ok = false;
        QPoint posInPixmap;

        const QPixmap *pixmapPtr = pixmap();
        if (pixmapPtr != 0)
        {
            QRect pixmapRect = getPixmapRect();
            int x = posInLabel.x() - pixmapRect.x();
            int y = posInLabel.y() - pixmapRect.y();
            posInPixmap.setX(x);
            posInPixmap.setY(y);
            if ( (x >= 0) && (x < pixmapRect.width()) && (y >= 0) && (y < pixmapRect.height()))
            { 
                ok = true;
            }
        }
        return posInPixmap;
    }


    QPoint ImageLabel::convertToUnscaledCoord(QPoint posInLabel, int &ok)
    {
        QPoint posInUnscaled;
        QPoint posInPixmap = convertToPixmapCoord(posInLabel,ok);
        if (ok)
        {
            posInUnscaled.setX(posInPixmap.x()/scaleFactor_);
            posInUnscaled.setY(posInPixmap.y()/scaleFactor_);
        }
        return posInUnscaled;
    }


    QRect ImageLabel::getSelectBoxRect()
    {
        int xMin = qMin(boxBegin_.x(), boxEnd_.x());
        int xMax = qMax(boxBegin_.x(), boxEnd_.x());
        int yMin = qMin(boxBegin_.y(), boxEnd_.y());
        int yMax = qMax(boxBegin_.y(), boxEnd_.y());
        return QRect(xMin, yMin, xMax-xMin, yMax-yMin);
    }


    void ImageLabel::mousePressEvent(QMouseEvent *event)
    {
        Qt::MouseButton button = event -> button();
        if (button == Qt::LeftButton)
        {
            int ok;
            QPoint posInLabel = event -> pos();
            QPoint posUnscaled =  convertToUnscaledCoord(posInLabel, ok);
            if (ok)
            {
                boxInProgress_ = true;
                boxBegin_ = posUnscaled;
            }
        }
    }


    void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
    {
        Qt::MouseButton button = event -> button();
        if (button == Qt::LeftButton)
        {
            if (boxInProgress_)
            {
                int ok;
                QPoint posInLabel = event -> pos();
                QPoint posUnscaled =  convertToUnscaledCoord(posInLabel, ok);
                if (ok)
                {
                    boxEnd_ = posUnscaled;
                    QRect boxRect = getSelectBoxRect();
                    emit selectBoxChanged(boxRect);
                }
            }
            boxInProgress_ = false;
        }
    }


    void ImageLabel::mouseMoveEvent(QMouseEvent *event)
    {
        if (boxInProgress_)
        {
            int ok;
            QPoint posInLabel = event -> pos();
            QPoint posUnscaled =  convertToUnscaledCoord(posInLabel, ok);
            if (ok)
            {
                boxEnd_ = posUnscaled;
                QRect boxRect = getSelectBoxRect();
                emit selectBoxChanged(boxRect);
            }
        }
    }

}
