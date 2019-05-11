#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader.h"
#include <string>
#include <memory>
#include <vector>

/**
  * A Material contains all the properties used to define the appareance of an object */
class Material
{
    public:
        /** if true back face culling is disabled */
        bool isTwoSided = false;

        /** the shader program used to render the mesh using this material */
        Shader shader;

        /**
          * Creates a new material using a vertexShader and a fragmentShader.
          * Vertex shader and fragment shader should be stored on disk.
          * @param vertexShader the path of the vertex shader
          * @param fragmentShader the path of the fragment shader */
        Material(const std::string& vertexShader, const std::string& fragmentShader);

        /**
          * Creates a new material using vertex shaders and fragment shader.
          * Vertex shaders and fragment shaders should be stored on disk.
          * The provided fragment shaders are combined together
          * one after the other in the order specified in the vector.
          * The same process is used for the vertex shaders.
          * @param vertexShaders the path of the vertex shaders
          * @param fragmentShaders the path of the fragment shaders */
        Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& fragmentShaders);

        /**
          * Sets this material as the current material for rendering.
          * This operation is also responsible for binding the correct
          * shader.
          */
        virtual void use() = 0;

        virtual ~Material();
};

using MaterialPtr = std::shared_ptr<Material>;

#endif // MATERIAL_H
