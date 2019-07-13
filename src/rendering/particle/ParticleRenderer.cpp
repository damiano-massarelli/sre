#include "ParticleRenderer.h"
#include "Engine.h"
#include <algorithm>
#include <glm/glm.hpp>

void ParticleRenderer::init()
{
	mParticleMesh = MeshCreator::plane();

	mParticleShader = Shader::loadFromFile(std::vector<std::string>{ "shaders/particleVS.glsl" }, {}, { "shaders/particleFS.glsl" });
	mParticleShader.use();
	mModelLocation = mParticleShader.getLocationOf("model");
}

glm::mat4 ParticleRenderer::getModelMatrix(const Particle& p)
{
	glm::mat4 viewMatrix = Engine::renderSys.getViewMatrix(Engine::renderSys.camera->transform);

	glm::mat3 counterRotation = glm::transpose(glm::mat3{ viewMatrix });

	glm::mat4 rotation = glm::mat4(counterRotation);
	rotation[3][3] = 1.0f;

	glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, p.position);
	glm::mat4 scale = glm::scale(translate, glm::vec3{ p.scale });
	
	return scale * rotation;
}


void ParticleRenderer::addEmitter(const ParticleEmitter* emitter)
{
	mEmitters.push_back(emitter);
}

void ParticleRenderer::removeEmitter(const ParticleEmitter* emitter)
{
	mEmitters.erase(std::remove(mEmitters.begin(), mEmitters.end(), emitter), mEmitters.end());
}

void ParticleRenderer::render()
{
	glBindVertexArray(mParticleMesh.getVao());
	mParticleShader.use();
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	for (const auto emitter : mEmitters) {
		setUpTextureAtlas(emitter);

		// bind texture for this emitter
		renderParticles(emitter);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

#include <iostream>
void ParticleRenderer::renderParticles(const ParticleEmitter* emitter)
{
	for (const auto& p : emitter->getParticles()) {
		float lifePercentage = p.elapsedTime / p.durationMillis;
		float percentage = (emitter->mFrames - 1) * lifePercentage;
		int frame = static_cast<int>(percentage);
		float blend = percentage - frame;

		int col = frame % emitter->mCols;
		int row = frame / emitter->mCols;

		auto offset = glm::vec2{
				col * emitter->mColSize,
				1.0f - row * emitter->mRowSize
		};

		auto nextOffset = offset;
		if (frame != emitter->mFrames - 1) {
			col = (frame + 1) % emitter->mCols;
			row = (frame + 1) / emitter->mCols;

			nextOffset = glm::vec2{
				col * emitter->mColSize,
				1.0f - row * emitter->mRowSize
			};
		}

		mParticleShader.setVec2("offset", offset);
		mParticleShader.setVec2("nextOffset", nextOffset);
		mParticleShader.setFloat("blend", blend);

		mParticleShader.setMat4(mModelLocation, getModelMatrix(p));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
	}
}

void ParticleRenderer::setUpTextureAtlas(const ParticleEmitter* emitter)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, emitter->mParticleAtlas.getId());

	const Texture& atlas = emitter->mParticleAtlas;
	mParticleShader.setVec2("frameSize", glm::vec2{ emitter->mColSize, emitter->mRowSize });
}


void ParticleRenderer::cleanUp()
{
	mParticleShader = Shader();
}

ParticleRenderer::~ParticleRenderer()
{

}
