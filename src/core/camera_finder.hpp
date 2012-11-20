#ifndef BIAS_CAMERA_FINDER_HPP
#define BIAS_CAMERA_FINDER_HPP

#include <set>
#include <string>
#include "basic_types.hpp"
#include "guid.hpp"
#include "camera.hpp"

#ifdef WITH_FC2
#include "FlyCapture2_C.h"
#endif
#ifdef WITH_DC1394
#include <dc1394/dc1394.h> 
#endif

namespace bias {

    class CameraFinder
    {
        public:
            CameraFinder();
            ~CameraFinder();

            unsigned int numberOfCameras();
            Guid getGuidByIndex(unsigned int index);

            GuidSet getGuidSet();
            GuidList getGuidList();

            CameraPtrSet createCameraPtrSet();
            CameraPtrList createCameraPtrList();

            std::string getGuidListAsString();
            void printGuid();

            // old version
            // ---------------------------
            //GuidPtrSet getGuidPtrSet();
            //GuidPtrList getGuidPtrList();
            // ---------------------------


        private:
            GuidSet guidSet_;

            // old version
            // ---------------------------
            //GuidPtrSet guidPtrSet_;
            // ----------------------------
            
            void createQueryContext_fc2();
            void destroyQueryContext_fc2();
            void createQueryContext_dc1394();
            void destroyQueryContext_dc1394();
            void update();
            void update_fc2();
            void update_dc1394();

#ifdef WITH_FC2
        private:
            fc2Context queryContext_fc2_;
#endif
#ifdef WITH_DC1394
        private:
            dc1394_t *queryContext_dc1394_;
#endif
    };

} // namespace bias

#endif // #ifndef BIAS_CAMERA_FINDER_HPP
