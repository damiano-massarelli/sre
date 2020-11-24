#include "rendering/deferredRendering/DeferredLightShader.h"

void DeferredLightShader::init(const std::vector<std::string>& vertexShaders,
    const std::vector<std::string>& fragmentShaders,
    const std::vector<std::string>& bufferNames,
    const std::map<std::string, std::uint32_t>& uboBindings) {
    shader = Shader::loadFromFile(vertexShaders, {}, fragmentShaders);

    ShaderScopedUsage useShader{ shader };
    mLightIndexLocation = shader.getLocationOf("lightIndex");
    mLightRadiusLocation = shader.getLocationOf("lightRadius", false);

    int pos = 0;
    for (const auto& name : bufferNames)
        shader.setInt(name, pos++);

    for (const auto& [name, bindingPoint] : uboBindings)
        shader.bindUniformBlock(name, bindingPoint);
}

void DeferredLightShader::setLightIndex(int index) const {
    shader.setInt(mLightIndexLocation, index);
}

void DeferredLightShader::setLightRadius(float radius) const {
    shader.setFloat(mLightRadiusLocation, radius);
}

void DeferredLightShader::cleanUp() {
    shader = Shader();
}
