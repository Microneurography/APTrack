/*
This code was very kindly donated to the project by Matthew Nagy.
If the semaphore gets deleted while threads are waiting on it it could get freaky, but it should work fine. 
It is needed when handling files, because if two threads attempt to read/write to the same file,
the file will become blank. 
*/
#include <thread>
#include <mutex>
#include <atomic>

class Semaphore{
public:

    void incriment(){
        internalMutex.lock();
        semCount += 1;

		if (semCount == 1) {//Newly unlocked
			lock.try_lock();
			lock.unlock();
		}

        internalMutex.unlock();
    }

    void decriment(){
        internalMutex.lock();
        bool locked = true;

        if(semCount == 0){
            internalMutex.unlock();
            lock.lock();

            lock.lock();
            internalMutex.lock();
            semCount -= 1;
            lock.unlock();
        }
        else{
            semCount -= 1;
        }

        internalMutex.unlock();
    }

    Semaphore(unsigned semCount):
        semCount(semCount)
    {}
private:
    std::mutex lock;
    std::mutex internalMutex;
    std::atomic_uint semCount;
};