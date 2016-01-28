#pragma once

#include "GL/GL.hpp"
#include "Geometry.hpp"

class GeometryBuffer {
public:
        GL::Buffer vertices;
        GL::Buffer normals;
        GL::Buffer texCoords;
        GL::Buffer indices;
        GL::Buffer tangents;
        GL::Buffer bitangents;
        unsigned int size;

        GeometryBuffer(Geometry& geo) {
                loadGeometry(geo);
        }

        void loadGeometry(Geometry& geo) {
                geo.calculateNormals();
                geo.calculateTangents();
                vertices.assign(geo.vertices, GL_STATIC_DRAW);
                normals.assign(geo.normals, GL_STATIC_DRAW);
                texCoords.assign(geo.texCoords, GL_STATIC_DRAW);
                indices.assign(geo.indices, GL_STATIC_DRAW);
                tangents.assign(geo.tangents, GL_STATIC_DRAW);
                bitangents.assign(geo.bitangents, GL_STATIC_DRAW);
                size = geo.indices.size();
        }

        void bind(GL::VertexArray& vao) {
                vao.bindVertexBuffer<glm::vec3>(0, vertices, 0);
                vao.bindVertexBuffer<glm::vec3>(1, normals, 0);
                vao.bindVertexBuffer<glm::vec2>(2, texCoords, 0);
                vao.bindVertexBuffer<glm::vec3>(3, tangents, 0);
                vao.bindVertexBuffer<glm::vec3>(4, bitangents, 0);
        }

        void draw() {
                indices.bindElems();
                glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, NULL);
        }
};
