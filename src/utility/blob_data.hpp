#ifndef BLOB_DATA_HPP
#define BLOB_DATA_HPP

#include <vector>
#include <list>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace bias
{

    class Centroid
    {
        public:
            double x;
            double y;

            Centroid();
            Centroid(cv::Moments moments);
            void setFromMoments(cv::Moments moments);
            std::string toStdString(unsigned int indent=0);
            void print(unsigned int indent=0);
    };

    class Ellipse
    {
        public:
            double centerX;
            double centerY;
            double semiMajor;
            double semiMinor;
            double angle;

            Ellipse();
            Ellipse(std::vector<cv::Point> contour);
            void setFromContour(std::vector<cv::Point> contour);
            std::string toStdString(unsigned int indent=0);
            void print(unsigned int indent=0);
            void draw(cv::Mat &img);
    };

    class BlobData
    {
        public:
            long id;
            bool old;
            bool onBorderX;
            bool onBorderY;
            double area;
            Centroid centroid;
            Ellipse ellipse;
            cv::Rect boundingRect;
            cv::Mat boundingImage;
            std::vector<cv::Point> contourVector;

            static const long ID_NOT_ASSIGNED;

            BlobData();
            BlobData(
                    std::vector<cv::Point> contour, 
                    cv::Mat image, 
                    unsigned int numPad
                    );

            void setFromContour(
                    std::vector<cv::Point> contour, 
                    cv::Mat image, 
                    unsigned int numPad
                    );

            std::string toStdString( unsigned int indent=0);
            void print(unsigned int indent=0);
            void draw(cv::Mat &img);
            bool isOnBorder();

    };


    typedef std::list<BlobData> BlobDataList;

    std::string getIndentString(unsigned int indent, unsigned int indentStep=2);
    int getNumBlobsExcludeXBorder(BlobDataList blobDataList);
    int getNumBlobsExcludeYBorder(BlobDataList blobDataList);


} // namespace bias


#endif // #ifndef BLOB_DATA_HPP


