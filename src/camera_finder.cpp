#include <iostream>
#include "camera_finder.hpp"


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
        guidSet_.clear();
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

        int count;
        std::set<std::shared_ptr<Guid>>::iterator it;
        for (it=guidSet_.begin(), count=0; it!=guidSet_.end(); it++, count++) 
        {
            std::cout << "[" << count << "] " << **it << std::endl;
        }
        std::cout << std::endl;
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
                std::shared_ptr<Guid> guidPtr(new Guid(guid_fc2));
                guidSet_.insert(guidPtr);
            }
        }

        printGuid();

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


}
