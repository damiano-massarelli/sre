#include "rendering/materials/Material.h"
#include <functional>

Material::Material(const std::string& vertexShader, const std::string& fragmentShader) 
	: Material{std::vector<std::string>{vertexShader}, {}, {fragmentShader}}
{

}

Material::Material(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
	: Material{std::vector<std::string>{vertexShader}, {geometryShader}, {fragmentShader}}
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& geometryShaders, const std::vector<std::string>& fragmentShaders)
    : shader{ Shader::loadFromFile(vertexShaders, geometryShaders, fragmentShaders) }
{
	mModelLocation = shader.getLocationOf("model");

	mNormalModelLocation = shader.getLocationOf("normalModel", false);
}

std::int32_t Material::getModelLocation() const
{
	return mModelLocation;
}

std::int32_t Material::getNormalModelLocation() const
{
	return mNormalModelLocation;
}

std::size_t Material::hash() const
{
	return shader.getId() + static_cast<int>(isTwoSided) + unSupportedRenderPhases;
}

bool Material::equalsTo(const Material* rhs) const
{
	return shader.getId() == rhs->shader.getId()
		&& isTwoSided == rhs->isTwoSided
		&& unSupportedRenderPhases == rhs->unSupportedRenderPhases;
}

Material::~Material()
{

}


