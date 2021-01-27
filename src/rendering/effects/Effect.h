#pragma once
#include "rendering/materials/Shader.h"
#include "rendering/materials/Texture.h"
#include "rendering/RenderTarget.h"
#include <string>
#include <glm/vec2.hpp>
#include <vector>
#include <functional>

class Effect {
private:
    std::string mEffectPath;
    std::string mName;

protected:
    Shader mPostProcessingShader;
    Texture mOutput;
    RenderTarget mOutputTarget;

public:
    struct Settings {
        std::string name;
        std::string effectPath;

        glm::vec2 outputSize;
        Texture::Settings outputSettings;
    };

    Effect(const Settings& settings);

    /**
     * Creates a new effect with a name and a path where the shader is stored.
     * @param name the name of the effect (also the name of the function of the
     * effect)
     * @param effectPath the path where the fragment shader of this effect is
     * stored.
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
     * @return the output Texture used by this effect. 
     */
    const Texture& getOutput() const;

    /**
     * Called every time the effect needs to be set up.
     */
    virtual void onSetup(Shader& postProcessingShader) { }

    /**
     * Called every time the image is going to be processed
     * by the effect.
     */
    virtual void update(Shader& postProcessingShader) { }

    /**
     * Applies the effect on the given Texture.
     * The effect may use other Texture%s if it needs to.
     * @param input the input Texture. The effect will be applied to this Texture.
     * @param dst an optional destination RenderTarget. If dst is not nullptr then it will
     * contain the output. Otherwise, it will be possible to retrieve it using getOutput.
     */
    virtual void applyEffect(const Texture& input, const RenderTarget* dst = nullptr);

    /**
     * Applies the effect on the given Texture%s.
     * The effect may use other Texture%s if it needs to.
     * @param inputs the input Texture%s. The effect will be applied to these Texture%s.
     * @param dst an optional destination RenderTarget. If dst is not nullptr then it will
     * contain the output. Otherwise, it will be possible to retrieve it using getOutput.
     */
    virtual void applyEffect(const std::vector<std::reference_wrapper<const Texture>>& inputs, const RenderTarget* dst = nullptr);

    virtual ~Effect() = default;
};
