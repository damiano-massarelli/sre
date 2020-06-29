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
        std::uint32_t mVao;
        Mesh mMesh;
        std::vector<std::uint32_t> mBuffers;

        int mCurrentAttribPointer = 0;

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
        template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
        std::uint32_t loadData(const T* data, std::uint32_t size, int dataPerVertex,
			GLenum bufferType = GL_ARRAY_BUFFER, GLenum dataType = GL_FLOAT, bool addToAttribPointer = true, GLenum usage = GL_STATIC_DRAW) {

            std::uint32_t bo;
            glGenBuffers(1, &bo);
            glBindBuffer(bufferType, bo);

            glBufferData(bufferType, size * sizeof(T), data, usage);

            if (addToAttribPointer) {
                glEnableVertexAttribArray(mCurrentAttribPointer);
                glVertexAttribPointer(mCurrentAttribPointer, dataPerVertex, dataType, GL_FALSE, dataPerVertex * sizeof(T), (void *) 0);
                mCurrentAttribPointer++;
			}
			
			if (bufferType == GL_ELEMENT_ARRAY_BUFFER)
				mMesh.mEbo = bo;

            mMesh.mBuffers.push_back(bo);

			return bo;
        }

		// template specialization for ints. They should use glVertexAttrib * I * Pointer
		template <>
		std::uint32_t loadData<std::int32_t, nullptr>(const std::int32_t* data, std::uint32_t size, int dataPerVertex,
			GLenum bufferType, GLenum dataType, bool addToAttribPointer, GLenum usage) {

			std::uint32_t bo;
			glGenBuffers(1, &bo);
			glBindBuffer(bufferType, bo);

			glBufferData(bufferType, size * sizeof(std::int32_t), data, usage);

			if (addToAttribPointer) {
				glEnableVertexAttribArray(mCurrentAttribPointer);
				glVertexAttribIPointer(mCurrentAttribPointer, dataPerVertex, dataType, dataPerVertex * sizeof(std::int32_t), (void *)0);
				mCurrentAttribPointer++;
			}
			
			if (bufferType == GL_ELEMENT_ARRAY_BUFFER)
				mMesh.mEbo = bo;

			mMesh.mBuffers.push_back(bo);

			return bo;
		}

		int addAttribPointer(GLenum bufferType, std::uint32_t vbo, int stride, int dataPerVertex, GLenum dataType, int offset) {
			glBindBuffer(bufferType, vbo);

			glEnableVertexAttribArray(mCurrentAttribPointer);
			glVertexAttribPointer(mCurrentAttribPointer, dataPerVertex, dataType, GL_FALSE, stride, (void *)offset);
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
