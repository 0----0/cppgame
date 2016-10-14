#include "Assets/Geometry/UnindexedGeometry.hpp"

#include "Assets/Geometry.hpp"

#include "extern/mikktspace.h"

static int getNumFaces(const SMikkTSpaceContext* pContext) {
        auto geo = static_cast<UnindexedGeometry*>(pContext->m_pUserData);
        return geo->vertices.size() / 3;
}

static int getNumVerticesOfFace(const SMikkTSpaceContext*, const int) {
        return 3;
}

static void getPosition(const SMikkTSpaceContext * pContext, float fvPosOut[],
                        const int iFace, const int iVert)
{
        auto geo = static_cast<UnindexedGeometry*>(pContext->m_pUserData);
        auto& pos = geo->vertices.at(iFace * 3 + iVert);
        fvPosOut[0] = pos.x;
        fvPosOut[1] = pos.y;
        fvPosOut[2] = pos.z;
}

static void getNormal(const SMikkTSpaceContext * pContext, float fvPosOut[],
                        const int iFace, const int iVert)
{
        auto geo = static_cast<UnindexedGeometry*>(pContext->m_pUserData);
        auto& norm = geo->normals.at(iFace * 3 + iVert);
        fvPosOut[0] = norm.x;
        fvPosOut[1] = norm.y;
        fvPosOut[2] = norm.z;
}

static void getTexCoord(const SMikkTSpaceContext * pContext, float fvPosOut[],
                        const int iFace, const int iVert)
{
        auto geo = static_cast<UnindexedGeometry*>(pContext->m_pUserData);
        auto& tex = geo->texCoords.at(iFace * 3 + iVert);
        fvPosOut[0] = tex.x;
        fvPosOut[1] = tex.y;
}

static void setTSpaceBasic(const SMikkTSpaceContext * pContext,
                           const float fvTangent[], const float fSign,
                           const int iFace, const int iVert)
{
        auto geo = static_cast<UnindexedGeometry*>(pContext->m_pUserData);
        auto vertID = iFace * 3 + iVert;
        geo->tangents.at(vertID) = {fvTangent[0], fvTangent[1], fvTangent[2]};
        geo->bitangents.at(vertID) = {fSign, fSign, fSign};
}

static SMikkTSpaceContext getMikkTSpaceContext(UnindexedGeometry& geo) {
        static SMikkTSpaceInterface interface {
                .m_getNumFaces = getNumFaces,
                .m_getNumVerticesOfFace = getNumVerticesOfFace,
                .m_getPosition = getPosition,
                .m_getNormal = getNormal,
                .m_getTexCoord = getTexCoord,
                .m_setTSpaceBasic = setTSpaceBasic,
                .m_setTSpace = nullptr
        };
        return SMikkTSpaceContext {
                .m_pInterface = &interface,
                .m_pUserData = &geo
        };
}

void UnindexedGeometry::calculateTangents() {
        if (m_hasTangents) return;
        if (!m_hasNormals)
                throw std::runtime_error("UnindexedGeometry must have normals before generating tangents.");
        if (!m_hasTexCoords)
                throw std::runtime_error("UnindexedGeometry must have texCoords before generating tangents.");

        tangents.resize(vertices.size());
        bitangents.resize(vertices.size());
        auto context = getMikkTSpaceContext(*this);
        genTangSpaceDefault(&context);
        m_hasTangents = true;
}

bool UnindexedGeometry::hasNormals() const {
        return m_hasNormals;
}

bool UnindexedGeometry::hasTangents() const {
        return m_hasTangents;
}

bool UnindexedGeometry::hasTexCoords() const {
        return m_hasTexCoords;
}


UnindexedGeometry::UnindexedGeometry(const Geometry& geo) {
        m_hasNormals = geo.hasNormals();
        m_hasTangents = geo.hasTangents();
        m_hasTexCoords = geo.hasTexCoords();
        for (auto i : geo.indices) {
                vertices.push_back(geo.vertices[i]);
                if (hasNormals()) normals.push_back(geo.normals[i]);
                if (hasTangents()) tangents.push_back(geo.tangents[i]);
                if (hasTexCoords()) texCoords.push_back(geo.texCoords[i]);
        }
}
