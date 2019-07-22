#pragma once
#include "Component.h"
#include "Particle.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include "Texture.h"
#include "EmitterSettings.h"
#include <vector>
#include <cstdint>

/**
 * Emitter for particles.
 */
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
	/** settings for particle generation */
	EmitterSettings settings;

	/**
	 * Creates a new ParticleEmitter.
	 * @param go the GameObject that will emit particles.
	 * @param maxParticles the maximum number of particles this system can emit.
	 */
	ParticleEmitter(const GameObjectEH& go, std::uint32_t maxParticles);

	/**
	 * Sets the Texture atlas used by this emitter.
	 * @param texture the texture to use
	 * @param frames the number of frames in the atlas
	 * @param rows the number of rows in the atlas
	 * @param cols the number of columns in the atlas
	 */
	void setTextureAtlas(const Texture& texture, int frames, int rows = 1, int cols = 1);

	/**
	 * Emits a Particle.
	 * @param p the particle to emit.
	 */
	bool emit(const Particle& p);

	/**
	 * Starts emitting particles at a continuous rate.
	 * @param rate the number of particles emitted per second.
	 */
	void start(float rate);

	virtual void onEvent(SDL_Event e) override;

	/**
	 * @return all the particles handled by this emitter
	 */
	const std::vector<Particle>& getParticles() const;

	/**
	* @return all the particles handled by this emitter
	*/
	std::vector<Particle>& getParticles();

	~ParticleEmitter();
};
