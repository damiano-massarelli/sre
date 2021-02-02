#include "rendering/effects/SSAO.h"
#include "Engine.h"

SSAO::SSAO()
    : Effect{ "ssao", "effects/ssao.glsl" } {

    {
        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setInt("inputTexture", 0);
        mPostProcessingShader.setInt("aoTexture", 1);
    }

    Texture::Settings loadOptions;
    loadOptions.dataPixelFormat = GL_RGBA;
    loadOptions.dataPixelType = GL_FLOAT;
    loadOptions.internalFormat = GL_RED;  // only red component is used
    loadOptions.appearanceOptions.hasMipmap = false;
    loadOptions.appearanceOptions.magFilter = GL_NEAREST;
    loadOptions.appearanceOptions.minFilter = GL_NEAREST;
    loadOptions.appearanceOptions.wrapS = GL_REPEAT;
    loadOptions.appearanceOptions.wrapT = GL_REPEAT;

    mTargetTexture
        = Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), loadOptions);

    mSSAOCreationTarget = RenderTarget{ &mTargetTexture, nullptr };

    std::uniform_real_distribution<float> dist;
    std::default_random_engine engine;

    createSamples(dist, engine);
    createNoiseTexture(dist, engine);

    mSSAOCreationShader = Shader::loadFromFile(
        { "effects/genericEffectVS.glsl" }, std::vector<std::string>{}, { "effects/ssaoCreateFS.glsl" }, false);

    {
        ShaderScopedUsage useShader{ mSSAOCreationShader };
        mSSAOCreationShader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
        mSSAOCreationShader.bindUniformBlock("Camera", Engine::renderSys.CAMERA_UNIFORM_BLOCK_INDEX);
        mSSAOCreationShader.setVec3Array("samples", mSSAOSamples);
        mSSAOCreationShader.setInt("positionBuffer", 0);
        mSSAOCreationShader.setInt("normalBuffer", 1);
        mSSAOCreationShader.setInt("noiseBuffer", 2);
    }
}

void SSAO::createSamples(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine) {
    const int numSamples = 64;
    mSSAOSamples.reserve(numSamples);

    for (int i = 0; i < 64; i++) {
        auto sample = glm::vec3{
            dist(engine) * 2.0f - 1.0f,
            dist(engine) * 2.0f - 1.0f,
            dist(engine)  // only on the positive direction of the hemisphere
        };

        sample = glm::normalize(sample);
        sample *= dist(engine);

        // make sure the majority of samples are close to the fragment
        float scale = i / 64.0f;
        scale = glm::mix(0.1f, 1.0f, scale * scale);

        sample *= scale;
        mSSAOSamples.push_back(sample);
    }
}

void SSAO::createNoiseTexture(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine) {
    std::vector<glm::vec3> noiseData;

    // resolution width and height
    const int res = 4;

    noiseData.reserve(res * res);

    for (int i = 0; i < res * res; ++i) {
        auto rotation = glm::vec3{ dist(engine) * 2.0f - 1.0f, dist(engine) * 2.0f - 1.0f, 0.0 };

        noiseData.push_back(rotation);
    }

    Texture::Settings loadOptions;
    loadOptions.dataPixelType = GL_FLOAT;
    loadOptions.dataPixelFormat = GL_RGB;
    loadOptions.internalFormat = GL_RGB;
    loadOptions.appearanceOptions.wrapS = GL_REPEAT;
    loadOptions.appearanceOptions.wrapT = GL_REPEAT;
    loadOptions.appearanceOptions.hasMipmap = false;
    mNoiseTexture = Texture::load(noiseData.data(), res, res, loadOptions);
}

void SSAO::update() {
    if (mNeedsUpdate) {
        ShaderScopedUsage useShader{ mPostProcessingShader };

        mNeedsUpdate = false;
        mPostProcessingShader.setFloat("darkenFactor", mDarkenFactor);
        mPostProcessingShader.setInt("blurSize", mBlurSize);
    }
}

void SSAO::applyEffect(const Texture& input, const RenderTarget* dst) {
    RenderSystem& rsys = Engine::renderSys;

    rsys.copyTexture({ rsys.gBuffer.getPositionBuffer(),
        rsys.gBuffer.getNormalBuffer(), mNoiseTexture }, mSSAOCreationTarget, mSSAOCreationShader);
    Effect::applyEffect({ input, mTargetTexture }, dst);
}

void SSAO::setKernelSize(int size) {
    if (size > 64)
        size = 64;

    mKernelSize = size;

    ShaderScopedUsage useShader{ mSSAOCreationShader };
    mSSAOCreationShader.setInt("kernelSize", size);
}

int SSAO::getKernelSize() const {
    return mKernelSize;
}

void SSAO::setRadius(float radius) {
    mRadius = radius;

    {
        ShaderScopedUsage usage{ mSSAOCreationShader };
        mSSAOCreationShader.setFloat("radius", radius);
    }
}

float SSAO::getRadius() const {
    return mRadius;
}

void SSAO::setDarkenFactor(float factor) {
    mDarkenFactor = factor;
    mNeedsUpdate = true;
}

float SSAO::getDarkenFactor() const {
    return mDarkenFactor;
}

void SSAO::setBlurSize(int size) {
    mBlurSize = size;
    mNeedsUpdate = true;
}

int SSAO::getBlurSize() const {
    return mBlurSize;
}
