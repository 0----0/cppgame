#pragma once

#include "BL/scope.hpp"

#include <unordered_map>
#include <memory>

namespace GL {
class Texture2D;
}
class Geometry;
class GeometryBuffer;

class AssetManager {
        template<typename T>
        using AssetMap = std::unordered_map<std::string, std::weak_ptr<T> >;

        AssetMap<const GL::Texture2D> images;
        AssetMap<Geometry> models;
        AssetMap<const GeometryBuffer> modelBuffers;

        AssetMap<std::string> scripts;
public:
        std::shared_ptr<const GL::Texture2D> getImage(const std::string& name);
        std::shared_ptr<Geometry> getModel(const std::string& name);
        std::shared_ptr<const GeometryBuffer> getModelBuffer(const std::string& name);

        std::shared_ptr<const std::string> getScript(const std::string& name);

        static AssetManager& get() {
                static AssetManager instance;
                return instance;
        }
};
