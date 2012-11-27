#ifndef BIAS_LOCKING_QUEUE_HPP
#define BIAS_LOCKING_QUEUE_HPP

#include <queue>
#include <QMutex>

namespace bias
{

    template <class T>
    class LockableQueue : public std::queue<T>
    {
        public:
            void acquireLock()
            {
                mutex.lock();
            }

            void releaseLock()
            {
                mutex.unlock();
            }

            bool tryLock()
            {
                return mutex.tryLock();
            }

            void clear()
            {
                while (!(this -> empty())) 
                { 
                    this -> pop();
                }
            }

        private:
            QMutex mutex;
    };

}

#endif // #ifndef BIAS_LOCKING_QUEUE_HPP
