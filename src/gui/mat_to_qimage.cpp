#include "mat_to_qimage.hpp"

QImage matToQImage(const cv::Mat& mat)
{
    // ------------------------------------------------------------------------
    // TO DO ... can definitely make this more efficient
    // ------------------------------------------------------------------------
    if(mat.type()==CV_8UC1)
    {
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
        {
            colorTable.push_back(qRgb(i,i,i));
        }

        const uchar *qImageBuffer = (const uchar*) mat.data;
        QImage img = QImage(
                qImageBuffer, 
                mat.cols, 
                mat.rows, 
                mat.step, 
                QImage::Format_Indexed8
                );

        img.setColorTable(colorTable);
        return img;
    }

    if(mat.type()==CV_8UC3)
    {
        const uchar *qImageBuffer = (const uchar*)mat.data;
        QImage img = QImage(
                qImageBuffer, 
                mat.cols, 
                mat.rows, 
                mat.step, 
                QImage::Format_RGB888
                );
        return img.rgbSwapped();
    }
    else
    {
        // --------------------------------------------------------------------
        // TO DO ... need some error handling here
        // --------------------------------------------------------------------
        return QImage();
    }
}

