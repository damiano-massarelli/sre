#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <iterator>
#include <algorithm>

// to print path vectors easily
std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& vec) {
    out << "[";
    std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<std::string>(out, ", "));
    out << *(vec.end() - 1);
    out << "]";
    return out;
}

Shader::Shader(const std::vector<std::string>& vertexPaths, const std::vector<std::string>& fragmentPaths)
{
    std::uint32_t vertexShader = createShader(vertexPaths, GL_VERTEX_SHADER);
    std::uint32_t fragmentShader = createShader(fragmentPaths, GL_FRAGMENT_SHADER);

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    int success;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        programId = 0;
        std::cerr << "linking problem " << vertexPaths << "-" << fragmentPaths << ": " << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : Shader{std::vector<std::string>{vertexPath}, std::vector<std::string>{fragmentPath}}
{

}

std::string Shader::sourceFromFile(const std::string& path)
{
    std::ifstream inputSource;
    inputSource.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::string source;
    try {
        inputSource.open(path);
        std::stringstream buffer;
        buffer << inputSource.rdbuf();

        source = buffer.str();
    } catch (...) {
        std::cerr << "cannot open " << path << "\n";
        return "";
    }

    return source;
}

void Shader::writeDebugShaderToFile(const std::string& source)
{
    std::ofstream debugFile;
    debugFile.open("debug.glsl");
    debugFile << source;

    debugFile.close();
}

std::uint32_t Shader::createShader(const std::vector<std::string>& paths, GLenum type, bool addVersion)
{
    std::uint32_t shader = glCreateShader(type);

    std::vector<const char*> sources;
    std::stringstream source;
    if (addVersion)
        source << "#version 420 core\n";

    for (std::size_t i = 0; i < paths.size(); ++i)
        source << "#line 1 " << i << "\n" << sourceFromFile(paths[i]);

    const std::string& sourceCode = source.str();
    const char* shaderSource = sourceCode.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "shader compilation error for " << paths << ":" << infoLog << "\n";
        writeDebugShaderToFile(source.str());
        return 0;
    }

    return shader;
}

std::int32_t Shader::getLocationOf(const std::string& name, bool warning) const
{
    std::int32_t location = glGetUniformLocation(programId, name.c_str());
    if (warning && location == -1)
        std::cerr << "unable to find uniform variable " << name << " make sure you are using it in your code\n";
    return location;
}

void Shader::setFloat(const std::string& name, float value) const
{
    std::int32_t location = getLocationOf(name);
    if (location != -1)
        glUniform1f(location, value);
}

void Shader::bindUniformBlock(const std::string& name, std::uint32_t bindingPoint)
{
    std::uint32_t index = glGetUniformBlockIndex(programId, name.c_str());
    if (index == GL_INVALID_INDEX)
        std::cerr << "unable to find uniform buffer " << name << "\n";

    glUniformBlockBinding(programId, index, bindingPoint);
}

void Shader::setInt(const std::string& name, int value) const
{
    std::int32_t location = getLocationOf(name);
    if (location != -1)
        glUniform1i(location, value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    std::int32_t location = getLocationOf(name);
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    std::int32_t location = getLocationOf(name);
    if (location != -1)
        glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::use() const
{
    glUseProgram(programId);
}

Shader::~Shader()
{
    glDeleteProgram(programId);
}


