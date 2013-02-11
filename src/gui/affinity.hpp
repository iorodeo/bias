#ifndef BIAS_AFFINITY_HPP
#define BIAS_AFFINITY_HPP

namespace bias 
{
    bool assignThreadAffinity(
            bool isImageGrabber, 
            unsigned int numberOfCameras, 
            unsigned int cameraNumber=0 
            );

} // namespace bias

#endif // #ifndef BIAS_AFFINITY_HPP
