#ifndef MESHLOADER_H
#define MESHLOADER_H
#include "Mesh.h"
#include <cstdint>
#include <vector>
#include <glad/glad.h>

/**
  * Creates a new Mesh given the data for its vertices.
  * This class also provides some utility methods to create
  * a Mesh from packed data. */
class MeshLoader
{
    private:
        int mDrawMode;
        std::uint32_t mVao;
        Mesh mMesh;
        std::vector<std::uint32_t> mBuffers;

        int mCurrentAttribPointer = 0;

    public:
        /** Creates a new mesh given an array of floats containing packed data.
          * about: vertex position, vertex normal, vertex texture coordinates.
          * Data should be provided in this exact order */
        static Mesh createMesh(float vertexData[], std::uint32_t numOfVertices, std::uint32_t indices[], std::uint32_t numOfIndices, int drawMode = GL_TRIANGLES);

        /** Creates a new MeshLoader specifying the draw mode for the Mesh to create
          * @param drawMode the mode used to draw the created mesh */
        MeshLoader(int drawMode = GL_TRIANGLES);

        /** Loads vertex description data for the Mesh.
          * @param data an array of data to load.
          * @param size the length of the data array
          * @param dataPerVertex how many elements of the array are describe a single vertex
          * @param bufferType the type of buffer to create (GL_ARRAY_BUFFER, GL_ELEMENT_ARAY_BUFFER, etc)
          * @param dataType the type of the data to load (GL_FLOAT, GL_UNSIGNED_INT, etc). */
        template <typename T>
        void loadData(T* data, std::uint32_t size, int dataPerVertex, int bufferType = GL_ARRAY_BUFFER, int dataType = GL_FLOAT) {
            std::uint32_t bo;
            glGenBuffers(1, &bo);
            glBindBuffer(bufferType, bo);

            glBufferData(bufferType, size * sizeof(T), data, GL_STATIC_DRAW);

            if (bufferType != GL_ELEMENT_ARRAY_BUFFER) {
                glEnableVertexAttribArray(mCurrentAttribPointer);
                glVertexAttribPointer(mCurrentAttribPointer, dataPerVertex, dataType, GL_FALSE, dataPerVertex * sizeof(T), (void *) 0);
                mCurrentAttribPointer++;
            }

            mMesh.mBuffers.push_back(bo);
        }

        /** Creates the Mesh
          * @param the number of vertices of this mesh
          * @param the number of indices. If this value is different from 0 the mesh will use indexed rendering.
          * @return the created Mesh */
        Mesh getMesh(std::uint32_t vertexNumber, std::uint32_t indexNumber);
};

#endif // MESHLOADER_H
