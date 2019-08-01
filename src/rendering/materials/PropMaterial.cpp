#include "PropMaterial.h"
#include "Engine.h"
#include <glm/gtx/hash.hpp>

PropMaterial::PropMaterial(bool showNormals)
    : Material{{"shaders/propVS.glsl"},
     (showNormals ? std::vector<std::string>{"shaders/propGS.glsl"} : std::vector<std::string>{}),
     {"shaders/propFS.glsl"}}
{
	unSupportedRenderPhases = RenderPhase::ALL & ~RenderPhase::FORWARD_RENDERING;

	mColorLocation = shader.getLocationOf("color");

    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void PropMaterial::use()
{
    shader.use();
    shader.setVec3(mColorLocation, color);
}

std::size_t PropMaterial::hash() const
{
	return std::hash<glm::vec3>{}(color);
}

bool PropMaterial::equalsTo(const Material* rhs) const
{
	if (shader.getId() != rhs->shader.getId())
		return false;

	auto other = static_cast<const PropMaterial*>(rhs);
	
	return Material::equalsTo(rhs)
		&& color == other->color;
}
