#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <iterator>
#include <algorithm>

std::map<std::string, Shader> Shader::mShaderCache;

// to print path vectors easily
std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& vec) {
    out << "[";
	if (vec.size() != 0) {
		std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<std::string>(out, ", "));
		out << *(vec.end() - 1);
	}
    out << "]";
    return out;
}

Shader Shader::load(const std::vector<std::string>& vertexPaths,
	const std::vector<std::string>& geometryPaths,
	const std::vector<std::string>& fragmentPaths) {

	std::stringstream cacheName;
	cacheName << vertexPaths << fragmentPaths << geometryPaths;
	std::string cacheKey = cacheName.str();
	
	// cache checks
	auto cachedShader = mShaderCache.find(cacheKey);
	if (cachedShader != mShaderCache.end()) {
		std::cout << "found in cache shader\n";
		return cachedShader->second;
	}

	Shader shader{ vertexPaths, geometryPaths, fragmentPaths };
	shader.refCount.onRemove = [cacheKey]() { std::cout << "shader removed\n"; Shader::mShaderCache.erase(cacheKey); };

	mShaderCache[cacheKey] = shader;
	shader.refCount.decrease(); // weak ref

	return shader;
}

Shader Shader::load(const std::vector<std::string>& vertexPaths, const std::vector<std::string>& fragmentPaths)
{
	return load(vertexPaths, {}, fragmentPaths);
}

Shader Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
	return load(std::vector<std::string>{ vertexPath }, std::vector<std::string>{}, std::vector<std::string>{ fragmentPath });
}

Shader Shader::load(const std::string & vertexPath, const std::string & geometryPath, const std::string & fragmentPath)
{
	return load(std::vector<std::string>{ vertexPath }, std::vector<std::string>{ geometryPath }, std::vector<std::string>{ fragmentPath });
}

Shader::Shader() : programId{0}
{
}



Shader::Shader(const std::vector<std::string>& vertexPaths, const std::vector<std::string>& geometryPaths, const std::vector<std::string>& fragmentPaths)
{
    std::uint32_t vertexShader = createShader(vertexPaths, GL_VERTEX_SHADER);
    std::uint32_t fragmentShader = createShader(fragmentPaths, GL_FRAGMENT_SHADER);
    std::uint32_t geometryShader = 0;
    if (geometryPaths.size() != 0)
        geometryShader = createShader(geometryPaths, GL_GEOMETRY_SHADER);

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    if (geometryShader != 0) glAttachShader(programId, geometryShader);
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

    if (geometryShader != 0) glDeleteShader(geometryShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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

void Shader::setFloat(std::int32_t location, float value) const
{
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

void Shader::setInt(std::int32_t location, int value) const
{
	glUniform1i(location, value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    std::int32_t location = getLocationOf(name);
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(std::int32_t location, const glm::mat4 & value) const
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    std::int32_t location = getLocationOf(name);
    if (location != -1)
        glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec3(std::int32_t location, const glm::vec3 & value) const
{
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::use() const
{
    glUseProgram(programId);
}

Shader::~Shader()
{
	if (refCount.shouldCleanUp() && programId != 0)
		glDeleteProgram(programId);
}


