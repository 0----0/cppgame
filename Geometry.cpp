#include "Geometry.hpp"

#include "UnindexedGeometry.hpp"

bool Geometry::hasNormals() const {
        return m_hasNormals;
}

bool Geometry::hasTangents() const {
        return m_hasTangents;
}

bool Geometry::hasTexCoords() const {
        // return m_hasTexCoords;
        return texCoords.size() == vertices.size();
}

void Geometry::calculateNormals() {
        if(m_hasNormals) return;
        normals.clear();
        normals.resize(vertices.size(), glm::vec3(0, 0, 0));

        for (auto i = indices.begin(); i != indices.end(); i += 3) {
                glm::vec3 faceVerts[3];
                std::transform(i, i+3, faceVerts, [&](auto j) {
                        return vertices.at(j);
                });
                glm::vec3 normal = glm::cross(
                        faceVerts[1] - faceVerts[0],
                        faceVerts[2] - faceVerts[0]
                );
                for(auto j = i; j < i + 3; ++j) {
                        normals[*j] += normal;
                }
        }

        for (auto& n : normals) {
                n = glm::normalize(n);
        }

        m_hasNormals = true;
}

// void Geometry::calculateTangents() {
//         if (m_hasTangents) return;
//         tangents.clear();
//         tangents.resize(vertices.size(), glm::vec3{0, 0, 0});
//
//         bitangents.clear();
//         bitangents.resize(vertices.size(), glm::vec3{0, 0, 0});
//
//         if (!m_hasTexCoords) return;
//
//         for(auto i = indices.begin(); i != indices.end(); i += 3) {
//                 glm::vec3 faceVerts[3];
//                 std::transform(i, i+3, faceVerts, [&](auto j) {
//                         return vertices.at(j);
//                 });
//                 glm::vec2 faceUVs[3];
//                 std::transform(i, i+3, faceUVs, [&](auto j) {
//                         return texCoords.at(j);
//                 });
//                 glm::vec2 deltaUV1 = faceUVs[1] - faceUVs[0];
//                 glm::vec2 deltaUV2 = faceUVs[2] - faceUVs[0];
//                 float invDet = 1.0f/(deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
//
//                 glm::vec3 deltaVert1 = faceVerts[1] - faceVerts[0];
//                 glm::vec3 deltaVert2 = faceVerts[2] - faceVerts[0];
//                 glm::vec3 firstVector = (deltaVert1 * deltaUV2.y - deltaVert2 * deltaUV1.y) * invDet;
//                 glm::vec3 secondVector = (deltaVert2 * deltaUV1.x - deltaVert1 * deltaUV2.x) * invDet;
//
//                 for(auto j = i; j < i + 3; ++j) {
//                         tangents.at(*j) += firstVector;
//                         bitangents.at(*j) += secondVector;
//                 }
//         }
//
//         for (auto& t : tangents) {
//                 t = glm::normalize(t);
//         }
//         for (auto& t : bitangents) {
//                 t = glm::normalize(t);
//         }
//
//         m_hasTangents = true;
// }

void Geometry::calculateTangents() {
        UnindexedGeometry uiGeo(*this);
        uiGeo.calculateTangents();
        *this = Geometry::fromUnindexed(std::move(uiGeo));
}

void Geometry::calculateTexCoords() {
        if (m_hasTexCoords) return;

        texCoords.clear();
        texCoords.resize(vertices.size(), glm::vec2{0.0f, 0.0f});

        m_hasTexCoords = true;
}

Geometry Geometry::square() {
        Geometry geo;

        geo.vertices = {{-1, -1, 0}, {1, -1, 0}, {-1, 1, 0}, {1, 1, 0}};
        geo.normals = {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};
        geo.indices = {0, 1, 2, 2, 3, 1};

        return geo;
}

