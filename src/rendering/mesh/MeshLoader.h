#ifndef MESHLOADER_H
#define MESHLOADER_H
#include "Mesh.h"
#include <cstdint>
#include <glad/glad.h>

class MeshLoader
{
    public:
        /** Creates a new mesh given an array of floats containing information
          * about: vertex position, vertex normal, vertex texture coordinates.
          * Data should be provided in this exact order */
        static Mesh createMesh(float vertexData[], std::uint32_t numOfVertices, std::uint32_t indices[], std::uint32_t numOfIndices, int drawMode = GL_TRIANGLES);
};

#endif // MESHLOADER_H
