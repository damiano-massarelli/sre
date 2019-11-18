#pragma once
#include "rendering/materials/Shader.h"
#include <vector>
#include <map>
#include <cstdint>

/**
 * A wrapper of the Shader class specialized in deferred rendering of Light%s.
 */
class DeferredLightShader
{
private:
	std::int32_t mLightIndexLocation = 0;
	std::int32_t mLightRadiusLocation = 0;

public:
	/** The shader used */
	Shader shader;

	DeferredLightShader() = default;

	void init(const std::vector<std::string>& vertexShaders,
		 const std::vector<std::string>& fragmentShaders,
		 const std::vector<std::string>& bufferNames,
		 const std::map<std::string, std::uint32_t>& uboBindings);

	/** Sets the light index for this shader.
	  * this index specifies a light in the array of all the lights */
	void setLightIndex(int index) const;

	/**
	 * Sets the radius of the Light.
	 * It has no effect on DirectionalLight%s.
	 */
	void setLightRadius(float radius) const;

	/** cleans up shader resources */
	void cleanUp();
};

