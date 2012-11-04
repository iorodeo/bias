#ifndef BIAS_BASIC_TYPES_HPP
#define BIAS_BASIC_TYPES_HPP

namespace bias {

    enum CameraLib 
    {
        CAMERA_LIB_FC2,
        CAMERA_LIB_DC1394,
        CAMERA_LIB_UNDEFINED
    };

    enum ErrorCode {
        ERROR_CAMERA_LIB_UNKNOWN=1,
        ERROR_CAMERA_LIB_UNDEFINED,

        // FlyCapture2 specific errors
        ERROR_NO_FC2,
        ERROR_FC2_CREATE_CONTEXT,
        ERROR_FC2_DESTROY_CONTEXT,
        ERROR_FC2_CONNECT,
        ERROR_FC2_DISCONNECT,
        ERROR_FC2_START_CAPTURE,
        ERROR_FC2_STOP_CAPTURE,
        ERROR_FC2_RETRIEVE_BUFFER,
        ERROR_FC2_CREATE_IMAGE,
        ERROR_FC2_DESTROY_IMAGE,
        ERROR_FC2_GET_GUID,
        ERROR_FC2_GET_NUMBER_OF_CAMERAS,
        ERROR_FC2_GET_CAMERA_FROM_INDEX,

        // Libdc1394 specific errors
        ERROR_NO_DC1394,
        ERROR_DC1394_CREATE_CONTEXT,
        ERROR_DC1394_NULL_POINTER,


    };

}

#endif // #ifndef BIAS_BASIC_TYPES_HPP
