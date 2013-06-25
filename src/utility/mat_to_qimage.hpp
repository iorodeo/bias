#ifndef BIAS_MAT_TO_QIMAGE_HPP
#define BIAS_MAT_TO_QIMAGE_HPP
#include <QImage>
#include <QVector>

namespace cv { class Mat; }

namespace bias
{
    QImage matToQImage(const cv::Mat& mat);
    QVector<QRgb> createColorTable();
}

#endif // #ifndef BIAS_MAT_TO_QIMAGE_HPP
