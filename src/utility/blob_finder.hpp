#ifndef BLOB_FINDER_HPP
#define BLOB_FINDER_HPP
#include "blob_finder_param.hpp"
#include "blob_data.hpp"
#include <opencv2/core/core.hpp>


namespace bias
{

    class BlobFinderData
    {
        public:
            BlobDataList blobDataList;
            cv::Mat blobDataImage;
            cv::Mat thresholdImage;
            bool success;
            BlobFinderData();
    };

    class BlobFinder
    {
        public:

            BlobFinder();
            BlobFinder(BlobFinderParam param);
            BlobFinderData findBlobs(cv::Mat image);
            void setParam(BlobFinderParam param);

        private:

            BlobFinderParam param_;
            bool withinAreaBounds(BlobData blobData);
            bool containsStrictThreshold(BlobData blobData, cv::Mat image);

    };


} // namespace bias




#endif // #ifndef BLOB_FINDER_HPP


