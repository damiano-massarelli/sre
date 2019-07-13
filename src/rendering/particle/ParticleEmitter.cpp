#include "ParticleEmitter.h"
#include "Engine.h"
#include "EventManager.h"
#include "Engine.h"

ParticleEmitter::ParticleEmitter(const GameObjectEH& go, std::uint32_t maxParticles) : Component{ go }, mMaxParticles{ maxParticles }
{
	mCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);

	mParticles.reserve(maxParticles);

	Engine::particleRenderer.addEmitter(this);
}

void ParticleEmitter::setTextureAtlas(const Texture& texture, int frames, int rows, int cols)
{
	mParticleAtlas = texture;
	mFrames = frames;
	mRows = rows;
	mCols = cols;

	mRowSize = 1.0f / mRows;
	mColSize = 1.0f / mCols;
}

bool ParticleEmitter::emit(const Particle& p)
{
	if (mParticles.size() >= mMaxParticles)
		return false;

	mParticles.push_back(p);
	return true;
}

void ParticleEmitter::start(float rate)
{
	mSecondsPerParticle = 1.0f / rate;
	mStarted = true;
	mElapsedFromLastEmission = 0.0f;
}

#include <cstdlib>
#include <glm/glm.hpp>
#include <iostream>
void ParticleEmitter::onEvent(SDL_Event e)
{
	float delta = *(static_cast<float*>(e.user.data1));
	float deltaSec = delta / 1000.0f;

	if (mStarted) {
		int particlesToEmit = static_cast<int>(mElapsedFromLastEmission / mSecondsPerParticle);

		mElapsedFromLastEmission -= particlesToEmit * mSecondsPerParticle;
		for (int i = 0; i < particlesToEmit; i++) {
			
			Particle p;
			p.durationMillis = 50000.0f;
			p.gravityScale = 0.0f;
			p.velocity = .5f * glm::vec3{ 0, ((float)rand() / (RAND_MAX)) + 0.5f, 0 };
			emit(p);
		}

		mElapsedFromLastEmission += deltaSec;
	}

	auto it = mParticles.begin();
	while (it != mParticles.end()) {
		it->elapsedTime += delta;

		Particle& p = *it;
		p.position += p.velocity * deltaSec;
		
		p.velocity.y -= gravity * deltaSec * p.gravityScale;

		if (it->elapsedTime > it->durationMillis) 
			it = mParticles.erase(it);
		 else
			++it;
	}
}

const std::vector<Particle>& ParticleEmitter::getParticles() const
{
	return mParticles;
}

ParticleEmitter::~ParticleEmitter()
{
	Engine::particleRenderer.removeEmitter(this);
}
