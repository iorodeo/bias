#include "guid.hpp"
#include "camera_finder.hpp"
#include "exception.hpp"
#include "camera.hpp"
#include <iostream>
#include <sstream>

namespace bias {

    CameraFinder::CameraFinder() 
    {
        createQueryContext_fc2();
        createQueryContext_dc1394();
        update();
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
        std::cout << getGuidListAsString();
    }

    std::string CameraFinder::getGuidListAsString()
    {
        int count;
        std::stringstream ss;

        GuidSet::iterator it;
        for (it=guidSet_.begin(), count=0; it!=guidSet_.end(); it++, count++) 
        {
            Guid guid = *it;
            ss << "[" << count << "] " << guid << std::endl;
        }
        ss << std::endl;
        return ss.str();
    }

    unsigned int CameraFinder::numberOfCameras() 
    {
        return guidSet_.size();
    }

    Guid CameraFinder::getGuidByIndex(unsigned int index)
    {
        if (index >= guidSet_.size()) {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 guid by index";
            throw RuntimeError(ERROR_FC2_GET_GUID, ssError.str());
        }
        GuidSet::iterator it = guidSet_.begin();
        std::advance(it,index);
        return *it;
    }

    GuidSet CameraFinder::getGuidSet()
    {
        update();
        return guidSet_;
    }

    GuidList CameraFinder::getGuidList()
    {
        GuidList guidList;
        update();
        std::copy(guidSet_.begin(), guidSet_.end(), std::back_inserter(guidList)); 
        return guidList;
    }

    CameraPtrSet CameraFinder::createCameraPtrSet()
    {
        CameraPtrSet camPtrSet;
        for (GuidSet::iterator it=guidSet_.begin(); it!=guidSet_.end(); it++)
        {
            CameraPtr camPtr = std::make_shared<Camera>(*it);
            camPtrSet.insert(camPtr);
        }
        return camPtrSet;
    }

    CameraPtrList CameraFinder::createCameraPtrList()
    {
        CameraPtrList camPtrList;
        for (GuidSet::iterator it=guidSet_.begin(); it!=guidSet_.end(); it++)
        {
            CameraPtr camPtr = std::make_shared<Camera>(*it);
            camPtrList.push_back(camPtr);
        }
        return camPtrList;
    }

#ifdef WITH_FC2

    // FlyCapture2 specific features
    // ------------------------------------------------------------------------

    void CameraFinder::createQueryContext_fc2()
    {
        fc2Error error = fc2CreateContext(&queryContext_fc2_);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 context";
            throw RuntimeError(ERROR_FC2_CREATE_CONTEXT, ssError.str());
        }
    }

    void CameraFinder::destroyQueryContext_fc2()
    {
        fc2Error error = fc2DestroyContext(queryContext_fc2_);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to destroy FlyCapture2 context";
            throw RuntimeError(ERROR_FC2_DESTROY_CONTEXT, ssError.str());
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
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to enumerate FlyCapture2 cameras";
            throw RuntimeError(ERROR_FC2_ENUMERATE_CAMERAS, ssError.str());
        }

        // Get attached camera guids - add to guid set 
        for (int i=0; i<numCameras; i++) 
        {
            error = fc2GetCameraFromIndex(queryContext_fc2_, i, &guid_fc2);
            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to get FlyCapture2 camera guid from index";
                throw RuntimeError(ERROR_FC2_GET_CAMERA_FROM_INDEX, ssError.str());
            }
            else 
            {
                guidSet_.insert(Guid(guid_fc2));
            }
        }
    }

#else

    // Dummy methods for when FlyCapture2 is not included
    // ------------------------------------------------------------------------

    void CameraFinder::createQueryContext_fc2() {};
    void CameraFinder::destroyQueryContext_fc2() {};
    void CameraFinder::update_fc2() {};

#endif

#ifdef WITH_DC1394

    // Libdc1394 specific features
    // ------------------------------------------------------------------------

    void CameraFinder::createQueryContext_dc1394()
    {
        queryContext_dc1394_ = NULL;
        queryContext_dc1394_ = dc1394_new();
        if ( !queryContext_dc1394_ ) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error creating dc1394_t" << std::endl;
            throw RuntimeError(ERROR_DC1394_CREATE_CONTEXT, ssError.str());
        }
    }

    void CameraFinder::destroyQueryContext_dc1394() 
    {
        dc1394_free(queryContext_dc1394_);
        queryContext_dc1394_ = NULL;
    }

    void CameraFinder::update_dc1394()
    {
        dc1394error_t error;
        dc1394camera_list_t *cameraList;

        if ( !queryContext_dc1394_ ) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error updating dc1394 context is NULL" << std::endl;
            throw RuntimeError(ERROR_DC1394_NULL_POINTER, ssError.str());
        }
        
        // Get number of attached cameras
        error = dc1394_camera_enumerate(queryContext_dc1394_, &cameraList);
        if (error != DC1394_SUCCESS)  
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to enumerate dc1394 camears, error code ";
            ssError << error  << std::endl;
            throw RuntimeError(ERROR_DC1394_ENUMERATE_CAMERAS, ssError.str());
        }

        // Add attached camera guids to the guid set.
        for (int i=0; i<(cameraList->num); i++) 
        {
            guidSet_.insert(Guid( cameraList -> ids[i].guid));
        }
        dc1394_camera_free_list(cameraList);
    }

#else
    // Dummy methods for when libdc1394 is not included
    // ------------------------------------------------------------------------

    void CameraFinder::createQueryContext_dc1394() {};
    void CameraFinder::destroyQueryContext_dc1394() {};
    void CameraFinder::update_dc1394() {};

#endif

} // namespace bias
