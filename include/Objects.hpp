#include "Object.hpp"
#include "AssetManager.hpp"

#include <memory>
#include <iostream>

namespace Obj {
        namespace Enemy {

        }
        namespace Player {
                std::shared_ptr<const Object> ship() {
                        static std::weak_ptr<Object> objWk;
                        auto shipDiffuse = AssetManager::get().getTexture("Diffuse Baked i4.png");
                        auto shipNormals = AssetManager::get().getTexture("DefaultNormals.png");
                        auto shipMat = std::make_shared<Material>(shipDiffuse, shipNormals);
                        shipMat->shininess = 32.0f;
                        if (objWk.expired()) {
                                std::cout << "Gotta construct the object" << std::endl;
                                auto obj = std::make_shared<Object>(
                                        glm::mat4(),
                                        AssetManager::get().getModel("ship1 i4.ply"),
                                        std::move(shipMat)
                                );
                                objWk = obj;
                                return obj;
                        } else {
                                return objWk.lock();
                        }
                }
        }
}
