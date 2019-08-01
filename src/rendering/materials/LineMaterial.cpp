#include "LineMaterial.h"

LineMaterial::LineMaterial() : Material{"shaders/lineVS.glsl", "shaders/lineFS.glsl"}
{
	unSupportedRenderPhases = RenderPhase::ALL & ~RenderPhase::FORWARD_RENDERING;

}

void LineMaterial::use()
{
    glLineWidth(2.5f);
    shader.use();
}

void LineMaterial::after()
{
    glLineWidth(1.0f);
}

LineMaterial::~LineMaterial()
{
    //dtor
}
