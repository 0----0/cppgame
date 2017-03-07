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

struct AssetTagTraitBase {
        static constexpr bool alwaysReload = false;
};

template<typename T>
struct AssetTagTrait {
        //using AssetType;
        //static AssetType load(const std::string& name);
        //static AssetType loadDefault();
};

struct TextureAssetTag;
struct ModelAssetTag;
struct ScriptAssetTag;
struct ObjectAssetTag;

template<>
struct AssetTagTrait<TextureAssetTag> : public AssetTagTraitBase {
        using AssetType = GL::Texture2D;
        static AssetType load(const std::string& name);
        static AssetType loadDefault();
};

template<>
struct AssetTagTrait<ModelAssetTag> : public AssetTagTraitBase {
        using AssetType = Geometry;
        static AssetType load(const std::string& name);
        static AssetType loadDefault();
};

template<>
struct AssetTagTrait<ScriptAssetTag> : public AssetTagTraitBase {
        static constexpr bool alwaysReload = true;
        using AssetType = std::string;
        static AssetType load(const std::string& name);
        static AssetType loadDefault();
};

template<>
struct AssetTagTrait<ObjectAssetTag> : public AssetTagTraitBase {
        using AssetType = Object;
        static AssetType load(const std::string& name);
        static AssetType loadDefault();
};

template<typename AssetTag>
class Asset {
private:
        using AssetT = typename AssetTagTrait<AssetTag>::AssetType;

        bool isLoaded = false;
        std::string filename;
        std::shared_ptr<AssetT> asset;
public:

};

template<typename AssetTag>
class AssetMap {
private:
        using Trait = AssetTagTrait<AssetTag>;
        using AssetT = typename AssetTagTrait<AssetTag>::AssetType;
        std::unordered_map<std::string, std::shared_ptr<AssetT>> map;
        std::shared_ptr<AssetT> defaultAsset;
public:
        std::shared_ptr<AssetT> get(const std::string& name) {
                auto iter = map.find(name);

                // if (iter != map.end()) {
                //         if (!iter->second.expired()) {
                //                 return iter->second.lock();
                //         }
                // }
                if (!Trait::alwaysReload && iter != map.end()) {
                        return iter->second;
                }

                // auto path = loadAsset<AssetT>(name);
                // auto asset = loadAsset<AssetT>(name);
                // map[name] = asset;

                auto asset = std::make_shared<AssetT>(Trait::load(name));
                map[name] = asset;

                return asset;
        }
};

class AssetManager {
        AssetMap<TextureAssetTag> images;
        AssetMap<ModelAssetTag> models;
        AssetMap<ScriptAssetTag> scripts;
        AssetMap<ObjectAssetTag> objPrototypes;
public:
        std::shared_ptr<const GL::Texture2D> getTexture(const std::string& name);
        std::shared_ptr<const std::string> getScript(const std::string& name);
        std::shared_ptr<Geometry> getModel(const std::string& name);
        std::shared_ptr<Object> getObjPrototype(const std::string& name);

        static AssetManager& get() {
                static AssetManager instance;
                return instance;
        }
};
