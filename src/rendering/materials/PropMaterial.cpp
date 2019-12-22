#include "rendering/materials/PropMaterial.h"
#include "Engine.h"
#include <glm/gtx/hash.hpp>

PropMaterial::PropMaterial(bool wireframe, bool showNormals)
    : Material{{"shaders/propVS.glsl"},
     (showNormals ? std::vector<std::string>{"shaders/propGS.glsl"} : std::vector<std::string>{}),
     {"shaders/propFS.glsl"}},
	mWireframe{ wireframe }
{
	unSupportedRenderPhases = RenderPhase::ALL & ~RenderPhase::FORWARD_RENDERING;

	mColorLocation = shader.getLocationOf("color");

    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void PropMaterial::use()
{
	if (mWireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
    shader.use();
    shader.setVec3(mColorLocation, color);
}

void PropMaterial::after()
{
	if (mWireframe) {
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

std::size_t PropMaterial::hash() const
{
	return std::hash<glm::vec3>{}(color)
		+ static_cast<std::int32_t>(mWireframe);
}

bool PropMaterial::equalsTo(const Material* rhs) const
{
	if (shader.getId() != rhs->shader.getId())
		return false;

	auto other = static_cast<const PropMaterial*>(rhs);
	
	return Material::equalsTo(rhs)
		&& color == other->color
		&& mWireframe == other->mWireframe;
}
