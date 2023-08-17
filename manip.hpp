#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <omp.h>
#include <thread>

// in-place
void process_sequential(std::vector<short> &data, const std::function<void(short &)> &func)
{
    for (short &sample : data)
    {
        func(sample);
    }
}

// in-place
void process_parallel(std::vector<short> &data, const std::function<void(short &)> &func, short num_threads)
{
    int segment_size = data.size() / num_threads;
    std::vector<std::thread> threads;

    // printf("Processing %d samples with %d threads.\n", data.size(), num_threads);

    for (int i = 0; i < num_threads; i++)
    {
        int start = i * segment_size;
        int end = (i + 1) * segment_size;
        threads.push_back(std::thread([start, end, &data, &func]()
                          {
            for (int i = start; i < end; i++)
            {
                func(data[i]);
            } }));

        // printf("Thread %d working on [%d:%d].\n", i, start, end);
    }

    for (auto &thread : threads)
        thread.join();
}

void process_omp(std::vector<short> &data, const std::function<void(short &)> &func, short num_threads)
{
    int segment_size = data.size() / num_threads;

    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for
    for (int i = 0; i < num_threads; i++)
    {
        int start = i * segment_size;
        int end = (i + 1) * segment_size;

        for (int i = start; i < end; i++)
        {
            func(data[i]);
        }
    }
}

namespace demo_filters
{
    std::function<void(short &)> halve = [](short &sample)
    {
        sample = sample / 2;
    };

    std::function<void(short &)> inverse_polarity = [](short &sample)
    {
        sample = -sample;
    };

    std::function<void(short &)> amplify = [](short &sample)
    {
        sample = sample * 2;
    };

    std::function<void(short &)> invert = [](short &sample)
    {
        sample = ~sample;
    };
}