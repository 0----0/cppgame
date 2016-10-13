#pragma once

#include "BL/scope.hpp"

#include <unordered_map>
#include <memory>

namespace GL {
class Texture2D;
}
class Geometry;
class GeometryBuffer;
class Object;

template<typename T>
std::shared_ptr<T> loadAsset(const std::string& name);

template<typename ObjT>
class AssetMap {
private:
        std::unordered_map<std::string, std::weak_ptr<ObjT>> map;

public:
        std::shared_ptr<ObjT> get(const std::string& name) {
                auto iter = map.find(name);

                if (iter != map.end()) {
                        if (!iter->second.expired()) {
                                return iter->second.lock();
                        }
                }

                auto path = loadAsset<ObjT>(name);
                auto asset = loadAsset<ObjT>(name);
                map[name] = asset;

                return asset;
        }
};

class AssetManager {
        AssetMap<const GL::Texture2D> images;
        AssetMap<Geometry> models;
        AssetMap<const GeometryBuffer> modelBuffers;

        AssetMap<std::string> scripts;
        AssetMap<Object> objPrototypes;
public:
        std::shared_ptr<const GL::Texture2D> getImage(const std::string& name);
        std::shared_ptr<Geometry> getModel(const std::string& name);
        std::shared_ptr<const GeometryBuffer> getModelBuffer(const std::string& name);

        std::shared_ptr<const std::string> getScript(const std::string& name);
        std::shared_ptr<Object> getObjPrototype(const std::string& name);

        static AssetManager& get() {
                static AssetManager instance;
                return instance;
        }
};
