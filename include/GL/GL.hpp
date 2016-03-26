#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include <array>

using uint = unsigned int;

namespace GL {

namespace detail {

template<typename T>
struct param_storage {
        using type = T;
};

template<>
struct param_storage<bool> {
        using type = int;
};

#define INTARRAY(N) std::array<int, N>
//X-macro: Lists the type of all return values from
#define GL_PNAME_TYPES(X) \
        X(GL_DELETE_STATUS, bool) \
        X(GL_LINK_STATUS, bool) \
        X(GL_VALIDATE_STATUS, bool) \
        X(GL_INFO_LOG_LENGTH, int) \
        X(GL_ATTACHED_SHADERS, int) \
        X(GL_ACTIVE_ATOMIC_COUNTER_BUFFERS, int) \
        X(GL_ACTIVE_ATTRIBUTES, int) \
        X(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, int) \
        X(GL_ACTIVE_UNIFORMS, int) \
        X(GL_ACTIVE_UNIFORM_MAX_LENGTH, int) \
        X(GL_PROGRAM_BINARY_LENGTH, int) \
        X(GL_COMPUTE_WORK_GROUP_SIZE, INTARRAY(3)) \
        X(GL_TRANSFORM_FEEDBACK_BUFFER_MODE, GLenum) \
        X(GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, int) \
        X(GL_GEOMETRY_VERTICES_OUT, int) \
        X(GL_GEOMETRY_INPUT_TYPE, GLenum) \
        X(GL_GEOMETRY_OUTPUT_TYPE, GLenum) \

;

template<GLenum pname>
struct program_param_info;

#define GL_PNAME_TYPE_UNWRAP(SYMBOL, TYPE) \
        template<> \
        struct program_param_info<SYMBOL> { \
                using return_type = TYPE; \
                using storage_type = typename param_storage<TYPE>::type; \
        }; \

GL_PNAME_TYPES(GL_PNAME_TYPE_UNWRAP)

#undef GL_PNAME_TYPE_UNWRAP
#undef GL_PNAME_TYPES
#undef INTARRAY



#define GL_PNAME_TYPES(X) \
        X(GL_SHADER_TYPE, GLenum) \
        X(GL_DELETE_STATUS, bool) \
        X(GL_COMPILE_STATUS, bool) \
        X(GL_INFO_LOG_LENGTH, int) \
        X(GL_SHADER_SOURCE_LENGTH, int) \

;

template<GLenum pname>
struct shader_param_info;

#define GL_PNAME_TYPE_UNWRAP(SYMBOL, TYPE) \
        template<> \
        struct shader_param_info<SYMBOL> { \
                using return_type = TYPE; \
                using storage_type = typename param_storage<TYPE>::type; \
        }; \

GL_PNAME_TYPES(GL_PNAME_TYPE_UNWRAP)

#undef GL_PNAME_TYPE_UNWRAP
#undef GL_PNAME_TYPES

//TYPE, ENUM, NORMALIZABLE
#define GL_TYPE_INFO(X) \
        X(GLbyte, GL_BYTE, true) \
        X(GLshort, GL_SHORT, true) \
        X(GLint, GL_INT, true) \
        X(GLfloat, GL_FLOAT, false) \
        X(GLdouble, GL_DOUBLE, false) \
 \
        X(GLubyte, GL_UNSIGNED_BYTE, true) \
        X(GLushort, GL_UNSIGNED_SHORT, true) \
        X(GLuint, GL_UNSIGNED_INT, true) \

;

template<typename T>
struct type_info;

template<typename T, GLint N>
struct type_info<T[N]> : type_info<T> {
        static constexpr GLint size = N;
};

template<typename T, glm::precision P>
struct type_info<glm::tvec1<T, P>> : type_info<T> {
        static constexpr GLint size = 1;
};

template<typename T, glm::precision P>
struct type_info<glm::tvec2<T, P>> : type_info<T> {
        static constexpr GLint size = 2;
};

template<typename T, glm::precision P>
struct type_info<glm::tvec3<T, P>> : type_info<T> {
        static constexpr GLint size = 3;
};

template<typename T, glm::precision P>
struct type_info<glm::tvec4<T, P>> : type_info<T> {
        static constexpr GLint size = 4;
};

#define GL_TYPE_INFO_UNWRAP(TYPE, ENUM, NORMALIZABLE) \
template<> \
struct type_info<TYPE> { \
        static constexpr GLenum type = ENUM; \
        static constexpr GLint size = 1; \
        static constexpr bool normalizable = NORMALIZABLE; \
};

GL_TYPE_INFO(GL_TYPE_INFO_UNWRAP)

#undef GL_TYPE_INFO_UNWRAP
#undef GL_TYPE_INFO


#define GL_UNIFORM_INFO(X) \
        X(float, glUniform1fv, false) \
        X(glm::vec2, glUniform2fv, false) \
        X(glm::vec3, glUniform3fv, false) \
        X(glm::vec4, glUniform4fv, false) \
        X(int, glUniform1iv, false) \
        X(glm::ivec2, glUniform2iv, false) \
        X(glm::ivec3, glUniform3iv, false) \
        X(glm::ivec4, glUniform4iv, false) \
        X(uint, glUniform1uiv, false) \
        X(glm::uvec2, glUniform2uiv, false) \
        X(glm::uvec3, glUniform3uiv, false) \
        X(glm::uvec4, glUniform4uiv, false) \
        X(glm::mat2, glUniformMatrix2fv, true) \
        X(glm::mat3, glUniformMatrix3fv, true) \
        X(glm::mat4, glUniformMatrix4fv, true) \

;

template<typename T>
struct vec_uniform_info_base {
        static auto value_ptr(const T& data) { return glm::value_ptr(data); }
};

template<typename T>
struct uniform_info_base {
        static auto value_ptr(const T& data) { return &data; }
};

template<typename T, glm::precision P>
struct uniform_info_base<glm::tvec1<T, P> > : vec_uniform_info_base<glm::tvec1<T, P> > {};
template<typename T, glm::precision P>
struct uniform_info_base<glm::tvec2<T, P> > : vec_uniform_info_base<glm::tvec2<T, P> > {};
template<typename T, glm::precision P>
struct uniform_info_base<glm::tvec3<T, P> > : vec_uniform_info_base<glm::tvec3<T, P> > {};
template<typename T, glm::precision P>
struct uniform_info_base<glm::tvec4<T, P> > : vec_uniform_info_base<glm::tvec4<T, P> > {};

template<typename T>
struct uniform_info;

#define GL_UNIFORM_INFO_UNWRAP(TYPE, FUNC, MATRIX) \
        template<> \
        struct uniform_info<TYPE> : uniform_info_base<TYPE> { \
                static constexpr auto getFunc() { \
                        return &FUNC; \
                } \
                static constexpr bool isMatrix = MATRIX; \
        }; \
;

GL_UNIFORM_INFO(GL_UNIFORM_INFO_UNWRAP)

#undef GL_UNIFORM_INFO_UNWRAP
#undef GL_UNIFORM_INFO

constexpr GLenum getDefaultFormat(GLenum internalFormat) {
        if (internalFormat == GL_DEPTH_COMPONENT
            || internalFormat == GL_DEPTH_COMPONENT16
            || internalFormat == GL_DEPTH_COMPONENT24
            || internalFormat == GL_DEPTH_COMPONENT32F
        ) {
                return GL_DEPTH_COMPONENT;
        }
        return GL_RED;
}

}; // namespace detail

template<typename T>
struct wrapper_traits;

template<typename T>
class Wrapper {
protected:
        using traits = wrapper_traits<T>;
        using handle_type = typename traits::handle_type;
        handle_type handle;
public:
        Wrapper(): handle(0) {}
        Wrapper(handle_type handle): handle(handle) {}

