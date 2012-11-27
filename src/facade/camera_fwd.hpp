#ifndef BIAS_CAMERA_FWD_HPP
#define BIAS_CAMERA_FWD_HPP

#include <memory>
#include <set>
#include <list>

namespace bias
{
    class Camera;

    typedef std::shared_ptr<Camera> CameraPtr;
    typedef std::set<CameraPtr> CameraPtrSet;
    typedef std::list<CameraPtr> CameraPtrList;
}


#endif // #ifndef BIAS_CAMERA_FRD_HPP
