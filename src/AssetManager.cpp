#include <fstream>

#include "AssetManager.hpp"
#include "GL/GL.hpp"
#include "Assets/Geometry.hpp"
#include "Assets/Geometry/GeometryBuffer.hpp"
#include "LuaInterface.hpp"

#include "extern/stb_image.h"

static std::string expandAssetPath(const std::string& name) {
        std::string path = "../assets/";
        path.append(name);
        return path;
}


typename AssetTagTrait<ModelAssetTag>::AssetType
AssetTagTrait<ModelAssetTag>::load(const std::string& name) {
        return Geometry::fromPly(expandAssetPath(name));
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

static std::string expandTexturePath(const std::string& name) {
        std::string path = "../assets/textures/";
        path.append(name);
        return path;
}

typename AssetTagTrait<TextureAssetTag>::AssetType
AssetTagTrait<TextureAssetTag>::load(const std::string& name) {
        return textureFromImage(expandTexturePath(name));
}

static std::string expandScriptPath(const std::string& name) {
        std::string path = "../assets/scripts/";
        path.append(name);
        return path;
}

static std::string readFile(const char* filename) {
        std::string string;
        std::ifstream file(filename);
        file.seekg(0, std::ios::end);
        string.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&string[0], string.size());
        file.close();
        return string;
}

typename AssetTagTrait<ScriptAssetTag>::AssetType
AssetTagTrait<ScriptAssetTag>::load(const std::string& name) {
        return readFile(expandScriptPath(name).c_str());
}

std::shared_ptr<const GL::Texture2D>
AssetManager::getTexture(const std::string& name) {
        return images.get(name);
}

std::shared_ptr<Geometry>
AssetManager::getModel(const std::string& name) {
        return models.get(name);
}

std::shared_ptr<const std::string>
AssetManager::getScript(const std::string& name) {
        return scripts.get(name);
}
