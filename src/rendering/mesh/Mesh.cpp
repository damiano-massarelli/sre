#include "rendering/mesh/Mesh.h"
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

Mesh& Mesh::operator=(const Mesh& rhs)
{
	if (mVao == rhs.mVao) return *this;

	cleanUpIfNeeded();

	mVao = rhs.mVao;
	mBuffers = rhs.mBuffers;

	mEbo = rhs.mEbo;

	mUsesIndices = rhs.mUsesIndices;

	mDrawMode = rhs.mDrawMode;

	mHasVertexData = rhs.mHasVertexData;
	mVertexData = rhs.mVertexData;

	mVertexNumber = rhs.mVertexNumber;
	mIndicesNumber = rhs.mIndicesNumber;

	refCount = rhs.refCount;

	return *this;
}

void Mesh::cleanUpIfNeeded()
{
	if (refCount.shouldCleanUp()) {
		for (auto& buffer : mBuffers)
			glDeleteBuffers(1, &buffer);
		mBuffers.clear();

		glDeleteVertexArrays(1, &mVao);
		mVao = 0;
	}
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
	cleanUpIfNeeded();
}
