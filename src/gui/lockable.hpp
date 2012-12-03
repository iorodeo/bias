#ifndef BIAS_LOCKABLE_HPP
#define BIAS_LOCKABLE_HPP

#include <QMutex>

namespace bias
{
    class Empty {};

    template <class T>
    class Lockable : public T 
    {

        // Mixin class for creating lockable objects

        public:

            Lockable() : T() {};

            template <class... A>
            explicit Lockable(const A&... a) : T(a...) {};

            bool tryLock()
            {
                return mutex_.tryLock();
            }
            void acquireLock()
            {
                mutex_.lock();
            }

            void releaseLock()
            {
                mutex_.unlock();
            }

        private:
            QMutex mutex_;
            
    };
}

#endif // #ifndef BIAS_LOCKABLE_HPP