Geometry Geometry::fromPly(const std::string& filename) {
        auto expect = [&](bool cond) {
                if(!cond) {
                        throw std::runtime_error {
                                "Improperly formatted .ply file: " + filename
                        };
                }
        };
        std::ifstream in(filename, std::ios::in);

        std::string line;
        getline(in, line);
        expect(line == "ply");

        std::istringstream lineS{line};
        std::string word;

        auto nextLine = [&]() {
                while(true) {
                        if(!getline(in, line)) return false;
                        lineS.str(line);
                        lineS.seekg(std::ios_base::beg);
                        lineS >> word;
                        if(word != "comment") return true;
                }
        };

        nextLine();
        // bool isAscii;
        expect(word == "format");
        lineS >> word;
        if(word == "ascii") {
                lineS >> word;
                expect(word == "1.0");
                // isAscii = true;
        } else {
                throw std::runtime_error {
                        "Parsing binary .ply files not yet implemented: " + filename
                };
        }

        using byte = unsigned char;
        struct Property {
                std::string type;
                std::string listNumType;
                std::string listElemType;
                std::string name;
                std::vector<byte> data;
        };
        struct Element {
                std::string name;
                uint num;
                std::vector<Property> props;

                Element(std::string&& name, uint num): name(std::move(name)), num(num) {}

                const Property& findProp(const std::string& name) {
                        for(auto& prop : props) {
                                if(prop.name == name) {
                                        return prop;
                                }
                        }
                        throw std::runtime_error {"Could not find property: " + name};
                };
                const Property* hasProp(const std::string& name) {
                        for(auto& prop : props) {
                                if(prop.name == name) {
                                        return &prop;
                                }
                        }
                        return nullptr;
                };
        };

        std::vector<Element> elements;
        auto findElem = [&](const std::string& name) -> Element& {
                for(auto& elem : elements) {
                        if(elem.name == name) {
                                return elem;
                        }
                }
                throw std::runtime_error {"Could not find element: " + name};
        };
        nextLine();
        while(word != "end_header") {
                expect(word == "element");
                lineS >> word; // name of element
                uint elemNum; lineS >> elemNum;
                elements.emplace_back(std::move(word), elemNum);

                while(nextLine() && word == "property") {
                        elements.back().props.push_back({});
                        Property& prop = elements.back().props.back();
                        lineS >> prop.type;
                        if(prop.type == "list") {
                                lineS >> prop.listNumType;
                                lineS >> prop.listElemType;
                        }
                        lineS >> prop.name;
                }

                // elements.back().propDatas.resize(elements.back().props.size());
        }
        expect(word == "end_header");

        for(auto& elem : elements) { for(int i = 0; i < elem.num; ++i) {
                expect(nextLine()); lineS.seekg(std::ios_base::beg);
                for(auto& prop : elem.props) {
                        auto& propData = prop.data;
                        // propData.reserve(elem.num);

                        auto putData = [&](auto data) {
                                using T = decltype(data);
                                propData.reserve(propData.capacity() + sizeof(T));
                                byte* start = reinterpret_cast<byte*>(&data);
                                byte* end = start + sizeof(T);
                                propData.insert(propData.end(), start, end);
                        };

                        auto readPutSimpleData = [&](const std::string& type) {
                                if (type == "float") {
                                        float data; lineS >> data;
                                        putData(data);
                                }
                                else if (type == "uchar") {
                                        int temp; lineS >> temp;
                                        byte data = temp;
                                        putData(data);
                                }
                                else if (type == "uint") {
                                        uint data; lineS >> data;
                                        putData(data);
                                }
                                else if (type == "int") {
                                        int data; lineS >> data;
                                        putData(data);
                                }
                                else {
                                        throw std::runtime_error {"Unsupported type: " + prop.type};
                                }
                        };

                        if(prop.type == "list") {
                                expect(prop.listNumType == "uchar");
                                // byte numElems; lineS >> numElems; //TODO: handle type of num elems
                                int temp; lineS >> temp;             //stil TODO
                                byte numElems = temp;

                                putData(numElems);
                                for (unsigned char e = 0; e < numElems; ++e) {
                                        readPutSimpleData(prop.listElemType);
                                }
                        } else {
                                readPutSimpleData(prop.type);
                        }
                }
        } }

        Geometry ret;
        auto getFloat = [](const Property& prop, int i) {
                return *(float*)&(prop.data[i*sizeof(float)]);
        };

        auto& vertices = findElem("vertex");
        ret.vertices.reserve(vertices.num);
        auto& x = vertices.findProp("x"); expect(x.type == "float");
        auto& y = vertices.findProp("y"); expect(y.type == "float");
        auto& z = vertices.findProp("z"); expect(z.type == "float");
        for(int i = 0; i < vertices.num; ++i) {
                ret.vertices.emplace_back(getFloat(x,i), getFloat(y,i), getFloat(z,i));
        }

        ret.normals.reserve(vertices.num);
        auto& nx = vertices.findProp("nx"); expect(nx.type == "float");
        auto& ny = vertices.findProp("ny"); expect(ny.type == "float");
        auto& nz = vertices.findProp("nz"); expect(nz.type == "float");
        for(int i = 0; i < vertices.num; ++i) {
                ret.normals.emplace_back(getFloat(nx,i), getFloat(ny,i), getFloat(nz,i));
        }
        ret.m_hasNormals = true;

        ret.texCoords.reserve(vertices.num);
        const Property* s;
        const Property* t;
        if ((s = vertices.hasProp("s")) && s->type == "float"
            && (t = vertices.hasProp("t")) && t->type == "float")
        {
                for(int i = 0; i < vertices.num; ++i) {
                        ret.texCoords.emplace_back(getFloat(*s,i), getFloat(*t,i));
                }
                ret.m_hasTexCoords = true;
        }

        auto& faces = findElem("face");
        auto& indices = faces.findProp("vertex_indices");
        expect(indices.type == "list");
        expect(indices.listNumType == "uchar");
        expect(indices.listElemType == "uint");

        auto curByte = indices.data.begin();
        while(curByte != indices.data.end()) {
                byte length = *curByte;
                ++curByte;
                if (length == 3) {
                        ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                        ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                        ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                } else { expect(length == 4);
                        // ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                        // ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                        // ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                        // ret.indices.push_back(ret.indices.back());
                        // ret.indices.push_back(*(uint*)(&*curByte)); curByte += sizeof(uint);
                        // ret.indices.push_back(ret.indices[ret.indices.size() - 5]);
                        float diag1 = glm::distance(
                                ret.vertices[(*(uint*)(&*curByte + sizeof(uint)*0))],
                                ret.vertices[(*(uint*)(&*curByte + sizeof(uint)*2))]
                        );
                        float diag2 = glm::distance(
                                ret.vertices[(*(uint*)(&*curByte + sizeof(uint)*1))],
                                ret.vertices[(*(uint*)(&*curByte + sizeof(uint)*3))]
                        );
                        if (diag1 < diag2) {
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*0)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*1)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*2)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*2)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*3)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*0)));
                        } else {
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*0)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*1)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*3)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*1)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*2)));
                                ret.indices.push_back((*(uint*)(&*curByte + sizeof(uint)*3)));
                        }
                        curByte += sizeof(uint)*4;
                }
        }

        return ret;
}

