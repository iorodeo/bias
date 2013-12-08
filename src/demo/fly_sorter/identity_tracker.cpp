#include "identity_tracker.hpp"
#include "hungarian.hpp"
#include <iostream>
#include <map>
#include <cmath>


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
    if (!isFirst_)
    {
        assignBlobsGreedy(blobFinderData);
    }
    else
    {
        isFirst_ = false;
    }

    // Assign id to any un-assigned blobs
    BlobDataList::iterator it;
    for (it=blobFinderData.blobDataList.begin(); it!=blobFinderData.blobDataList.end(); it++)
    {
        if ( (!(it -> isOnBorder())) && ((it -> id) == BlobData::ID_NOT_ASSIGNED))
        { 
            (it -> id) = idCounter_;
            idCounter_++;
        }
    }

    // Debug
    // ------------------------------------------------------------------------------------------------
    //for (it=blobFinderDataPrev_.blobDataList.begin(); it!=blobFinderDataPrev_.blobDataList.end(); it++)
    //{
    //    BlobData blobData = *it;
    //    if (blobData.id != BlobData::ID_NOT_ASSIGNED)
    //    { 
    //        std::cout << "old: " << blobData.id << std::endl;
    //    }
    //}
    //for (it=blobFinderData.blobDataList.begin(); it!=blobFinderData.blobDataList.end(); it++)
    //{
    //    BlobData blobData = *it;
    //    if (blobData.id != BlobData::ID_NOT_ASSIGNED)
    //    {
    //        std::cout << "new: " << blobData.id << std::endl;
    //    }
    //}
    //std::cout << std::endl;
    // ------------------------------------------------------------------------------------------------

    blobFinderDataPrev_ = blobFinderData;
}


void IdentityTracker::assignBlobsGreedy(BlobFinderData &blobFinderData)
{ 
    std::vector<std::vector<float>> costMatrix = getCostMatrix(blobFinderData);
    if (costMatrix.size() == 0)
    {
        return;
    }

    std::map<int,BlobDataList::iterator> indexToPrevPtrMap = getIndexToBlobDataPtrMap(
            blobFinderDataPrev_
            );

    std::map<int,BlobDataList::iterator> indexToCurrPtrMap = getIndexToBlobDataPtrMap(
            blobFinderData
            );

    int numCurr = indexToCurrPtrMap.size();
    int numPrev = indexToPrevPtrMap.size();

    // DEBUG
    // ---------------------------------------------------------------------------------
    //std::cout << "numPrev: " << numPrev << std::endl;
    //std::cout << "numCurr: " << numCurr << std::endl;
    // ---------------------------------------------------------------------------------

    std::vector<bool> usedVector = std::vector<bool>(numPrev, false);
    
    for (int indCurr=0; indCurr<numCurr; indCurr++)
    {
        float minCost = 2.0*param_.maxCost;  // Assign to someting larger than maxCost
        int indPrevMin = -1;                 // -1 means not found
        
        for (int indPrev=0; indPrev<numPrev;indPrev++)
        {
            float cost = costMatrix[indCurr][indPrev];
            if ((cost < minCost) & (usedVector[indPrev] != true))
            {
                minCost = cost;
                indPrevMin= indPrev;
            }
        }

        if (indPrevMin != -1)
        {
            (indexToCurrPtrMap[indCurr] -> id) = (indexToPrevPtrMap[indPrevMin] -> id);
            usedVector[indPrevMin] = true;

            // DEBUG
            // --------------------------------------------------------------------------
            //std::cout << "assigning " << indCurr << " from " << indPrevMin << std::endl;  
            //std::cout << "Prev id = " << (indexToPrevPtrMap[indPrevMin] -> id) << std::endl;
            //std::cout << "Curr id = " << (indexToCurrPtrMap[indCurr] -> id) << std::endl;
            // --------------------------------------------------------------------------
        }
    }
}


