#include "manip.hpp"
#include "stopwatch.hpp"
#include "wav.hpp"
#include "utils.hpp"
#include "test.hpp"
#include <iostream>
#include <filesystem>
#include <string>

/*
i5 12400F (6C/12T | 480KB L1, 7.5MB L2, 18MB L3)
3060 Ti (8GB, 4864 CUDA cores, 152 Tensor cores, 448GB/s bandwidth)
32GB DDR5 5600MHz
*/

namespace fs = std::filesystem;

// int main()
// {
//     test();
//     return 0;
// }

int _main(int argC, char **argV)
{
    if (argC != 4)
    {
        std::cout << "Usage: " << argV[0]
                  << "<path to .wav file> "
                  << "<output file> "
                  << "<# of threads>" << std::endl;
        return 1;
    }

    printf("File size: %.2f [MB].\n", filesize(argV[1]));
    std::string time = snapshot();

    time = replace(time, " ", "_");
    time = replace(time, "\n", "");
    time = replace(time, ":", "-");

    if (!fs::directory_entry("results").exists())
    {
        fs::create_directory("results");
    }

    printf("Benchmarking - %s\n", time.c_str());
    std::ofstream file("results/" + time + ".log");
    log("--- " + std::string(argV[1]) + " ---\n", file);
    stopwatch sw;
    WAV w(argV[1]);
    w.init();

    int num_threads = atoi(argV[3]);
    auto original = std::vector<short>(w.data);

    // 1. Sequential
    sw.start();
    process_sequential(w.data, demo_filters::halve);
    sw.stop();
    printf("Sequential: %d [ms].\n", sw.elapsed());
    log("sequential " + std::to_string(sw.elapsed()), file);

    w.save("results/sequential_" + std::string(argV[2]));
    w.data = original;
    sw.reset();

    // 2. Parallelized
    sw.start();
    process_parallel(w.data, demo_filters::halve, num_threads);
    sw.stop();
    printf("Segmenting (%d threads): %d [ms].\n", num_threads, sw.elapsed());
    log("segmenting " + std::to_string(sw.elapsed()), file);

    w.save("results/segmenting_" + std::string(argV[2]));
    w.data = original;
    sw.reset();

    // 3. OpenMP
    sw.start();
    process_omp(w.data, demo_filters::halve, num_threads);
    sw.stop();
    printf("OpenMP (%d threads): %d [ms].\n", num_threads, sw.elapsed());
    log("openmp " + std::to_string(sw.elapsed()), file);
    w.save("results/openmp_" + std::string(argV[2]));
    w.data = original;
    sw.reset();

    file.close();

    return 0;
}
