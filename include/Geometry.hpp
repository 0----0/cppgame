
#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

#include <glm/glm.hpp>

class UnindexedGeometry;

using uint = unsigned int;

class Geometry {
private:
        using index_type = uint;
        bool m_hasNormals = false;
        bool m_hasTangents = false;
        bool m_hasTexCoords = false;
public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        std::vector<index_type> indices;

        void calculateNormals();

        void calculateTangents();

        void calculateTexCoords();

        bool hasNormals() const;
        bool hasTangents() const;
        bool hasTexCoords() const;

        static Geometry square();
        static Geometry fromPly(const std::string& filename);

        static Geometry fromObj(const std::string& filename);

        static Geometry fromUnindexed(const UnindexedGeometry&);
        static Geometry fromUnindexed(UnindexedGeometry&&);
};