        ~Wrapper() {
                traits::release(handle);
        }

        Wrapper(const Wrapper& rhs) = delete;
        Wrapper(Wrapper&& rhs) noexcept {
                handle = rhs.handle;
                rhs.handle = 0;
        }

        Wrapper& operator=(Wrapper&& rhs) {
                traits::release(handle);
                handle = rhs.handle;
                rhs.handle = 0;
                return *this;
        }

        handle_type& operator()() {
                return handle;
        }

        const handle_type& operator()() const {
                return handle;
        }
};

class Buffer;

template<>
struct wrapper_traits<Buffer> {
        using handle_type = GLuint;
        static void release(const handle_type& handle) {
                ::glDeleteBuffers(1, &handle);
        }
};

class Buffer : public Wrapper<Buffer> {
public:
        Buffer() {
                ::glGenBuffers(1, &handle);
        }

        void bind() const {
                ::glBindBuffer(GL_ARRAY_BUFFER, handle);
        }

        void bindElems() const {
                ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
        }

        template<typename T>
        void assign(size_t num, const T* data, GLenum usage) {
                bind();
                ::glBufferData(GL_ARRAY_BUFFER, num*sizeof(T), data, usage);
        }

        template<typename T, typename A>
        void assign(const std::vector<T, A>& data, GLenum usage) {
                assign(data.size(), &data[0], usage);
        }

