#pragma once
#include "ParticleEmitter.h"
#include "Particle.h"
#include "Shader.h"
#include "Mesh.h"
#include "MeshCreator.h"
#include <vector>
#include <cstdint>

class ParticleRenderer
{
private:
	std::vector<const ParticleEmitter*> mEmitters;

	Shader mParticleShader;
	std::int32_t mModelLocation;

	Mesh mParticleMesh;

	void renderParticles(const ParticleEmitter* emitter);

	glm::mat4 getModelMatrix(const Particle& p);

	void setUpTextureAtlas(const ParticleEmitter* emitter);

public:
	ParticleRenderer() = default;

	void init();

	void addEmitter(const ParticleEmitter* emitter);

	void removeEmitter(const ParticleEmitter* emitter);

	void render();

	void cleanUp();

	~ParticleRenderer();
};