Geometry Geometry::fromObj(const std::string& filename) {
        std::ifstream in(filename, std::ios::in);
        std::vector<glm::vec3> vertices;
        std::vector<index_type> elements;

        std::string line;
        while(getline(in, line)) {
                if (line.substr(0, 2) == "v ") {
                        std::istringstream data(line.substr(2));
                        glm::vec3 vert;
                        data >> vert.x; data >> vert.y; data >> vert.z;
                        vertices.push_back(vert);
                }
                else if (line.substr(0, 2) == "f ") {
                        std::istringstream data(line.substr(2));
                        index_type idx;
                        data >> idx; elements.push_back(idx - 1);
                        data >> idx; elements.push_back(idx - 1);
                        data >> idx; elements.push_back(idx - 1);
                }
        }

        Geometry ret;

        ret.vertices = std::move(vertices);
        ret.indices = std::move(elements);
        ret.calculateNormals();

        return ret;
}

Geometry Geometry::fromUnindexed(const UnindexedGeometry& uiGeo) {
        Geometry geo;
        geo.vertices = uiGeo.vertices;
        if (uiGeo.hasNormals()) {
                geo.m_hasNormals = true;
                geo.normals = uiGeo.normals;
        }
        if (uiGeo.hasTexCoords())  {
                geo.m_hasTexCoords = true;
                geo.texCoords = uiGeo.texCoords;
        }
        if (uiGeo.hasTangents()) {
                geo.m_hasTangents = true;
                geo.tangents = uiGeo.tangents;
                geo.bitangents = uiGeo.bitangents;
        }
        geo.indices.reserve(geo.vertices.size());
        for (int i = 0; i < geo.vertices.size(); i++) {
                geo.indices.push_back(i);
        }
        return geo;
}

Geometry Geometry::fromUnindexed(UnindexedGeometry&& uiGeo) {
        Geometry geo;
        geo.vertices = std::move(uiGeo.vertices);
        if (uiGeo.hasNormals()) {
                geo.m_hasNormals = true;
                geo.normals = std::move(uiGeo.normals);
        }
        if (uiGeo.hasTexCoords()) {
                geo.m_hasTexCoords = true;
                geo.texCoords = std::move(uiGeo.texCoords);
        }
        if (uiGeo.hasTangents()) {
                geo.m_hasTangents = true;
                geo.tangents = std::move(uiGeo.tangents);
                geo.bitangents = std::move(uiGeo.bitangents);
        }
        geo.indices.reserve(geo.vertices.size());
        for (int i = 0; i < geo.vertices.size(); i++) {
                geo.indices.push_back(i);
        }
        return geo;
}
