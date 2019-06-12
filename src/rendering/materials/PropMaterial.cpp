#include "PropMaterial.h"
#include "Engine.h"

PropMaterial::PropMaterial(bool showNormals)
    : Material{{"shaders/propVS.glsl"},
     (showNormals ? std::vector<std::string>{"shaders/propGS.glsl"} : std::vector<std::string>{}),
     {"shaders/propFS.glsl"}}
{
	mColorLocation = shader.getLocationOf("color");

    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void PropMaterial::use()
{
    shader.use();
    shader.setVec3(mColorLocation, color);
}
