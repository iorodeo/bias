#include "affinity.hpp"
#include <iostream>
#include <map>

#ifdef WIN32
#include <windows.h>
#endif

namespace bias
{

    unsigned int ThreadAffinityService::numberOfCameras_ = 0;
    QMutex ThreadAffinityService::coutDebugMutex_; 

    void ThreadAffinityService::setNumberOfCameras(unsigned int numberOfCameras)
    {
        numberOfCameras_ = numberOfCameras;
    }
    
    bool ThreadAffinityService::assignThreadAffinity(bool isImageGrabber, unsigned int cameraNumber)
    {
        bool rval;

        if (numberOfCameras_ == 0)
        {
            return false;
        }
        if (cameraNumber >= numberOfCameras_)
        {
            return false;
        }

#ifdef WIN32

        DWORD_PTR availProcMask;
        DWORD_PTR systemMask;

        rval = GetProcessAffinityMask(GetCurrentProcess(),&availProcMask,&systemMask);
        if (!rval) 
        { 
            // Unable to get process affinity mask
            return false;
        }

        std::map<unsigned int, DWORD_PTR> cameraNumToProcMask;
        DWORD_PTR allGrabberProcMask = 0;
        unsigned int cameraCnt = 0;

        for (unsigned int i=0; i<sizeof(DWORD_PTR); i++)
        {
            if ( ((1<<i) & availProcMask) != 0)
            {
                cameraNumToProcMask[cameraCnt] =  DWORD_PTR(1<<i);
                allGrabberProcMask |= DWORD_PTR(1 << i);
                cameraCnt++;
                if (cameraCnt>=numberOfCameras_)
                {
                    break;
                }
            }
        }
        if (cameraCnt == 0)
        {
            return false;
        }
        if (cameraCnt < numberOfCameras_)
        {
            // Too many cameras for number of available processors. 
            for (unsigned int i=cameraCnt; i<numberOfCameras_; i++)
            {
                // Put all remaining  cameras on last available processor.
                cameraNumToProcMask[i] = cameraNumToProcMask[cameraCnt-1];
            }
            cameraCnt = numberOfCameras_;
        }


        DWORD_PTR grabberProcMask = cameraNumToProcMask[cameraNumber];
        DWORD_PTR normalProcMask = availProcMask & ~allGrabberProcMask;
        if (isImageGrabber)
        {
            rval = SetThreadAffinityMask(GetCurrentThread(), grabberProcMask);

        }
        else
        {
            rval = SetThreadAffinityMask(GetCurrentThread(), normalProcMask);
        }

        
        //coutDebugMutex_.lock();
        //std::cout << std::endl;
        //std::cout << "numberOfCameras:    " << numberOfCameras_ << std::endl;
        //std::cout << "cameraNumber:       " << cameraNumber << std::endl;
        //std::cout << "isImageGrabber:     " << isImageGrabber << std::endl;
        //std::cout << "processor:          " << GetCurrentProcessorNumber() << std::endl;
        //std::cout << "availProcMask:      " << std::hex << int(availProcMask)  << std::dec << std::endl;
        //std::cout << "allGrabberProcMask: " << std::hex << int(allGrabberProcMask) << std::dec << std::endl;
        //std::cout << "grabberProcMask:    " << std::hex << int(grabberProcMask) << std::dec << std::endl;
        //std::cout << "normalProcMask:     " << std::hex << int(normalProcMask) << std::dec << std::endl;
        //std::cout << std::endl;
        //coutDebugMutex_.unlock();
#endif

        return true;


    }  // assignThreadAffinity


} // namespace bias