        template<typename T, typename A>
        Buffer(const std::vector<T, A>& data, GLenum usage): Buffer() {
                assign(data, usage);
        }
};

class Texture;

template<>
struct wrapper_traits<Texture> {
        using handle_type = GLuint;
        static void release(const handle_type& handle) {
                ::glDeleteTextures(1, &handle);
        }
};

class Texture : public Wrapper<Texture> {
public:
        Texture() {
                ::glGenTextures(1, &handle);
        }
};

class Texture2D : public Texture {
public:
        void bind() const {
                ::glBindTexture(GL_TEXTURE_2D, handle);
        }

        void assign(GLint level, GLint internalFormat, GLsizei width,
                    GLsizei height, GLenum format, GLenum type, const GLvoid* data)
        {
                bind();
                ::glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, format, type, data);
        }

        void assign(GLint level, GLint internalFormat, GLsizei width, GLsizei height)
        {
                assign(level, internalFormat, width, height, detail::getDefaultFormat(internalFormat), GL_UNSIGNED_BYTE, nullptr);
        }

        template<typename T>
        void assign(GLint level, GLint internalFormat, GLsizei width,
                    GLsizei height, GLenum format, const T* data)
        {
                GLenum type = detail::type_info<T>::type;
                assign(level, internalFormat, width, height, format, type, data);
        }

        template<typename T, glm::precision P>
        void assign(GLint level, GLint internalFormat, GLsizei width,
                    GLsizei height, const glm::tvec3<T,P>* data) {
                assign(level, internalFormat, width, height, GL_RGB, data);
        }

        template<typename T, glm::precision P>
        void assign(GLint level, GLint internalFormat, GLsizei width,
                    GLsizei height, const glm::tvec4<T,P>* data) {
                assign(level, internalFormat, width, height, GL_RGBA, data);
        }

        void generateMipmap() {
                bind();
                ::glGenerateMipmap(GL_TEXTURE_2D);
        }
};

class Texture2DArray : public Texture {
public:
        void bind() {
                ::glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
        }
        void assign(GLint level, GLint internalFormat, GLsizei width,
                    GLsizei height, GLsizei depth, GLenum format, GLenum type,
                    const GLvoid* data)
        {
                bind();
                ::glTexImage3D(GL_TEXTURE_2D_ARRAY, level, internalFormat, width, height, depth, 0, format, type, data);
        }
        void assign(GLint level, GLint internalFormat, GLsizei width,
                    GLsizei height, GLsizei depth)
        {
                assign(level, internalFormat, width, height, depth, detail::getDefaultFormat(internalFormat), GL_UNSIGNED_BYTE, nullptr);
        }
};

class TextureBuffer : public Texture {
public:
        void bind() {
                ::glBindTexture(GL_TEXTURE_BUFFER, handle);
        }
        void assign(GLint internalFormat, const GL::Buffer& buff)
        {
                bind();
                ::glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buff());
        }
};

class Framebuffer;

