#include "Material.h"

Material::Material(const std::string& vertexShader, const std::string& fragmentShader) 
	: Material{std::vector<std::string>{vertexShader}, {}, {fragmentShader}}
{

}

Material::Material(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
	: Material{std::vector<std::string>{vertexShader}, {geometryShader}, {}}
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& geometryShaders, const std::vector<std::string>& fragmentShaders)
    : shader{ Shader::loadFromFile(vertexShaders, geometryShaders, fragmentShaders) }
{
	mModelLocation = shader.getLocationOf("model");
}

std::uint32_t Material::getModelLocation() const
{
	return mModelLocation;
}

Material::~Material()
{

}
