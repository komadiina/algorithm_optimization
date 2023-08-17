#pragma once

#include <iostream>
#include <fstream>
#include <vector>

typedef struct
{
    /* --- RIFF chunk descriptor --- */
    char chunk_id[4]; // "RIFF"
    int chunk_size; // 4 + 8 + SubChunk1Size + 8 + SubChunk2Size
    char format[4];   // "WAVE" for .wav

    /* ------- FMT sub-chunk ------- */
    char subchunk1_id[4];    // "FMT"
    int subchunk1_size;    // 16 for PCM
    short audio_format;    // 1: PCM (uncompressed), != 1: compressed
    short num_channels;    // 1: mono, 2: stereo ...
    int sample_rate;       // 8000, 44100, 48000 ...
    int byte_rate;         // SampleRate * NumChannels * BitsPerSample / 8
    short block_align;     // NumChannels * BitsPerSample / 8
    short bits_per_sample; // 8: 8bpc, 16: 16bpc

    /* ------ DATA sub-chunk ------ */
    char subchunk2_id[4]; // "DATA"
    int subchunk2_size; // NumSamples * NumChannels * BitsPerSample / 8
} WAVHeader;

class WAV
{
public:
    std::string filename;
    WAVHeader header;
    std::vector<short> data;

public:
    WAV() = default;
    WAV(std::string filename) : filename(filename) {}

    void init()
    {
        if (filename == "")
            throw std::runtime_error("No filename specified.");

        std::ifstream file = std::ifstream(filename, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + filename);

        read_header(file);
        read_data(file);

        file.close();
    }

    void read_header(std::ifstream &file)
    {
        // header.chunk_id = new char[4];
        // header.format = new char[4];
        // header.subchunk1_id = new char[4];
        // header.subchunk2_id = new char[4];

        file.read(header.chunk_id, 4);
        file.read((char *)&header.chunk_size, 4);
        file.read(header.format, 4);
        file.read(header.subchunk1_id, 4);
        file.read((char *)&header.subchunk1_size, 4);
        file.read((char *)&header.audio_format, 2);
        file.read((char *)&header.num_channels, 2);
        file.read((char *)&header.sample_rate, 4);
        file.read((char *)&header.byte_rate, 4);
        file.read((char *)&header.block_align, 2);
        file.read((char *)&header.bits_per_sample, 2);
        file.read(header.subchunk2_id, 4);
        file.read((char *)&header.subchunk2_size, 4);
    }

    void print_header(const WAVHeader &header)
    {
        std::cout << "chunk_id: " << header.chunk_id << std::endl;
        std::cout << "chunk_size: " << header.chunk_size << std::endl;
        std::cout << "format: " << header.format << std::endl;
        std::cout << "subchunk1_id: " << header.subchunk1_id << std::endl;
        std::cout << "subchunk1_size: " << header.subchunk1_size << std::endl;
        std::cout << "audio_format: " << header.audio_format << std::endl;
        std::cout << "num_channels: " << header.num_channels << std::endl;
        std::cout << "sample_rate: " << header.sample_rate << std::endl;
        std::cout << "byte_rate: " << header.byte_rate << std::endl;
        std::cout << "block_align: " << header.block_align << std::endl;
        std::cout << "bits_per_sample: " << header.bits_per_sample << std::endl;
        std::cout << "subchunk2_id: " << header.subchunk2_id << std::endl;
        std::cout << "subchunk2_size: " << header.subchunk2_size << std::endl;
    }

    void read_data(std::ifstream &file)
    {
        file.seekg(44, std::ios::beg); // skip header

        short sample;
        size_t bytes_per_sample = header.bits_per_sample / 8;

        for (int i = 0; i < header.subchunk2_size / bytes_per_sample * 2; i += bytes_per_sample)
        {
            file.read((char *)&sample, bytes_per_sample);
            data.push_back(sample);
        }
    }

    void save(std::string output_filename) {
        std::ofstream file(output_filename, std::ios::binary);
        file.seekp(0, std::ios_base::beg);
        
        write_header(file);
        write_data(file);
        
        file.close();        
    }
    
    private:
        void write_header(std::ofstream& file) {
            file.write((char*)&header.chunk_id, 4);
            file.write((char*)&header.chunk_size, 4);
            file.write((char*)&header.format, 4);

            // Write the 'fmt' sub-chunk
            file.write((char*)&header.subchunk1_id, 4);
            file.write((char*)&header.subchunk1_size, 4);
            file.write((char*)&header.audio_format, 2);
            file.write((char*)&header.num_channels, 2);
            file.write((char*)&header.sample_rate, 4);
            file.write((char*)&header.byte_rate, 4);
            file.write((char*)&header.block_align, 2);
            file.write((char*)&header.bits_per_sample, 2);

            // Write the 'data' sub-chunk
            file.write((char*)&header.subchunk2_id, 4);
            file.write((char*)&header.subchunk2_size, 4);
        }
        
        void write_data(std::ofstream& file) {
            file.seekp(0x2C, std::ios_base::beg);
            int bytes_per_sample = header.bits_per_sample / 8;
            for (short elem : data) 
                file.write((char*)&elem, bytes_per_sample);
        }
};