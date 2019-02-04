
/**
 * A mutex allow thread-safe operations
 */
namespace Mongoose
{
    class Mutex
    {
        public:
            Mutex();
            virtual ~Mutex();

            /**
             * Locks the mutex
             */
            virtual void lock();

            /**
             * Unlock the mutex
             */
            virtual void unlock();

        protected:
            pthread_mutex_t _mutex;
    };
}
