#ifndef IDENTITY_TRACKKER_HPP
#define IDENTITY_TRACKKER_HPP
#include "parameters.hpp"
#include "blob_data.hpp"
#include <list>

class IdentityData
{
    unsigned int id;
    BlobData blobData;
};

typedef std::list<IdentityData> IdentityDataList;


class IdentityTracker
{
    public:

        IdentityTracker();

    private:

        IdentityTrackerParam param_;
        IdentityDataList dataList_;

};

#endif // ifndef IDENTITY_TRACKER_HPP
