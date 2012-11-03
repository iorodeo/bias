#ifndef CAMERA_FINDER_HPP
#define CAMERA_FINDER_HPP

#include <set>
#include <string>
#include "basic_types.hpp"
#include "guid.hpp"

#ifdef WITH_FC2
#include "C/FlyCapture2_C.h"
#endif
#ifdef With_DC1394
// include dc1394 headers
#endif

namespace bias {

    class CameraFinder
    {
        public:
            CameraFinder();
            ~CameraFinder();
            void update();
            void printGuid();
            std::string getGuidListAsString();

            unsigned int numberOfCameras();
            Guid getGuidByIndex(unsigned int index);
            GuidPtrSet getGuidPtrSet();
            GuidPtrList getGuidPtrList();

        private:
            GuidPtrSet guidPtrSet_;
            void createQueryContext_fc2();
            void createQueryContext_dc1394();
            void destroyQueryContext_fc2();
            void destroyQueryContext_dc1394();
            void update_fc2();
            void update_dc1394();

#ifdef WITH_FC2
        private:
            fc2Context queryContext_fc2_;
#endif
#ifdef WITH_DC1394
        private:
            dc1394 queryContext_dc1394_;
#endif
    };

}

#endif // #ifndef CAMERA_FINDER_HPP
