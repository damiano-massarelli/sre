#pragma once
#include "rendering/materials/Shader.h"
#include <string>

class Effect
{
private:
	std::string mEffectPath;
	std::string mName;

public:
	/**
	 * Creates a new effect with a name and a path where the shader is stored.
	 * @param name the name of the effect (also the name of the function of the effect)
	 * @param effectPath the path where the fragment shader of this effect is stored.
	 */
	Effect(const std::string& name, const std::string& effectPath);

	/**
	 * @return the path where the fragment shader of this effect is stored.
	 */
	const std::string& getEffectPath() const;

	/**
	 * @return the name of this effect.
	 */
	const std::string& getName() const;

	/**
	 * Called every time the effect needs to be set up.
	 */
	virtual void onSetup(Shader& postProcessingShader) {}

	/**
	 * Called every time the image is going to be processed
	 * by the effect. 
	 */
	virtual void update(Shader& postProcessingShader) {}

	virtual ~Effect() = default;
};
