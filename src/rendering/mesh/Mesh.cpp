#include "Mesh.h"
#include <glad/glad.h>

Mesh::Mesh(std::uint32_t vao, std::uint32_t vbo, std::uint32_t ebo) : mVao{vao}, mVbo{vbo}, mEbo{ebo}
{

}

const std::vector<Vertex>& Mesh::getVertexData() const
{
    return mVertexData;
}

bool Mesh::hasVertexData() const
{
    return mHasVertexData;
}

void Mesh::cleanUp()
{
    glDeleteBuffers(1, &mVbo);
    mVbo = 0;
    glDeleteBuffers(1, &mEbo);
    mEbo = 0;
    glDeleteVertexArrays(1, &mVao);
    mVao = 0;
}
