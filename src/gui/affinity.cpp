#include "affinity.hpp"
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

namespace bias
{
    bool assignThreadAffinity( 
            bool isImageGrabber, 
            unsigned int numberOfCameras, 
            unsigned int cameraNumber
            )
    {
        bool rval;
        unsigned int grabberProcNum;

        if (numberOfCameras == 0)
        {
            return false;
        }
        if (cameraNumber >= numberOfCameras)
        {
            return false;
        }

#ifdef WIN32

        DWORD_PTR availProcMask;
        DWORD_PTR systemMask;

        rval = GetProcessAffinityMask(GetCurrentProcess(),&availProcMask,&systemMask);
        if (!rval) 
        { 
            std::cout << "Error unable to get process affinity mask" << std::endl;
            // TO DO - raise error and abort
        }

        // Get index of first available processor
        for (grabberProcNum = 0; grabberProcNum< sizeof(DWORD_PTR); grabberProcNum++)
        {
            if ( ((1<<grabberProcNum) & availProcMask) != 0 )
            {
                break;
            }
        }

        DWORD_PTR cameraProcMask = 1<<grabberProcNum;
        DWORD_PTR normalProcMask = availProcMask & ~(1<<grabberProcNum);
        if (isImageGrabber)
        {
            rval = SetThreadAffinityMask(GetCurrentThread(), cameraProcMask);
        }
        else
        {
            rval = SetThreadAffinityMask(GetCurrentThread(), normalProcMask);
        }

        if (!rval)
        {
            std::cout << "Error unable to set thread affinity mask" << std::endl;
            // TO DO - raise error and abort
        }

        //std::cout << "isImageGrabber:  " << isImageGrabber << std::endl;
        //std::cout << "grabberProcNum:  " << grabberProcNum << std::endl;
        //std::cout << "processor:       " << GetCurrentProcessorNumber() << std::endl;
        //std::cout << "availProcMask:   " << std::hex << int(availProcMask)  << std::dec << std::endl;
        //std::cout << "cameraProcMask:  " << std::hex << int(cameraProcMask) << std::dec << std::endl;
        //std::cout << "normalProcMask:  " << std::hex << int(normalProcMask) << std::dec << std::endl;
#endif


    } // bool setThreadCPUAffinity


} // namespace bias
