#include "AssetManager.hpp"
#include "GL/GL.hpp"
#include "Geometry.hpp"
#include "GeometryBuffer.hpp"

#include "extern/stb_image.h"

static std::string expandAssetPath(const std::string& name) {
        std::string path = "../assets/";
        path.append(name);
        return path;
}

static std::string expandTexturePath(const std::string& name) {
        std::string path = "../assets/textures/";
        path.append(name);
        return path;
}

static GL::Texture2D textureFromImage(const std::string& filename) {
        int w, h, n;
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &n, 0);
        if (!data) { throw std::runtime_error(std::string{"Failed to load image: "}.append(filename)); }
        scope_exit([&] { stbi_image_free(data); });
        if (!w || !h) { throw std::runtime_error(std::string{"Empty image: "}.append(filename)); }

        GLenum type;
        switch (n) {
        case 1:
                type = GL_RED;
                break;
        case 2:
                type = GL_RG;
                break;
        case 3:
                type = GL_RGB;
                break;
        case 4:
                type = GL_RGBA;
                break;
        default:
                throw std::runtime_error(std::string{filename}.append(": Unknown number of components: ").append(std::to_string(n)));
        }

        GL::Texture2D tex;
        tex.assign(0, type, w, h, type, GL_UNSIGNED_BYTE, data);
        tex.generateMipmap();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return tex;
}

std::shared_ptr<const GL::Texture2D>
AssetManager::getImage(const std::string& name) {
        auto iter = images.find(name);

        if (iter != images.end()) {
                if (!iter->second.expired()) {
                        return iter->second.lock();
                }
        }

        auto path = expandTexturePath(name);
        auto img = std::make_shared<GL::Texture2D>(textureFromImage(path));
        images[name] = img;

        return img;
}

std::shared_ptr<Geometry>
AssetManager::getModel(const std::string& name) {
        auto iter = models.find(name);

        if (iter != models.end()) {
                if (!iter->second.expired()) {
                        return iter->second.lock();
                }
        }

        auto path = expandAssetPath(name);
        auto img = std::make_shared<Geometry>(Geometry::fromPly(path));
        models[name] = img;

        return img;
}

std::shared_ptr<const GeometryBuffer>
AssetManager::getModelBuffer(const std::string& name) {
        auto iter = modelBuffers.find(name);

        if (iter != modelBuffers.end()) {
                if (!iter->second.expired()) {
                        return iter->second.lock();
                }
        }

        auto img = std::make_shared<GeometryBuffer>(*getModel(name));
        modelBuffers[name] = img;

        return img;
}
