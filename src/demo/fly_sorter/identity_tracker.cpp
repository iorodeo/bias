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

            int indCurr;
            BlobDataList::iterator itCurr;
            for (
                    indCurr = 0, itCurr = blobDataListCurr.begin(); 
                    itCurr != blobDataListCurr.end(); 
                    indCurr++, itCurr++
                    )
            {
                BlobData blobDataCurr = *itCurr;
                
            }
        }
    }

    return costMatrix;

}

int IdentityTracker::getCost(BlobData blob0, BlobData blob1)
{
    // Get center and size of blob0
    int x0 = blob0.boundingRect.x + blob0.boundingRect.width/2;
    int y0 = blob0.boundingRect.y + blob0.boundingRect.height/2;
    int width0 = blob0.boundingRect.width;
    int height0 = blob0.boundingRect.height;

    // Get center and size of blob1
    int x1 = blob1.boundingRect.x + blob1.boundingRect.width/2;
    int y1 = blob1.boundingRect.y + blob1.boundingRect.height/2;
    int width1 = blob1.boundingRect.width;
    int height1 = blob1.boundingRect.height;


}


int getNumberOkItems(BlobDataList blobDataList)
{
    // Returns number of items not on the border.
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



