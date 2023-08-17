#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>

float filesize(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    std::streampos fsize = 0;

    fsize = file.tellg();
    file.seekg(0, std::ios::end);
    fsize = file.tellg() - fsize;
    file.close();

    return fsize / 1024.0f / 1024.0f; // MB
}

void log(const std::string& text, std::ofstream& file) {
    file << text << std::endl;
}

std::string snapshot() {
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::string(std::ctime(&t));
}

std::string replace(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos;
    
    while ((start_pos = str.find(from)) != std::string::npos)
        str.replace(start_pos, from.length(), to);

    return str;
}