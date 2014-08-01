#include "affinity.hpp"
#include <iostream>
#include <map>

#ifdef WIN32
#include <windows.h>
#endif

namespace bias
{

    unsigned int ThreadAffinityService::numberOfCameras_ = 0;
    QMutex ThreadAffinityService::coutMutex_; 

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
            std::cout << "Error unable to get process affinity mask" << std::endl;
            return false;
        }

        std::map<unsigned int, unsigned int> cameraNumToProcBitMap;
        DWORD_PTR allCameraProcMask = 0;
        unsigned int cameraCnt = 0;

        for (unsigned int i=0; i<sizeof(DWORD_PTR); i++)
        {
            if ( ((1<<i) & availProcMask) != 0)
            {
                cameraNumToProcBitMap[cameraCnt] = i;
                allCameraProcMask |= (1 << i);
                cameraCnt++;
                if (cameraCnt>=numberOfCameras_)
                {
                    break;
                }
            }
        }
        if (cameraCnt = 0)
        {
            return false;
        }
        if (cameraCnt < numberOfCameras_)
        {
            // Too many cameras for number of available processors - put all remaining 
            // cameras on last available processor.
            for (unsigned int i=cameraCnt; i<numberOfCameras_; i++)
            {
                cameraNumToProcBitMap[i] = cameraNumToProcBitMap[cameraCnt-1];
            }
            cameraCnt = numberOfCameras_;
        }


        DWORD_PTR cameraProcMask = (1 << cameraNumToProcBitMap[cameraNumber]);
        DWORD_PTR normalProcMask = availProcMask & ~allCameraProcMask;
        if (isImageGrabber)
        {
            rval = SetThreadAffinityMask(GetCurrentThread(), cameraProcMask);

        }
        else
        {
            rval = SetThreadAffinityMask(GetCurrentThread(), normalProcMask);
        }

        

        //// Get index of first available processor
        //unsigned int grabberProcNum;
        //for (grabberProcNum = 0; grabberProcNum< sizeof(DWORD_PTR); grabberProcNum++)
        //{
        //    if ( ((1<<grabberProcNum) & availProcMask) != 0 )
        //    {
        //        break;
        //    }
        //}
        //DWORD_PTR cameraProcMask = 1<<grabberProcNum;
        //DWORD_PTR normalProcMask = availProcMask & ~(1<<grabberProcNum);


        //if (isImageGrabber)
        //{
        //    rval = SetThreadAffinityMask(GetCurrentThread(), cameraProcMask);
        //}
        //else
        //{
        //    rval = SetThreadAffinityMask(GetCurrentThread(), normalProcMask);
        //}

        //if (!rval)
        //{
        //    std::cout << "Error unable to set thread affinity mask" << std::endl;
        //    return false;
        //}

        
        coutMutex_.lock();
        std::cout << std::endl;
        std::cout << "numberOfCameras:   " << numberOfCameras_ << std::endl;
        std::cout << "isImageGrabber:    " << isImageGrabber << std::endl;
        //std::cout << "grabberProcNum:    " << grabberProcNum << std::endl;
        std::cout << "processor:         " << GetCurrentProcessorNumber() << std::endl;
        std::cout << "availProcMask:     " << std::hex << int(availProcMask)  << std::dec << std::endl;
        std::cout << "allCameraProcMask: " << std::hex << int(allCameraProcMask) << std::dec << std::endl;
        std::cout << "cameraProcMask:    " << std::hex << int(cameraProcMask) << std::dec << std::endl;
        std::cout << "normalProcMask:    " << std::hex << int(normalProcMask) << std::dec << std::endl;
        std::cout << std::endl;
        coutMutex_.unlock();
#endif

        return true;


    }  // assignThreadAffinity



    // Old method
    //  ---------------------------------------------------------------------------------
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
        
        std::cout << "**************** old thread affinity function" << std::endl;
        //std::cout << "isImageGrabber:  " << isImageGrabber << std::endl;
        //std::cout << "grabberProcNum:  " << grabberProcNum << std::endl;
        //std::cout << "processor:       " << GetCurrentProcessorNumber() << std::endl;
        //std::cout << "availProcMask:   " << std::hex << int(availProcMask)  << std::dec << std::endl;
        //std::cout << "cameraProcMask:  " << std::hex << int(cameraProcMask) << std::dec << std::endl;
        //std::cout << "normalProcMask:  " << std::hex << int(normalProcMask) << std::dec << std::endl;
#endif

        return true;

    } // bool assignThreadAffinity 


} // namespace bias
