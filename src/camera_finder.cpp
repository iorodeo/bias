#include "camera_finder.hpp"
#include <iostream>
#include <sstream>

namespace bias {

    CameraFinder::CameraFinder() 
    {
        createQueryContext_fc2();
        createQueryContext_dc1394();
    };

    CameraFinder::~CameraFinder() 
    {
        destroyQueryContext_fc2();
        destroyQueryContext_dc1394();
    };

    void CameraFinder::update() 
    {
        guidPtrSet_.clear();
        update_fc2();
        update_dc1394();
    }

    void CameraFinder::printGuid() 
    {
        std::cout << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << " CameraFinder Guid List" << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << std::endl;
        std::cout << getGuidListAsString();
    }

    std::string CameraFinder::getGuidListAsString()
    {
        int count;
        std::stringstream ss;
        GuidPtrSet::iterator it;

        for (it=guidPtrSet_.begin(), count=0; it!=guidPtrSet_.end(); it++, count++) 
        {
            ss << "[" << count << "] " << **it << std::endl;
        }
        ss << std::endl;
        return ss.str();
    }

    unsigned int CameraFinder::numberOfCameras() 
    {
        return guidPtrSet_.size();
    }

    Guid CameraFinder::getGuidByIndex(unsigned int index)
    {
        if (index >= guidPtrSet_.size()) {
            // TO DO ... throw some kind of error
            std::cout << "guid index out of range" << std::endl;
            return Guid();
        }
        GuidPtrSet::iterator it = guidPtrSet_.begin();
        std::advance(it,index);
        return **it;
    }

    GuidPtrSet CameraFinder::getGuidPtrSet()
    {
        return guidPtrSet_;
    }

    GuidPtrList CameraFinder::getGuidPtrList()
    {
        GuidPtrList guidPtrList;
        std::copy(
                guidPtrSet_.begin(), 
                guidPtrSet_.end(), 
                std::back_inserter(guidPtrList)
                ); 
        return guidPtrList;
        
    }


#ifdef WITH_FC2

    // FlyCapture2 specific features
    // ------------------------------------------------------------------------
    void CameraFinder::createQueryContext_fc2()
    {
        fc2Error error = fc2CreateContext(&queryContext_fc2_);
        if (error != FC2_ERROR_OK) 
        {
            // TO DO ... throw some kind of error
            std::cout << "Error creating fc2 query context" << std::endl;
        }
    }

    void CameraFinder::destroyQueryContext_fc2()
    {
        fc2Error error = fc2DestroyContext(queryContext_fc2_);
        if (error != FC2_ERROR_OK) 
        {
            // TO DO ... throw some kind of error
            std::cout << "Error destroying fc2 query context" << std::endl;
        }
    }

    void CameraFinder::update_fc2()
    {
        fc2Error error;
        fc2PGRGuid guid_fc2;
        unsigned int numCameras = 0;

        // Get number of attached cameras.
        error = fc2GetNumOfCameras(queryContext_fc2_, &numCameras);
        if (error != FC2_ERROR_OK) 
        {
            // TO DO ... throw some kind of error
            std::cout << "Error getting number of fc2 cameras" << std::endl;
            return;
        }

        // Get attached camera guids - add to guid set 
        for (int i=0; i<numCameras; i++) 
        {
            error = fc2GetCameraFromIndex(queryContext_fc2_, i, &guid_fc2);
            if (error != FC2_ERROR_OK) 
            {
                // TO DO ... throw some kind of error (maybe ... or maybe ignore?)
                std::cout << "Error getting fc2 camera guid from index" << std::endl;
            }
            else 
            {
                GuidPtr guidPtr(new Guid(guid_fc2));
                guidPtrSet_.insert(guidPtr);
            }
        }
    }

#else

    // Dummy methods for when FlyCapture2 is not included
    // ------------------------------------------------------------------------

    void CameraFinder::createQueryContecxt_fc2() {};
    void CameraFinder::destroyQueryContext_dc1394 {};
    void CamrerFinder::update_fc2() {};

#endif

#ifdef WITH_DC1394

    // Libdc1394 specific features
    // ------------------------------------------------------------------------
    void CameraFinder::createQueryContext_dc1394()
    {
    }

    void CameraFinder::destroyQueryConext_dc1394() 
    {
    }

    void CameraFinder::update_dc1394()
    {
    }


#else
    // Dummy methods for when libdc1394 is not included
    // ------------------------------------------------------------------------

    void CameraFinder::createQueryContext_dc1394() {};
    void CameraFinder::destroyQueryContext_dc1394() {};
    void CameraFinder::update_dc1394() {};

#endif

} // namespace bias
