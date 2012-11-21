#ifndef BIAS_MAT_TO_QIMAGE_HPP
#define BIAS_MAT_TO_QIMAGE_HPP

#include <QtGui>
#include <QDebug>
#include <opencv2/core/core.hpp>

QImage matToQImage(const cv::Mat& mat);

#endif // #ifndef BIAS_MAT_TO_QIMAGE_HPP
