#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
class Semaphore {
public:
	Semaphore(long count = 0)
		: count_(count) {
	}

    void Signal(unsigned int c = 1) {
		std::unique_lock<std::mutex> lock(mutex_);
        count_=c;
		cv_.notify_one();
	}

	void Wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [=] { return count_ > 0; });
		--count_;
	}

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	long count_;
};

#endif //__SEMAPHORE_H__
