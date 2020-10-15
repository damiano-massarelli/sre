#pragma once

#include <cstdint>
#include <glm/glm.hpp>

/**
 * Handles fog settings.
 */
class FogSettings {
private:
    std::uint32_t mFogUbo = 0;

    glm::vec3 mFogColor = glm::vec3{ 130.0f, 150.0f, 170.0f } / 255.0f;

    /** a value of 0 means the fog is infinitely distant */
    float mInvereDistance = 0.0035f;

    /** When this value is high transition from not in fog to fog happens
     * suddently */
    float mRapidity = 3.5f;

    void updateUbo();

public:
    FogSettings() = default;

    FogSettings(const FogSettings& fs) = delete;

    FogSettings& operator=(const FogSettings& fs) = delete;

    void init();

    /**
     * Sets the color of the fog.
     * @param fogColor the color of the fog.
     */
    void setFogColor(const glm::vec3& fogColor);

    /**
     * Sets the inverse distance at which fog appears.
     * If this value approaches 0 the fog will be infinitely distant,
     * if this value approaches inf fog will be very near to the camera.
     * @param invDistance the inverse distance
     */
    void setInverseDistance(float invDistance);

    /**
     * Sets how rapid the change from non-fog to fog is.
     * @rapidity the rapidity
     */
    void setRapidity(float rapidity);

    ~FogSettings();
};
