#ifndef TIMERTASK_H
#define TIMERTASK_H

#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>



class TimerMan
{
    public:
        struct TimerTask
        {
            timespec lastTime;
            timespec currentTime;
            size_t timePulse;
            bool used;
            std::function<void()> task;
        };

        static TimerMan& getInstance()
        {
            static TimerMan timerMan;
            return timerMan;
        }

        TimerMan()
        {
            runFlag_ = true;
            bgTask_ = std::move(std::thread(std::bind(&TimerMan::run, this)));
        }

        ~TimerMan()
        {
            runFlag_ = false;
            bgTask_.join();
        }

        // 1秒以上周期调用使用
        void setWithTi(const std::function<void()>& task, size_t Ti)  // ti second
        {
            TimerTask timerTask;
            if(Ti < 0) Ti = 1;

            timerTask.task = task;
            timerTask.timePulse = 1000000000 * Ti;
            clock_gettime(CLOCK_MONOTONIC, &timerTask.lastTime);
            timerTask.used = true;
            std::unique_lock<std::mutex> l(mx_);
            timerTasks_.push_back(timerTask); 
        }

        // 1秒一下周期调用使用
        void setWithFreq(const std::function<void()>& task, size_t runFreq)
        {
            TimerTask timerTask;
            if(runFreq > 1000000)    runFreq = 1000000;
            else if(runFreq < 0)     runFreq = 1;

            timerTask.task = task;
            timerTask.timePulse = 1000000000 / runFreq;
            clock_gettime(CLOCK_MONOTONIC, &timerTask.lastTime);
            timerTask.used = true;

            printf("timePulse = %lu\n", timerTask.timePulse);

            std::unique_lock<std::mutex> l(mx_);
            timerTasks_.push_back(timerTask); 
        }

        void run()
        {
            uint64_t deltaT;
            while(runFlag_)
            {
                {
                    std::unique_lock<std::mutex> l(mx_);
                    for(size_t i = 0; i < timerTasks_.size(); i++)
                    {
                        if(timerTasks_[i].used)
                        {
                            clock_gettime(CLOCK_MONOTONIC, &timerTasks_[i].currentTime);
                            deltaT = (timerTasks_[i].currentTime.tv_sec - timerTasks_[i].lastTime.tv_sec) * 1000000000 
                                + timerTasks_[i].currentTime.tv_nsec - timerTasks_[i].lastTime.tv_nsec;

                            if(deltaT > (timerTasks_[i].timePulse - 50000))  // accurate +-50000 ns
                            {
                                timerTasks_[i].task();
                                timerTasks_[i].lastTime = timerTasks_[i].currentTime;
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::microseconds(100)); 
                    }
                }
            }
        }

    private:
        std::vector<TimerTask> timerTasks_;
        std::mutex mx_;
        std::thread bgTask_;
        bool runFlag_;
};














#endif




