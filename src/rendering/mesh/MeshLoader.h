#ifndef MESHLOADER_H
#define MESHLOADER_H
#include "rendering/mesh/Mesh.h"
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <type_traits>

/**
  * Creates a new Mesh given the data for its vertices.
  * This class also provides some utility methods to create
  * a Mesh from packed data. */
class MeshLoader
{
    private:
        GLenum mDrawMode;
        GLuint mVao;
        Mesh mMesh;
        std::vector<GLuint> mBuffers;

        GLuint mCurrentAttribPointer = 0;

    public:
        /** Creates a new mesh given an array of floats containing packed data.
          * about: vertex position, vertex normal, vertex texture coordinates.
          * Data should be provided in this exact order */
        static Mesh createMesh(const float vertexData[], std::uint32_t numOfVertices, std::uint32_t indices[], std::uint32_t numOfIndices, GLenum drawMode = GL_TRIANGLES);

        /** Creates a new MeshLoader specifying the draw mode for the Mesh to create
          * @param drawMode the mode used to draw the created mesh */
        MeshLoader(GLenum drawMode = GL_TRIANGLES);

        /** Loads vertex description data for the Mesh.
          * @param data an array of data to load.
          * @param size the length of the data array
          * @param dataPerVertex how many elements of the array are describe a single vertex
          * @param bufferType the type of buffer to create (GL_ARRAY_BUFFER, GL_ELEMENT_ARAY_BUFFER, etc)
          * @param dataType the type of the data to load (GL_FLOAT, GL_UNSIGNED_INT, etc).
		  * @param addToAttribPointer if true an attrib pointer is created for the data provided 
		  * @param usage the usage (static, stream, etc) */
        template<typename T>
        GLuint loadData(const T* data, std::size_t size, GLint dataPerVertex,
			GLenum bufferType = GL_ARRAY_BUFFER, GLenum dataType = GL_FLOAT, bool addToAttribPointer = true, GLenum usage = GL_STATIC_DRAW) {
            static_assert(std::is_arithmetic_v<T>);

            GLuint bo;
            glGenBuffers(1, &bo);
            glBindBuffer(bufferType, bo);

            glBufferData(bufferType, size * sizeof(T), data, usage);

            if (addToAttribPointer) {
                glEnableVertexAttribArray(mCurrentAttribPointer);
                if constexpr (std::is_floating_point_v<T>) {
                    glVertexAttribPointer(mCurrentAttribPointer, dataPerVertex, dataType, GL_FALSE, dataPerVertex * sizeof(T), (void*)0);
                }
                else if constexpr (std::is_integral_v<T>) {
                    glVertexAttribIPointer(mCurrentAttribPointer, dataPerVertex, dataType, dataPerVertex * sizeof(T), (void*)0);
                }
                mCurrentAttribPointer++;
			}
			
			if (bufferType == GL_ELEMENT_ARRAY_BUFFER)
				mMesh.mEbo = bo;

            mMesh.mBuffers.push_back(bo);

			return bo;
        }

		GLuint addAttribPointer(GLenum bufferType, GLuint vbo, GLsizei stride, GLint dataPerVertex, GLenum dataType, std::uintptr_t offset) {
			glBindBuffer(bufferType, vbo);

			glEnableVertexAttribArray(mCurrentAttribPointer);
			glVertexAttribPointer(mCurrentAttribPointer, dataPerVertex, dataType, GL_FALSE, stride, (void *)(offset));
			glVertexAttribDivisor(mCurrentAttribPointer, 1);

			auto attrib = mCurrentAttribPointer;
			mCurrentAttribPointer++;

			return attrib;
		}

        /** Creates the Mesh
          * @param the number of vertices of this mesh
          * @param the number of indices. If this value is different from 0 the mesh will use indexed rendering.
          * @return the created Mesh */
        Mesh getMesh(std::uint32_t vertexNumber, std::uint32_t indexNumber);
};

#endif // MESHLOADER_H
