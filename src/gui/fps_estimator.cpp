#include "fps_estimator.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <iostream>

namespace bias
{

    FPS_Estimator::FPS_Estimator()
    {
        cutOffFreq_ = DEFAULT_CUTOFF_FREQ;
        reset();
    }

    double FPS_Estimator::getValue()
    {
        return fps_;
    }

    double FPS_Estimator::getCutOffFreq()
    {
        return cutOffFreq_;
    }

    double FPS_Estimator::setCutOffFreq(double freq)
    {
        cutOffFreq_ = freq;

    }

    void FPS_Estimator::update(double timeNew) 
    {
        if (!isFirst_) 
        {
            double dt = timeNew - timeLast_;
            double fpsRaw = 1.0/dt;

            if (fpsRaw != std::numeric_limits<double>::infinity())
            {
                double tc = 1.0/(2.0*M_PI*cutOffFreq_);
                double alpha = dt/(tc + dt);
                fps_ = alpha*fpsRaw + (1.0-alpha)*fps_;
            }
        }
        else 
        {
            isFirst_ = false;
        }
        timeLast_ = timeNew;
    }

    void FPS_Estimator::reset()
    {
        isFirst_ = true;
        fps_ = 0;
        timeLast_ = 0;
    }

}
