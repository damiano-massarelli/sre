#include "MeshLoader.h"
#include <glad/glad.h>

Mesh MeshLoader::createMesh(float vertexData[], std::uint32_t numOfVertices, std::uint32_t indices[], std::uint32_t numOfIndices, int drawMode)
{
    Mesh mesh{0, 0, 0};
    mesh.drawMode = drawMode;
    mesh.mVertexNumber = numOfVertices;
    mesh.mIndicesNumber = numOfIndices;

    glGenVertexArrays(1, &mesh.mVao);

    glGenBuffers(1, &mesh.mVbo);

    if (numOfIndices != 0)
        glGenBuffers(1, &mesh.mEbo);

    glBindVertexArray(mesh.mVao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.mVbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * numOfVertices * sizeof(float), vertexData, GL_STATIC_DRAW);

    if (numOfIndices != 0) {
        mesh.usesIndices = true;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * numOfIndices, indices, GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

    glBindVertexArray(0);

    return mesh;
}
