#include "Material.h"

Material::Material(const std::string& vertexShader, const std::string& fragmentShader) : shader{vertexShader, fragmentShader}
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& fragmentShaders) :
    shader{vertexShaders, fragmentShaders}
{

}

Material::Material(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
 : shader{vertexShader, geometryShader, fragmentShader}
{

}

Material::Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& geometryShaders, const std::vector<std::string>& fragmentShaders)
    : shader{vertexShaders, geometryShaders, fragmentShaders}
{

}

Material::~Material()
{

}
