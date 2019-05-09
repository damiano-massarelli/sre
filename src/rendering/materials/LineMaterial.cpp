#include "LineMaterial.h"

LineMaterial::LineMaterial() : Material{"shaders/lineVS.glsl", "shaders/lineFS.glsl"}
{
    drawMode = GL_LINES;
}

void LineMaterial::use()
{
    glLineWidth(2.5f);
    shader.use();
}


LineMaterial::~LineMaterial()
{
    //dtor
}
