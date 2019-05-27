#include "Material.h"

Material::Material(const std::string& vertexShader, const std::string& fragmentShader) 
	: shader{Shader::load(vertexShader, fragmentShader)}
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& fragmentShaders) :
    shader{Shader::load(vertexShaders, fragmentShaders)}
{

}

Material::Material(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
 : shader{Shader::load(vertexShader, geometryShader, fragmentShader)}
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& geometryShaders, const std::vector<std::string>& fragmentShaders)
    : shader{Shader::load(vertexShaders, geometryShaders, fragmentShaders)}
{

}

Material::~Material()
{

}
