#include "Mesh.h"
#include <glad/glad.h>

Mesh::Mesh(std::uint32_t vao) : mVao{vao}
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
    for (auto& buffer : mBuffers)
        glDeleteBuffers(1, &buffer);
	mBuffers.clear();

    glDeleteVertexArrays(1, &mVao);
    mVao = 0;
}

std::uint32_t Mesh::getVao() const
{
	return mVao;
}

std::uint32_t Mesh::getEbo() const
{
	return mEbo;
}

Mesh::~Mesh()
{
	if (refCount.shouldCleanUp())
		cleanUp();
}
