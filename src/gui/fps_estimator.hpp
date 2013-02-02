#ifndef BIAS_FPS_ESTIMATOR_HPP
#define BIAS_FPS_ESTIMATOR_HPP

namespace bias
{
    class FPS_Estimator
    {
        //const double DEFAULT_CUTOFF_FREQ = 100.0;
        const double DEFAULT_CUTOFF_FREQ = 0.1;

        public:
            FPS_Estimator();
            double getValue() const;
            double getCutOffFreq() const;
            double setCutOffFreq(double freq);
            void reset();
            void update(double timeNew); 

        private:
            double fps_;
            double cutOffFreq_;
            double timeLast_;
            bool isFirst_;
    };

}

#endif // #ifndef BIAS_FPS_ESTIMATOR_HPP
