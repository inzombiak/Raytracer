#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <atomic>

/*
    mutex - When locked, threads block until they can acquire it, allowing for synch
    unique_lock - Locks the mutex for the scope
    condition_variable - 
        .notify_one() - notifies a single thread 
        .notify_all() - notifies all waiting threads
        .wait() - waits on a condition before proceeding. When a notify_one is used, a random thread checks its wait(), 
                  when notify all is used all of them do
    atomic - a variable that is free from data races. Basically, guarantees CORRECT informaiton. Slow 
*/
class ThreadPool {
    public:
        void start() {
            njobs_pending = 0;
            uint32_t num_threads = std::thread::hardware_concurrency();
            for(uint32_t i = 0; i < num_threads; ++i)
                threads.push_back(std::thread(&ThreadPool::threadLoop, this));

        }
        void queueJob(std::function<void(int)> job, int i) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                jobs.push( [job, i] () {job(i);} );
                ++njobs_pending;
            }
            mutex_condition.notify_one();
        };
        void stop() {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                should_terminate = true;
            }
            mutex_condition.notify_all();
            for (std::thread& active_thread : threads) {
                active_thread.join();
            }
            threads.clear();
        }
        void waitForCompletion() {
            std::unique_lock<std::mutex> lock(main_mutex);
            main_condition.wait(lock);
        }

        int jobCount() const {
            return jobs.size();
        }

    private:
        void threadLoop() {
            while (true) {
                std::function<void()>  job;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    mutex_condition.wait(lock, [this] {
                        return !jobs.empty() || should_terminate;
                    });
                    if (should_terminate) {
                        return;
                    }
                    job = jobs.front();
                    jobs.pop();
                }
                job();
                if ( --njobs_pending == 0 ) {
                    main_condition.notify_one();
                } else {
                    clog << njobs_pending << " jobs left\n";
                }
            }
        }

        bool should_terminate = false;
        std::mutex queue_mutex;
        std::condition_variable mutex_condition;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> jobs;

        std::atomic<int> njobs_pending;
        std::mutex main_mutex;
        std::condition_variable main_condition;

};

#endif