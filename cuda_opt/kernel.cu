
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "wav.hpp"
#include "stopwatch.hpp"
#include "manip.hpp"
#include "utils.hpp"

#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <thread>

// --------------
std::ofstream logfile("results.log");
const int numCudaCores = 4864; // RTX 3060 Ti (4864 CUDA Cores)
std::string outputFilename = "";
int numSegments = 4;
// --------------

__global__ void halveCudaHelper(short* data) {
    data[blockIdx.x * blockDim.x + threadIdx.x] /= 2;
}

void processCuda(std::vector<short>& samples, int threadsPerBlock) {
    short* data = samples.data();
    size_t numSamples = samples.size();

    cudaMalloc(&data, numSamples * sizeof(short));
    cudaMemcpy(data, samples.data(), numSamples, cudaMemcpyHostToDevice);

    int blocksPerGrid = (numSamples + threadsPerBlock - 1) / threadsPerBlock;
    halveCudaHelper<<<blocksPerGrid, threadsPerBlock>>> (data);

    cudaMemcpy(samples.data(), data, numSamples, cudaMemcpyDeviceToHost);
    cudaFree(data);
}


void processCudaSegmented(std::vector<short>& samples, int numSegments) {
    int segmentSize = samples.size() / numSegments;
    std::vector<std::thread> threads;

    for (int i = 0; i < numSegments; i++)
    {
        int start = i * segmentSize;
        int end = (i + 1) * segmentSize;
        threads.push_back(std::thread([start, end, numSegments, &samples]()
            {
                std::vector<short> segment(samples.begin() + start, samples.begin() + end);
                processCuda(segment, numCudaCores / numSegments);
            }
        ));
    }

    for (auto& t : threads)
        t.join();
}

void benchmark(const std::string& filename) {
    stopwatch sw;
    WAV w(filename);
    w.init();
    auto original = std::vector<short>(w.data);

    printf("Processing %s, size: %.2f MB.\n", filename.c_str(), filesize(filename));

    /*

    // 1. CUDA
    printf("Started: CUDA\n");
    sw.start();
    processCuda(w.data, numCudaCores);
    sw.stop();

    printf("CUDA: %d [ms].\n", sw.elapsed());
    log("CUDA: " + filename + " " + std::to_string(sw.elapsed()) + " [ms]", logfile);
    sw.reset();
    w.save("results/cuda_" + filename);

    */

    if (numSegments > 1) {
        if (numSegments > w.data.size())
        {
            fprintf(stderr, "Segmenting failed! num_segments > num_samples (%d > %d)\n", numSegments, w.data.size());
            exit(1);
        }

        // 2. CUDA + Segmenting
        printf("Started: CUDA + Segmenting\n");
        sw.start();
        processCudaSegmented(w.data, numSegments);
        sw.stop();
        printf("CUDA + Segmenting (%d segments): %d [ms].\n", numSegments, sw.elapsed());
        log("CUDA + Segmenting: " + filename + " " + std::to_string(sw.elapsed()) + " [ms]", logfile);
        sw.reset();
        w.save("results/cudasegmenting_" + filename);
    }

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaError_t cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        logfile.close();
        exit(1);
    }
}

void testall() {
    std::vector<std::string> filenames = { "long.wav", "medium.wav", "short.wav", "vshort.wav" };
    for (auto& filename : filenames)
        for (int i = 0; i < 4; i++) {
            benchmark(filename);
        }
}

int main(int argC, char **argV)
{
    //if (argC != 3 && argC != 4)
    //{
    //    std::cout << "Usage: "
    //        << "<path to .wav file> "
    //        << "<output file> "
    //        << "<optional: # of segments> "
    //        << std::endl;
    //    return 0;
    //}
    //else if (argC == 4)
    //    numSegments = std::atoi(argV[3]);

    /*for (int i = 0; i < 4; i++) {
        benchmark(std::string(argV[1]));
    }*/

    testall();

    logfile.close();
    cudaDeviceReset();
    
    std::cout << "Press any key to exit..." << std::endl;
    getchar();
    return 0;
}