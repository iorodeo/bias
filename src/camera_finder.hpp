#ifndef CAMERA_FINDER_HPP
#define CAMERA_FINDER_HPP

//#include <list>
#include <set>
#include "guid.hpp"

#ifdef WITH_FC2
#include "C/FlyCapture2_C.h"
#endif

namespace bias {

    class CameraFinder
    {
        public:
            CameraFinder();
            ~CameraFinder();
            void update();
            void printGuid();

        private:
            //std::set<std::shared_ptr<Guid>> guidSet_;
            std::set<std::shared_ptr<Guid>,GuidPtrCmp> guidSet_;
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
