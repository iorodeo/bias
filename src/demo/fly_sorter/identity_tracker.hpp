#ifndef IDENTITY_TRACKKER_HPP
#define IDENTITY_TRACKKER_HPP
#include "parameters.hpp"
#include "blob_finder.hpp"
#include <vector>

class IdentityTracker
{
    public:

        IdentityTracker();
        IdentityTracker(IdentityTrackerParam param);
        void setParam(IdentityTrackerParam param);
        void update(BlobFinderData &blodFinderData);


    private:

        bool isFirst_;
        long idCounter_;
        IdentityTrackerParam param_;
        BlobFinderData blobFinderDataPrev_;

        std::vector<std::vector<int>> getCostMatrix(BlobFinderData &blobFinderData);

};

int getNumberOkItems(BlobDataList blobDataList);


#endif // ifndef IDENTITY_TRACKER_HPP
