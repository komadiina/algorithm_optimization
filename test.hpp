#pragma once

#include "manip.hpp"
#include "utils.hpp"
#include "wav.hpp"
#include "stopwatch.hpp"

void test_sequential(std::string filename)
{
    WAV w(filename);
    w.init();
    auto original = std::vector<short>(w.data);
    stopwatch sw;
    sw.start();
    process_sequential(w.data, demo_filters::halve);
    sw.stop();
    printf("Sequential: %d [ms].\n", sw.elapsed());
    w.save("results/sequential_" + filename);
    w.data = original;
}

void test_parallel(std::string filename, int num_threads)
{
    WAV w(filename);
    w.init();
    auto original = std::vector<short>(w.data);
    stopwatch sw;
    sw.start();
    process_parallel(w.data, demo_filters::halve, num_threads);
    sw.stop();
    printf("Segmenting (%d threads): %d [ms].\n", num_threads, sw.elapsed());
    w.save("results/segmenting_" + filename);
    w.data = original;
}

void test_omp(std::string filename, int num_threads)
{
    WAV w(filename);
    w.init();
    auto original = std::vector<short>(w.data);
    stopwatch sw;
    sw.start();
    process_omp(w.data, demo_filters::halve, num_threads);
    sw.stop();
    printf("OpenMP (%d threads): %d [ms].\n", num_threads, sw.elapsed());
    w.save("results/openmp_" + filename);
    w.data = original;
}

void test()
{
    std::vector<std::string> filenames = {"vlong.wav", "long.wav", "medium.wav", "short.wav", "vshort.wav"};

    for (auto &filename : filenames)
    {
        std::cout << "-------------- " << filename << " --------------\n";

        for (int i = 0; i < 4; i++)
            test_sequential(filename);
            
            

        for (int j = 0; j < 4; j++)
            test_parallel(filename, 2);

        for (int j = 0; j < 4; j++)
            test_parallel(filename, 4);

        for (int j = 0; j < 4; j++)
            test_parallel(filename, 8);


        for (int j = 0; j < 4; j++)
            test_omp(filename, 2);

        for (int j = 0; j < 4; j++)
            test_omp(filename, 4);

        for (int j = 0; j < 4; j++)
            test_omp(filename, 8);
    }
}