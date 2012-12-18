#ifndef BIAS_LOCKABLE_HPP
#define BIAS_LOCKABLE_HPP

#include <QMutex>
#include <QWaitCondition>
#include <queue>

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

        protected:
            QMutex mutex_;
            
    };

    template <class T>
    class LockableQueue : public std::queue<T>, public Lockable<Empty>
    {
        // Mixin class for creating lockable queues 

        public:

            LockableQueue() : std::queue<T>() {};

            void clear()
            {
                while (!(this -> empty())) 
                { 
                    this -> pop();
                }
            }

            void waitIfEmpty()
            {
                if (this -> empty())
                {
                    emptyWaitCond_.wait(&mutex_);
                }
            }

            void signalNotEmpty()
            {
                emptyWaitCond_.wakeAll();
            }

        protected:
            QWaitCondition emptyWaitCond_;
    };
}

#endif // #ifndef BIAS_LOCKABLE_HPP
