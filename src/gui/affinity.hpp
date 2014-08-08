#ifndef BIAS_AFFINITY_HPP
#define BIAS_AFFINITY_HPP
#include <QMutex>

namespace bias 
{


    class ThreadAffinityService
    {
        public:
            static void setNumberOfCameras(unsigned int numberOfCameras);
            static bool assignThreadAffinity(bool isImageGrabber, unsigned int cameraNumber);

        private:
            static unsigned int numberOfCameras_;
            static QMutex coutDebugMutex_;
    };

} // namespace bias

#endif // #ifndef BIAS_AFFINITY_HPP
