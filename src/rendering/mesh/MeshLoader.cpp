#include "rendering/mesh/MeshLoader.h"
#include <glad/glad.h>

MeshLoader::MeshLoader(GLenum drawMode) : mDrawMode{drawMode}, mVao{0}
{
    glGenVertexArrays(1, &mMesh.mVao);
    glBindVertexArray(mMesh.mVao);
}


Mesh MeshLoader::getMesh(std::uint32_t vertexNumber, std::uint32_t indexNumber)
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mMesh.mVertexNumber = vertexNumber;
    mMesh.mIndicesNumber = indexNumber;
    mMesh.mUsesIndices = indexNumber != 0;

    return mMesh;
}

Mesh MeshLoader::createMesh(const float vertexData[], std::uint32_t numOfVertices, std::uint32_t indices[], std::uint32_t numOfIndices, GLenum drawMode)
{
    Mesh mesh{0};
    mesh.mDrawMode = drawMode;
    mesh.mVertexNumber = numOfVertices;
    mesh.mIndicesNumber = numOfIndices;

    glGenVertexArrays(1, &mesh.mVao);

    std::uint32_t vbo;
    glGenBuffers(1, &vbo);

    std::uint32_t ebo = 0;
    if (numOfIndices != 0)
        glGenBuffers(1, &ebo);

    glBindVertexArray(mesh.mVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 11 * numOfVertices * sizeof(float), static_cast<const void *>(vertexData), GL_STATIC_DRAW);

    if (numOfIndices != 0) {
        mesh.mUsesIndices = true;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * numOfIndices, indices, GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (6 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    mesh.mBuffers.insert(mesh.mBuffers.end(), {vbo, ebo});
    glBindVertexArray(0);

    return mesh;
}
