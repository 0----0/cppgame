
#pragma once

#include <memory>

class Image {
public:
    std::unique_ptr<unsigned char> data;
    int w;
    int h;
    int n;

    static Image load(const std::string& filename);
};
