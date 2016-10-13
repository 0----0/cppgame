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
                        auto shipDiffuse = AssetManager::get().getImage("ship1 diffuse AO2.png");
                        auto shipNormals = AssetManager::get().getImage("ship normalsdf.png");
                        if (objWk.expired()) {
                                std::cout << "Gotta construct the object" << std::endl;
                                auto obj = std::make_shared<Object>(
                                        glm::mat4(),
                                        AssetManager::get().getModelBuffer("ship1 v9.ply"),
                                        std::make_shared<Material>(shipDiffuse, shipNormals)
                                );
                                objWk = obj;
                                return obj;
                        } else {
                                return objWk.lock();
                        }
                }
        }
}
