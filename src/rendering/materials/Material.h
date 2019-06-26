#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader.h"
#include "RenderPhase.h"
#include <string>
#include <memory>
#include <vector>
#include <glm/common.hpp>

/**
  * A Material contains all the properties used to define the appareance of an object */
class Material
{
private:
	/**
	 * Location of the model transformation matrix in the shader
	 */
	std::uint32_t mModelLocation = 0;

public:
    /** if true back face culling is disabled */
    bool isTwoSided = false;

	/** The render phases supported by this material.
	  * if a render phase is not supported the mesh having this material
	  * wont be rendered during that phase */
	RenderPhase supportedRenderPhases = RenderPhase::ALL;

    /** the shader program used to render the mesh using this material */
    Shader shader;

    /**
        * Creates a new material using a vertexShader and a fragmentShader.
        * Vertex shader and fragment shader should be stored on disk.
        * @param vertexShader the path of the vertex shader
        * @param fragmentShader the path of the fragment shader */
    Material(const std::string& vertexShader, const std::string& fragmentShader);

    /**
        * Creates a new material using a vertexShader a fragmentShader and a geom shader.
        * Shaders should be stored on disk.
        * @param vertexShader the path of the vertex shader
        * @param geomtryShader the path of the geometry shader
        * @param fragmentShader the path of the fragment shader */
    Material(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);

    /**
        * Creates a new material using vertex shaders, fragment shader and a geom shader.
        * Shaders should be stored on disk.
        * The provided fragment shaders are combined together
        * one after the other in the order specified in the vector.
        * The same process is used for the vertex shaders and geometry shader.
        * @param vertexShaders the paths of the vertex shaders
        * @param geometryShaders the paths of the geometry shaders
        * @param fragmentShaders the paths of the fragment shaders */
    Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& geometryShaders, const std::vector<std::string>& fragmentShaders);

    /**
        * Sets this material as the current material for rendering.
        * This operation is also responsible for binding the correct
        * shader.
        */
    virtual void use() = 0;

    /**
        * Called after the material is used.
        * Should reset the state that can be altered in use() */
    virtual void after() {};

    /**
        * Specify whether Mesh%es with this material need to be rendered in order.
        * Some Mesh%es need to be drawn in a certain order (e.g transparent ones)
        * their materials should return true. The order in which Mesh%es are rendered
        * is determined by the value of renderOrder()
        * @return true if the corresponding mesh needs to be drawn in a certain order, false otherwise */
    virtual bool needsOrderedRendering() { return false; };

    /**
        * Provides a value to order Mesh%es.
        * @param position the position of the GameObject to which this material belongs
        * @return the render order (smaller value => rendered first) */
    virtual float renderOrder(const glm::vec3& position) { return 0.0f; };

	/**
	 * @return the model matrix location in the shader.
	 */
	std::uint32_t getModelLocation() const;

    virtual ~Material();
};

using MaterialPtr = std::shared_ptr<Material>;

#endif // MATERIAL_H