template<>
struct wrapper_traits<Framebuffer> {
        using handle_type = GLuint;
        static void release(const handle_type& handle) {
                ::glDeleteFramebuffers(1, &handle);
        }
};

class Framebuffer : public Wrapper<Framebuffer> {
public:
        Framebuffer() {
                ::glGenFramebuffers(1, &handle);
        }

        void bind() {
                ::glBindFramebuffer(GL_FRAMEBUFFER, handle);
        }

        static void unbind() {
                ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void attachTexture(GLenum attachment, const Texture& tex, GLint level) {
                bind();
                ::glFramebufferTexture(GL_FRAMEBUFFER, attachment, tex(), level);
        }

        void attachTextureLayer(GLenum attachment, const Texture2DArray& texture, GLint level, GLint layer) {
                bind();
                ::glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture(), level, layer);
        }
};


class VertexArray;

template<>
struct wrapper_traits<VertexArray> {
        using handle_type = GLuint;
        static void release(const handle_type& handle) {
                ::glDeleteVertexArrays(1, &handle);
        }
};

class VertexArray : public Wrapper<VertexArray> {
public:
        VertexArray() {
                ::glGenVertexArrays(1, &handle);
        }

        void bind() {
                ::glBindVertexArray(handle);
        }

        void vertexAttribFormat(GLuint attribIndex, GLint size, GLenum type,
                                GLboolean normalized, GLuint relativeOffset)
        {
                bind();
                ::glVertexAttribFormat(
                        attribIndex, size, type, normalized, relativeOffset
                );
        }

        template<typename T>
        void vertexAttribFormat(GLuint attribIndex, GLboolean normalized,
                                GLuint relativeOffset)
        {
                using info = detail::type_info<T>;
                vertexAttribFormat(attribIndex, info::size, info::type,
                                   normalized, relativeOffset);
        }

        template<typename T>
        std::enable_if_t<!detail::type_info<T>::normalizable,
        void> vertexAttribFormat(GLuint attribIndex, GLuint relativeOffset)
        {
                vertexAttribFormat<T>(attribIndex, GL_FALSE, relativeOffset);
        }

        void vertexAttribBinding(GLuint attribIndex, GLuint bindingIndex) {
                bind();
                ::glVertexAttribBinding(attribIndex, bindingIndex);
        }

        void bindVertexBuffer(GLuint bindingIndex, GLuint buffer,
                              GLintptr offset, GLintptr stride)
        {
                bind();
                ::glBindVertexBuffer(bindingIndex, buffer, offset, stride);
        }

        void bindVertexBuffer(GLuint bindingIndex, const Buffer& buffer,
                              GLintptr offset, GLintptr stride)
        {
                bindVertexBuffer(bindingIndex, buffer(), offset, stride);
        }

        template<typename T>
        void bindVertexBuffer(GLuint bindingIndex, const Buffer& buffer,
                              GLintptr offset)
        {
                bindVertexBuffer(bindingIndex, buffer, offset, sizeof(T));
        }

        void enableVertexAttrib(GLuint index) {
                bind();
                ::glEnableVertexAttribArray(index);
        }

        void disableVertexAttrib(GLuint index) {
                bind();
                ::glDisableVertexAttribArray(index);
        }
};

class Program;
class Shader;

template<>
struct wrapper_traits<Program> {
        using handle_type = GLuint;
        static void release(const handle_type& handle) {
                ::glDeleteProgram(handle);
        }
};

class Program : public Wrapper<Program> {
public:
        Program() {
                handle = ::glCreateProgram();
        }

        void get(GLenum pname, GLint* params) const {
                ::glGetProgramiv(handle, pname, params);
        }

        template<GLenum pname>
        typename detail::program_param_info<pname>::return_type
        get() const {
                typename detail::program_param_info<pname>::storage_type param;
                get(pname, reinterpret_cast<int*>(&param));
                return param;
        }

        void getInfoLog(GLsizei maxLength, GLsizei* length, GLchar* infoLog) const {
                ::glGetProgramInfoLog(handle, maxLength, length, infoLog);
        }

