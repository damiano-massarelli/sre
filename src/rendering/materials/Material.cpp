#include "Material.h"

Material::Material(const std::string& vertexShader, const std::string& fragmentShader) 
	: shader{ Shader::loadFromFile( { vertexShader }, std::vector<std::string>{}, { fragmentShader }) }
{

}

Material::Material(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
 : shader{ Shader::loadFromFile(vertexShader, geometryShader, fragmentShader) }
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& geometryShaders, const std::vector<std::string>& fragmentShaders)
    : shader{ Shader::loadFromFile(vertexShaders, geometryShaders, fragmentShaders) }
{

}

Material::~Material()
{

}
