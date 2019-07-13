#pragma once
#include "Component.h"
#include "Particle.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include "Texture.h"
#include <vector>
#include <cstdint>

class ParticleEmitter :
	public Component, EventListener
{
	friend class ParticleRenderer;

private:
	static constexpr float gravity = 9.8f;

 	CrumbPtr mCrumb;

	std::vector<Particle> mParticles;

	std::uint32_t mMaxParticles;

	float mSecondsPerParticle = 0.0f;
	float mElapsedFromLastEmission = 0.0f;
	bool mStarted = false;

	Texture mParticleAtlas;
	int mCols = 1;
	int mRows = 1;
	float mColSize = 0.0f;
	float mRowSize = 0.0f;
	int mFrames = 1;

public:
	ParticleEmitter(const GameObjectEH& go, std::uint32_t maxParticles);

	void setTextureAtlas(const Texture& texture, int frames, int rows = 1, int cols = 1);

	bool emit(const Particle& p);

	void start(float rate);

	virtual void onEvent(SDL_Event e) override;

	const std::vector<Particle>& getParticles() const;

	~ParticleEmitter();
};
