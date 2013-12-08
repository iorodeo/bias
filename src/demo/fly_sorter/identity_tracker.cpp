#include "identity_tracker.hpp"
#include "hungarian.hpp"
#include <iostream>
#include <map>



// Identity Tracker Methods
// ----------------------------------------------------------------------------
IdentityTracker::IdentityTracker() 
{ 
    isFirst_ = true;
    idCounter_ = 0;
}


IdentityTracker::IdentityTracker(IdentityTrackerParam param) : IdentityTracker()
{
    setParam(param);
}


void IdentityTracker::setParam(IdentityTrackerParam param)
{
    param_ = param;
}


void IdentityTracker::update(BlobFinderData &blobFinderData)
{
    if (isFirst_)
    {
        // If is the first call just give each blob a unique id number
        BlobDataList::iterator it;
        for (it=blobFinderData.blobDataList.begin(); it!=blobFinderData.blobDataList.end(); it++)
        {
            (it -> id) = idCounter_;
            idCounter_++;
        }
        //isFirst_ = false;
    }
    else
    {
        // Use velocity model and greedy algorithm to try and associate new and old blobs
        std::vector<std::vector<int>> costMatrix = getCostMatrix(blobFinderData);
    }

    // Debug
    // ------------------------------------------------------------------------------------------------
    //BlobDataList::iterator it;
    //for (it=blobFinderDataPrev_.blobDataList.begin(); it!=blobFinderDataPrev_.blobDataList.end(); it++)
    //{
    //    BlobData blobData = *it;
    //    std::cout << "old: " << blobData.id << std::endl;
    //}
    //for (it=blobFinderData.blobDataList.begin(); it!=blobFinderData.blobDataList.end(); it++)
    //{
    //    BlobData blobData = *it;
    //    std::cout << "new: " << blobData.id << std::endl;

    //}
    //std::cout << std::endl;
    // ------------------------------------------------------------------------------------------------


    blobFinderDataPrev_ = blobFinderData;
}

std::vector<std::vector<int>> IdentityTracker::getCostMatrix(BlobFinderData &blobFinderData)
{
    std::vector<std::vector<int>> costMatrix;

    BlobDataList blobDataListPrev = blobFinderDataPrev_.blobDataList;
    BlobDataList blobDataListCurr = blobFinderData.blobDataList;

    int numPrev = getNumberOkItems(blobDataListPrev);
    int numCurr = getNumberOkItems(blobDataListCurr);

    if ((numPrev > 0) && (numCurr > 0))
    {
        costMatrix.resize(numPrev,std::vector<int>(numCurr,0));

        int indPrev;
        BlobDataList::iterator itPrev;
        for (
                indPrev = 0, itPrev = blobDataListPrev.begin(); 
                itPrev != blobDataListPrev.end(); 
                indPrev++, itPrev++
                )
        {
            BlobData blobDataPrev = *itPrev;

            int xPrev = blobDataPrev.boundingRect.x + 0.5*blobDataPrev.boundingRect.width;
            int yPrev = blobDataPrev.boundingRect.y + 0.5*blobDataPrev.boundingRect.height;
            int widthPrev = blobDataPrev.boundingRect.width;
            int heightPrev = blobDataPrev.boundingRect.height;

            int indCurr;
            BlobDataList::iterator itCurr;
            for (
                    indCurr = 0, itCurr = blobDataListCurr.begin(); 
                    itCurr != blobDataListCurr.end(); 
                    indCurr++, itCurr++
                    )
            {
                BlobData blobDataCurr = *itCurr;
                int xCurr = blobDataCurr.boundingRect.x + 0.5*blobDataCurr.boundingRect.width;
                int yCurr = blobDataCurr.boundingRect.y + 0.5*blobDataCurr.boundingRect.height;
                int widthCurr = blobDataCurr.boundingRect.width;
                int heightCurr = blobDataCurr.boundingRect.height;
                
            }
        }
    }

    return costMatrix;

}


int getNumberOkItems(BlobDataList blobDataList)
{
    int numOk = 0;
    BlobDataList::iterator it;
    for (it = blobDataList.begin(); it != blobDataList.end(); it++)
    {
        BlobData blobData = *it;
        if (!(blobData.onBorderX || blobData.onBorderY))
        {
            numOk++;
        }
    }
    return numOk;
}



