#include "Assets/Image.hpp"

#include "extern/stb_image.h"

Image Image::load(const std::string& filename) {
        int w, h, n;
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &n, 0);
        if (!data) { throw std::runtime_error(std::string{"Failed to load image: "}.append(filename)); }
        if (!w || !h) { stbi_image_free(data); throw std::runtime_error(std::string{"Empty image: "}.append(filename)); }

        return Image{std::unique_ptr<unsigned char>{data}, w, h, n};
}
