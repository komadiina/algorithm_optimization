#pragma once

#include <vector>
#include <fstream>
#include <iostream>

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