#pragma once
#include "Particle.h"
#include "ParticleEmitter.h"
#include "rendering/materials/Shader.h"
#include "rendering/mesh/Mesh.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

/**
 * Renderer for particles.
 * ParticleEmitter%s send particles to this renderer in order to render
 * the particles they generate.
 */
class ParticleRenderer {
private:
    static constexpr int MAX_PARTICLES = 10000;
    static constexpr int FLOATS_PER_PARTICLE = 21;

    std::vector<ParticleEmitter*> mEmitters;

    Shader mParticleShader;
    std::int32_t mFrameSizeLocation;

    Mesh mParticleMesh;

    std::uint32_t mParticleDataVBO = 0;

    glm::mat4 mInverseView;

    void renderParticles(ParticleEmitter* emitter);

    void storeModelMatrix(const Particle& p, std::vector<float>& data);

    void storeOffsetsAndBlendFactor(const Particle& p, const ParticleEmitter* emitter, std::vector<float>& data);

    void setUpTextureAtlas(const ParticleEmitter* emitter);

    void prepareParticleQuad();

    void updateParticleVBO(const std::vector<float>& data);

    void computeInverseViewMatrix();

public:
    ParticleRenderer() = default;

    /**
     * Inits the renderer.
     * Should only be called by the RenderSystem.
     */
    void init();

    /**
     * Add a ParticleEmitter to this renderer.
     * ParticleEmitter%s call this method automatically when they are created.
     * @param emitter the ParticleEmitter to add.
     */
    void addEmitter(ParticleEmitter* emitter);

    /**
     * Remove a ParticleEmitter.
     * ParticleEmitter%s call this method automatically when they are destroyed.
     * @param emitter the emitter to remove.
     */
    void removeEmitter(const ParticleEmitter* emitter);

    /**
     * Renders the particles.
     */
    void render();

    /**
     * TODO
     */
    void cleanUp();

    /**
     * Cleans up rendering data.
     * This method should only be called by the RenderSystem.
     */
    void shutdown();

    ~ParticleRenderer();
};
