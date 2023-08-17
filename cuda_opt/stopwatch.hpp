#pragma once

#include <iostream>

class stopwatch {
    private:
        int start_time;
        int end_time;
        bool running;
        
    public:
        stopwatch() {
            start_time = 0;
            end_time = 0;
            running = false;
        }
        
        void start() {
            if (!running) {
                start_time = clock();
                running = true;
            }
        }

        void stop() {
            running = false;
            end_time = clock();
        }

        int elapsed() {
            if (running) {
                return clock() - start_time;
            } else {
                return end_time - start_time;
            }
        }
        
        void reset() {
            start_time = 0;
            end_time = 0;
            running = false;
        }
};