        std::string getInfoLog() const {
                auto length = get<GL_INFO_LOG_LENGTH>();
                std::string infoLog;
                infoLog.resize(length);
                getInfoLog(length, nullptr, &infoLog[0]);
                return infoLog;
        }

        void attachShader(GLuint shader) {
                ::glAttachShader(handle, shader);
        }

        void attachShader(const Shader& shader);

        void link() {
                ::glLinkProgram(handle);
                if (!get<GL_LINK_STATUS>()) {
                        throw std::runtime_error(getInfoLog());
                }
        }

        void use() {
                ::glUseProgram(handle);
        }

        GLint attrib(const char* name) {
                return ::glGetAttribLocation(handle, name);
        }

        GLint getUniformLocation(const char* name) {
                return ::glGetUniformLocation(handle, name);
        }

        template<typename T>
        std::enable_if_t<!detail::uniform_info<T>::isMatrix,
        void> uniform(const char* name, const T& data) {
                using uniform_info = detail::uniform_info<T>;
                use();
                (*uniform_info::getFunc())(getUniformLocation(name), 1, uniform_info::value_ptr(data));
        }

        template<typename T>
        std::enable_if_t<detail::uniform_info<T>::isMatrix,
        void> uniform(const char* name, const T& data) {
                using uniform_info = detail::uniform_info<T>;
                use();
                (*uniform_info::getFunc())(getUniformLocation(name), 1, GL_FALSE, &data[0][0]);
        }

        template<typename T, std::size_t N>
        void uniform(const char* name, const T (&data)[N]) {
                using uniform_info = detail::uniform_info<T>;
                use();
                (*uniform_info::getFunc())(getUniformLocation(name), N, uniform_info::value_ptr(data));
        }
};

template<>
struct wrapper_traits<Shader> {
        using handle_type = GLuint;
        static void release(const handle_type& handle) {
                ::glDeleteShader(handle);
        }
};

class Shader : public Wrapper<Shader> {
public:
        Shader(GLenum type) {
                handle = ::glCreateShader(type);
        }

        void get(GLenum pname, GLint* params) const {
                ::glGetShaderiv(handle, pname, params);
        }

        template<GLenum pname>
        typename detail::shader_param_info<pname>::return_type
        get() const {
                typename detail::shader_param_info<pname>::storage_type param;
                get(pname, reinterpret_cast<int*>(&param));
                return param;
        }

        void getInfoLog(GLsizei maxLength, GLsizei* length, GLchar* infoLog) const {
                ::glGetShaderInfoLog(handle, maxLength, length, infoLog);
        }

        std::string getInfoLog() const {
                auto length = get<GL_INFO_LOG_LENGTH>();
                std::string infoLog;
                infoLog.resize(length);
                getInfoLog(length, nullptr, &infoLog[0]);
                return infoLog;
        }

        void source(GLsizei count, const GLchar** string, const GLint* length) {
                ::glShaderSource(handle, count, string, length);
        }

        void source(const GLchar* string, const GLint length) {
                source(1, &string, &length);
        }

        void source(const GLchar* string) {
                source(1, &string, nullptr);
        }

        void source(const std::string& string) {
                source(string.data(), string.size());
        }

        void source(const std::vector<std::string>& strings) {
                const GLchar* cstrs[strings.size()];
                GLint cstrlens[strings.size()];

                for (int i = 0; i < strings.size(); i++) {
                        cstrs[i] = &strings[i][0];
                        cstrlens[i] = strings[i].size();
                }

                source(strings.size(), cstrs, cstrlens);
        }

        void compile() {
                ::glCompileShader(handle);
                if (!get<GL_COMPILE_STATUS>()) {
                        throw std::runtime_error(getInfoLog());
                }
        }

        static Shader fromString(GLenum type, const std::string& string) {
                Shader shader{type};
                shader.source(string);
                shader.compile();
                return shader;
        }

        static Shader fromString(GLenum type, const char* string) {
                Shader shader{type};
                shader.source(string);
                shader.compile();
                return shader;
        }
};

inline void Program::attachShader(const Shader& shader) {
        attachShader(shader());
}

} //namespace GL
