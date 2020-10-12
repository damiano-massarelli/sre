#ifndef SHADER_H
#define SHADER_H
#include "resourceManagment/RefCount.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

class Shader {
public:
    RefCount refCount;
    static const char* GLSL_VERSION_STRING;

private:
    static std::map<std::string, Shader> mShaderCache;

    static GLuint mInUse;

    GLuint mProgramId = 0;

    static std::uint32_t createShaderFromFiles(
        const std::vector<std::string>& paths, GLenum type, bool addVersion = true);

    static std::uint32_t createShader(
        const std::vector<std::string>& code, GLenum type, GLint& success, bool addVersion = true);

    static std::uint32_t createProgram(
        std::uint32_t vertexShader, std::uint32_t geometryShader, std::uint32_t fragmentShader, GLint& success);

    static void writeDebugShaderToFile(const std::string& source);

    void cleanUpIfNeeded();

    Shader(std::uint32_t programId);

public:
    static Shader loadFromFile(const std::vector<std::string>& vertexPaths,
        const std::vector<std::string>& geometryPaths,
        const std::vector<std::string>& fragmentPaths,
        bool cache = true);

    static Shader loadFromFile(
        const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath);

    static std::string sourceFromFile(const std::string& path);

    static Shader fromCode(const std::vector<std::string>& vertexCode,
        const std::vector<std::string>& geometryCode,
        const std::vector<std::string>& fragmentCode);

    Shader();

    Shader& operator=(const Shader& rhs);

    std::int32_t getLocationOf(const std::string& name, bool warning = true) const;

    void setFloat(const std::string& name, float value) const;

    void setFloat(std::int32_t location, float value) const;

    void setInt(const std::string& name, int value) const;

    void setInt(std::int32_t location, int value) const;

    void setMat3(const std::string& name, const glm::mat3& value) const;

    void setMat3(std::int32_t location, const glm::mat3& value) const;

    void setMat4(const std::string& name, const glm::mat4& value) const;

    void setMat4(std::int32_t location, const glm::mat4& value) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;

    void setVec3(std::int32_t location, const glm::vec3& value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;

    void setVec2(std::int32_t location, const glm::vec2& value) const;

    void setMat4Array(const std::string& name, const std::vector<glm::mat4>& array) const;

    void setMat4Array(std::int32_t location, const std::vector<glm::mat4>& array) const;

    void setVec3Array(std::int32_t location, const std::vector<glm::vec3>& array) const;

    void setVec3Array(const std::string& name, const std::vector<glm::vec3>& array) const;

    void bindUniformBlock(const std::string& name, std::uint32_t bindingPoint);

    /**
     * Sets this shader as the current one.
     */
    void use();

    /**
     * This shader will no longer be the one in use.
     */
    void stop() const;

    bool isInUse() const;

    operator bool() const;

    bool operator==(const Shader& other);

    bool isValid() const;

    GLuint getId() const;

    virtual ~Shader();
};

/**
 * Controls the usage of Shader in a scope.
 * Shader::use is automatically called when an object of this class
 * is created. Shader::stop is then called when that objects goes out
 * of scope.
 */
class ShaderScopedUsage {
private:
    Shader mShader;

public:
    ShaderScopedUsage(Shader& shader);

    ~ShaderScopedUsage();
};

#endif  // SHADER_H
