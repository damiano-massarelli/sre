#pragma once
#include "Particle.h"
#include "gameobject/GameObjectEH.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * Settings for ParticleEmitter%s.
 */
class EmitterSettings {
private:
    float randVal(float min, float max);

    glm::vec3 randVal(const glm::vec3& min, const glm::vec3& max);

public:
    /** min lifetime for particles (millis) */
    float minDuration = 5000.0f;
    /** max lifetime for particles (millis) */
    float maxDuration = 5000.0f;

    /** The min value for a particle position (relative to the ParticleEmitter
     * position)  */
    glm::vec3 positionOffsetMin{ 0, 0, 0 };
    /** The min value for a particle position (relative to the ParticleEmitter
     * position)  */
    glm::vec3 positionOffsetMax{ 0, 0, 0 };

    /** If true, velocityMin and velocityMax are rotated according to their
     * emitter */
    bool followEmitterDirection = false;
    /** min velocity */
    glm::vec3 velocityMin{ 0, 0, 0 };
    /** max velocity */
    glm::vec3 velocityMax{ 0, 0, 0 };

    /** min value for initial gravity scale */
    float initialGravityScaleMin = 1.0f;
    /** max value for initial gravity scale */
    float initialGravityScaleMax = 1.0f;

    /** min value for final gravity scale */
    float finalGravityScaleMin = 1.0f;
    /** max value for final gravity scale */
    float finalGravityScaleMax = 1.0f;

    /** min value for initial rotation */
    float initialRotationMin = 0.0f;
    /** max value for initial rotation */
    float initialRotationMax = 0.0f;

    /** min value for final rotation */
    float finalRotationMin = 0.0f;
    /** max value for final rotation */
    float finalRotationMax = 0.0f;

    /** min value for initial scale */
    float initialScaleMin = 1.0f;
    /** max value for initial scale */
    float initialScaleMax = 1.0f;

    /** min value for final scale */
    float finalScaleMin = 1.0f;
    /** max value for final scale */
    float finalScaleMax = 1.0f;

    /** specifies whether this emitter uses alpha blending for its particles */
    bool useAlphaBlending = true;
    /** source alpha equation */
    GLenum sfactor = GL_SRC_ALPHA;
    /** destination alpha equation */
    GLenum dfactor = GL_ONE_MINUS_SRC_ALPHA;

    /**
     * Sets up a Particle according the settings.
     * @param go the emitter GameObject
     * @param p the Particle to set up.
     */
    void setUp(const GameObjectEH& go, Particle& p);
};
