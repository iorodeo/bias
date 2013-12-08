#ifndef IDENTITY_TRACKKER_HPP
#define IDENTITY_TRACKKER_HPP
#include "parameters.hpp"
#include "blob_finder.hpp"
#include <vector>
#include <memory>
#include <map>

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

        void assignBlobsGreedy(BlobFinderData &blobfinderData);
        std::vector<std::vector<float>> getCostMatrix(BlobFinderData &blobFinderData);
        float getCost(BlobData blobCurr, BlobData blobPrev);

        std::map<int,BlobDataList::iterator> getIndexToBlobDataPtrMap(
                BlobFinderData &blobFinderData
                );
};

int getNumberOkItems(BlobDataList blobDataList);
void printMatrix(std::vector<std::vector<float>> matrix);

#endif // ifndef IDENTITY_TRACKER_HPP
