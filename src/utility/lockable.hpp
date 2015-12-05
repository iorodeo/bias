#ifndef BIAS_LOCKABLE_HPP
#define BIAS_LOCKABLE_HPP

#include <QMutex>
#include <QWaitCondition>
#include <queue>
#include <set>

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


            bool tryLock(int timeout)
            {
                return mutex_.tryLock(timeout);
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

            void wakeOne()
            {
                emptyWaitCond_.wakeOne();
            }

        protected:
            QWaitCondition emptyWaitCond_;
    };


    template <class T, class TCmp>
    class LockableSet : public std::set<T,TCmp>, public Lockable<Empty>
    {
        // Mixin class for creating lockable sets

        public:

            LockableSet() : std::set<T,TCmp>() {};

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

} // namespace bias

#endif // #ifndef BIAS_LOCKABLE_HPP
