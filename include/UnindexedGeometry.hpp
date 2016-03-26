#pragma once

#include <glm/glm.hpp>

#include <vector>

class Geometry;

class UnindexedGeometry {
private:
        using index_type = uint;
        bool m_hasNormals = false;
        bool m_hasTangents = false;
        bool m_hasTexCoords = false;
public:
        UnindexedGeometry(const Geometry&);

        bool hasNormals() const;
        bool hasTangents() const;
        bool hasTexCoords() const;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        void calculateTangents();
};
