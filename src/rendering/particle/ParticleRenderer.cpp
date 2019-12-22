#include "ParticleRenderer.h"
#include "Engine.h"
#include "MeshLoader.h"
#include "MeshCreator.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

template <class Iterator, class Compare>
void insertionSort(Iterator begin, Iterator end, Compare cmp) {
	auto current = begin;
	while (current != end) {
		auto swapper = current;
		while (swapper != begin && cmp(*(swapper - 1), *swapper)) {
			std::swap(*swapper, *(swapper - 1));
			swapper--;
		}
		current++;
	}
}

std::vector<float> positions{
	.5f, .5f, 0.0f,
	.5f, -.5f, 0.0f,
	-.5f, -.5f, 0.0f,
	-.5f, .5f, 0.0f
};

std::vector<std::uint32_t> indices{
	2, 1, 0,
	2, 0, 3
};

void ParticleRenderer::init()
{
	prepareParticleQuad();

	mParticleShader = Shader::loadFromFile(std::vector<std::string>{ "shaders/particleVS.glsl" }, {}, { "shaders/particleFS.glsl" });
	mParticleShader.use();
	mFrameSizeLocation = mParticleShader.getLocationOf("frameSize");
}


void ParticleRenderer::prepareParticleQuad()
{
	MeshLoader loader;
	loader.loadData(positions.data(), positions.size(), 3);
	loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, false);

	mParticleDataVBO = loader.loadData<float>(nullptr, MAX_PARTICLES * FLOATS_PER_PARTICLE, 0, GL_ARRAY_BUFFER, GL_FLOAT, false, GL_STREAM_DRAW);

	int stride = FLOATS_PER_PARTICLE * sizeof(float);
	loader.addAttribPointer(GL_ARRAY_BUFFER, mParticleDataVBO, stride, 4, GL_FLOAT, 0);
	loader.addAttribPointer(GL_ARRAY_BUFFER, mParticleDataVBO, stride, 4, GL_FLOAT, 4 * sizeof(float));
	loader.addAttribPointer(GL_ARRAY_BUFFER, mParticleDataVBO, stride, 4, GL_FLOAT, 8 * sizeof(float));
	loader.addAttribPointer(GL_ARRAY_BUFFER, mParticleDataVBO, stride, 4, GL_FLOAT, 12 * sizeof(float));

	loader.addAttribPointer(GL_ARRAY_BUFFER, mParticleDataVBO, stride, 4, GL_FLOAT, 16 * sizeof(float));

	loader.addAttribPointer(GL_ARRAY_BUFFER, mParticleDataVBO, stride, 1, GL_FLOAT, 20 * sizeof(float));

	mParticleMesh = loader.getMesh(0, indices.size());
}

void ParticleRenderer::updateParticleVBO(const std::vector<float>& data)
{
	glBindBuffer(GL_ARRAY_BUFFER, mParticleDataVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), nullptr, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleRenderer::computeInverseViewMatrix()
{
	glm::mat4 viewMatrix = Engine::renderSys.getViewMatrix(Engine::renderSys.getCamera()->transform);

	glm::mat3 counterRotation = glm::transpose(glm::mat3{ viewMatrix });

	mInverseView = glm::mat4(counterRotation);
	mInverseView[3][3] = 1.0f;
}

void ParticleRenderer::storeModelMatrix(const Particle& p, std::vector<float>& data)
{
	float lifePercent = p.elapsedTime / p.durationMillis;
	float rotation = glm::mix(p.initialRotation, p.finalRotation, lifePercent);
	float scale = glm::mix(p.initialScale, p.finalScale, lifePercent);

	glm::mat4 transform = glm::mat4{ 1.0f };
	float ssin = scale * glm::sin(rotation);
	float scos = scale * glm::cos(rotation);

	transform[0] = glm::vec4{ scos, ssin, 0, 0 };
	transform[1] = glm::vec4{ -ssin, scos, 0, 0 };
	transform[2] = glm::vec4{ 0, 0, scale, 0 };
	transform[3] = glm::vec4{ p.position, 1.0f };

	// TODO avoid this transformation here, it is heavy
	transform = transform * mInverseView;

	data.insert(data.end(), glm::value_ptr(transform), glm::value_ptr(transform) + 16);
}


void ParticleRenderer::storeOffsetsAndBlendFactor(const Particle& p, const ParticleEmitter* emitter, std::vector<float>& data)
{
	float lifePercentage = p.elapsedTime / p.durationMillis;
	float percentage = (emitter->mFrames - 1) * lifePercentage;
	int frame = static_cast<int>(percentage);
	float blend = percentage - frame;

	int col = frame % emitter->mCols;
	int row = frame / emitter->mCols;

	auto offset = glm::vec4{
			col * emitter->mColSize,
			1.0f - row * emitter->mRowSize,
			col * emitter->mColSize,
			1.0f - row * emitter->mRowSize,
	};

	if (frame != emitter->mFrames - 1) {
		col = (frame + 1) % emitter->mCols;
		row = (frame + 1) / emitter->mCols;

		offset[2] = col * emitter->mColSize;
		offset[3] = 1.0f - row * emitter->mRowSize;
	}

	data.insert(data.end(), glm::value_ptr(offset), glm::value_ptr(offset) + 4);
	data.push_back(blend);
}

void ParticleRenderer::addEmitter(ParticleEmitter* emitter)
{
	mEmitters.push_back(emitter);
}

void ParticleRenderer::removeEmitter(const ParticleEmitter* emitter)
{
	mEmitters.erase(std::remove(mEmitters.begin(), mEmitters.end(), emitter), mEmitters.end());
}

void ParticleRenderer::render()
{
	computeInverseViewMatrix();

	glBindVertexArray(mParticleMesh.getVao());
	mParticleShader.use();

	glDepthMask(GL_FALSE);

	for (const auto emitter : mEmitters) {
		if (emitter->settings.useAlphaBlending) {
			glEnable(GL_BLEND);
			glBlendFunc(emitter->settings.sfactor, emitter->settings.dfactor);
		}
		setUpTextureAtlas(emitter);

		renderParticles(emitter);

		glDisable(GL_BLEND);
	}

	glDepthMask(GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

void ParticleRenderer::renderParticles(ParticleEmitter* emitter)
{
	std::vector<Particle>& particles = emitter->getParticles();

	const glm::vec3& camPosition = Engine::renderSys.getCamera()->transform.getPosition();

	insertionSort(particles.begin(), particles.end(), [&camPosition](const auto& p, const auto& p2) {
		return glm::distance2(p.position, camPosition) < glm::distance2(p2.position, camPosition);
	});

	std::vector<float> data;
	data.reserve(particles.size() * FLOATS_PER_PARTICLE);

	for (const auto& p : emitter->getParticles()) {
		storeModelMatrix(p, data);
		storeOffsetsAndBlendFactor(p, emitter, data);
	}

	updateParticleVBO(data);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, particles.size());
}

void ParticleRenderer::setUpTextureAtlas(const ParticleEmitter* emitter)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, emitter->mParticleAtlas.getId());

	const Texture& atlas = emitter->mParticleAtlas;

	mParticleShader.setVec2(mFrameSizeLocation, glm::vec2{ emitter->mColSize, emitter->mRowSize });
}


void ParticleRenderer::cleanUp()
{
	mParticleShader = Shader();
}

ParticleRenderer::~ParticleRenderer()
{

}
