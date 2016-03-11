#pragma once

#include <glm/glm.hpp>

struct AxisAlignedBoundingBox {
        glm::vec2 lowerLeft;
        glm::vec2 upperRight;

        bool intersects(const AxisAlignedBoundingBox& rhs) const {
                return
                            lowerLeft.x < rhs.upperRight.x
                        && upperRight.x > rhs.lowerLeft.x
                        &&  lowerLeft.y < rhs.upperRight.y
                        && upperRight.y > rhs.lowerLeft.y
                ;
        }

        AxisAlignedBoundingBox operator+(const glm::vec2& rhs) const {
                AxisAlignedBoundingBox newBox{*this};
                newBox.lowerLeft += rhs;
                newBox.upperRight += rhs;
                return newBox;
        }
};

using AABB = AxisAlignedBoundingBox;