std::vector<std::vector<float>> IdentityTracker::getCostMatrix(BlobFinderData &blobFinderData)
{
    std::vector<std::vector<float>> costMatrix;

    BlobDataList blobDataListPrev = blobFinderDataPrev_.blobDataList;
    BlobDataList blobDataListCurr = blobFinderData.blobDataList;

    int numCurr = getNumberOkItems(blobDataListCurr);
    int numPrev = getNumberOkItems(blobDataListPrev);

    // DEBUG
    // ----------------------------------------------------------------------------------
    //std::cout << "numCurr: " << numCurr << std::endl;
    //std::cout << "numPrev: " << numPrev << std::endl;
    // ----------------------------------------------------------------------------------

    if ((numPrev > 0) && (numCurr > 0))
    {
        costMatrix.resize(numCurr,std::vector<float>(numPrev,0.0));

        int indCurr = 0;
        BlobDataList::iterator itCurr;

        for (itCurr=blobDataListCurr.begin(); itCurr!=blobDataListCurr.end(); itCurr++)
        {
            BlobData blobDataCurr = *itCurr;
            if (blobDataCurr.isOnBorder()) 
            { 
                continue; 
            }

            int indPrev = 0;
            BlobDataList::iterator itPrev;

            for (itPrev=blobDataListPrev.begin(); itPrev!=blobDataListPrev.end(); itPrev++)
            {
                BlobData blobDataPrev = *itPrev;
                if (blobDataPrev.isOnBorder()) 
                { 
                    continue; 
                }

                float cost = getCost(blobDataCurr, blobDataPrev);
                costMatrix[indCurr][indPrev] = cost;

                indPrev++;
                
            } // for (itPrev

            indCurr++;

        } // for (itCurr

    }

    return costMatrix;
}

float IdentityTracker::getCost(BlobData blobCurr, BlobData blobPrev)
{

    // Get center and size of blobCurr
    float xCurr = float(blobCurr.boundingRect.x) + 0.5*float(blobCurr.boundingRect.width);
    float yCurr = float(blobCurr.boundingRect.y) + 0.5*float(blobCurr.boundingRect.height);
    float widthCurr = float(blobCurr.boundingRect.width);
    float heightCurr = float(blobCurr.boundingRect.height);

    // Get center and size of blobPrev
    float xPrev = float(blobPrev.boundingRect.x)+ 0.5*float(blobPrev.boundingRect.width);
    float yPrev = float(blobPrev.boundingRect.y) + 0.5*float(blobPrev.boundingRect.height);
    float widthPrev = float(blobPrev.boundingRect.width);
    float heightPrev = float(blobPrev.boundingRect.height);

    // Compute cost 
    float costX = std::abs((xCurr - xPrev - param_.meanDx)/param_.stdDx);
    float costY = std::abs((yCurr - yPrev - param_.meanDy)/param_.stdDy);
    float costWidth = std::abs((widthCurr - widthPrev - param_.meanWidth)/param_.stdWidth);
    float costHeight = std::abs((heightCurr - heightPrev - param_.meanHeight)/param_.stdHeight);
    float costTotal = costX + costY + costWidth + costHeight;

    return costTotal;
}


std::map<int,BlobDataList::iterator> IdentityTracker::getIndexToBlobDataPtrMap(
        BlobFinderData &blobFinderData
        )
{
    std::map<int,BlobDataList::iterator> map;

    BlobDataList::iterator it;
    int index = 0;

    for (it=blobFinderData.blobDataList.begin(); it!=blobFinderData.blobDataList.end(); it++)
    {
        if ( (it->isOnBorder()) )
        {
            continue;
        }
        //BlobData *blobDataPtr = &(*it);
        //map[index] = blobDataPtr;
        map[index] = it;
        index++;
    }
    return map;
}

// Utility functions
// ----------------------------------------------------------------------------


int getNumberOkItems(BlobDataList blobDataList)
{
    // Returns number of items not on the border.
    int numOk = 0;
    BlobDataList::iterator it;
    for (it = blobDataList.begin(); it != blobDataList.end(); it++)
    {
        BlobData blobData = *it;
        if (!blobData.isOnBorder())
        {
            numOk++;
        }
    }
    return numOk;
}


void printMatrix(std::vector<std::vector<float>> matrix)
{
    if (matrix.size() == 0)
    {
        return;
    }

    for (int i=0; i<matrix.size(); i++)
    {
        std::vector<float> row = matrix[i];

        for (int j=0; j<row.size(); j++)
        {
            std::cout << row[j] << std::endl;

        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}





