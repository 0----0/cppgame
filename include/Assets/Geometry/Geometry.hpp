
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

class UnindexedGeometry;
class GeometryBuffer;

using uint = unsigned int;

class Geometry {
private:
        using index_type = uint;
        bool m_hasNormals = false;
        bool m_hasTangents = false;
        bool m_hasTexCoords = false;

        std::unique_ptr<GeometryBuffer> buffer;
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

        GeometryBuffer& getBuffer();

        static Geometry square();
        static Geometry fromPly(const std::string& filename);

        static Geometry fromObj(const std::string& filename);

        static Geometry fromUnindexed(const UnindexedGeometry&);
        static Geometry fromUnindexed(UnindexedGeometry&&);

        Geometry();
        Geometry(const Geometry&) = delete;
        Geometry(Geometry&&);
        Geometry& operator=(const Geometry& rhs) = delete;
        Geometry& operator=(Geometry&& rhs);
        ~Geometry();
};
