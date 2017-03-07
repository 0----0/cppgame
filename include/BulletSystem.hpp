#pragma once

#include "GL/GL.hpp"
#include "AssetManager.hpp"
#include "AxisAlignedBoundingBox.hpp"

#include <glm/glm.hpp>

#include <vector>

struct Bullet {
        glm::vec2 position;
        glm::vec2 velocity;
        unsigned int death;
};

class BulletSystem {
private:
        std::vector<glm::vec2> bulletVel;
        std::vector<unsigned int> bulletDeathTime;
        GL::Buffer bulletPosBuff;
        glm::vec2 scale{1.0f, 1.0f};

public:
        std::vector<glm::vec2> bulletPos;
        AABB bounds{scale/-2.0f, scale/2.0f};

        std::shared_ptr<const GL::Texture2D> img {
                AssetManager::get().getTexture("bullet2.png")
        };

        BulletSystem(std::shared_ptr<const GL::Texture2D> img, glm::vec2 scale):
                scale( scale ),
                img( std::move(img) )
        {}


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
        const GL::Buffer& getBuffer() const {
                return bulletPosBuff;
        }

        glm::vec2 getScale() const {
                return scale;
        }
};
