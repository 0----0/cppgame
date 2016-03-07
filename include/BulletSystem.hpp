#pragma once

#include "GL/GL.hpp"
#include "AssetManager.hpp"

#include <glm/glm.hpp>

#include <vector>

struct Bullet {
        glm::vec2 position;
        glm::vec2 velocity;
        unsigned int death;
};

class BulletSystem {
private:
        std::vector<glm::vec2> bulletPos;
        std::vector<glm::vec2> bulletVel;
        std::vector<unsigned int> bulletDeathTime;
        GL::Buffer bulletPosBuff;

public:
        std::shared_ptr<const GL::Texture2D> img {
                AssetManager::get().getImage("bullet1.png")
        };


        void addBullet(const Bullet& bullet) {
                bulletPos.push_back(bullet.position);
                bulletVel.push_back(bullet.velocity);
                bulletDeathTime.push_back(bullet.death);
        }

        void update() {
                for (int i = 0; i < bulletPos.size(); i++) {
                        bulletPos[i] += bulletVel[i];
                }
        }

        void prepareForDraw() {
                bulletPosBuff.assign(bulletPos, GL_DYNAMIC_DRAW);
        }
        unsigned int numBullets() const {
                return bulletPos.size();
        }
        const GL::Buffer& getBuffer() {
                return bulletPosBuff;
        }
};
