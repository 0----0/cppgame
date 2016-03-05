#pragma once

#include <string>
#include <fstream>

inline std::string readFile(const char* filename) {
        std::string string;
        std::ifstream file(filename);
        file.seekg(0, std::ios::end);
        string.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&string[0], string.size());
        file.close();
        return string;
}
