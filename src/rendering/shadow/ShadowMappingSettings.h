#pragma once

#include <glm/glm.hpp>
#include <cstdint>

/**
 * Basic settings for shadow mapping.
 * This class holds information about the dimension of the shadow map texture
 * and the size of the shadow cuboid. Its depth also defines how far away
 * the shadow is active. However, in order to smooth shadows that are far away,
 * the same information can be changed using setShadowDistance which will also 
 * affect where shadows starts to fade out. This method should only be called when
 * depth changes considerably. To set the range in which shadows fade out
 * (from full-shadow to no-shadow) use setSmoothRange.
 */
class ShadowMappingSettings
{
private:
	std::uint32_t mShadowUbo = 0;

	float mFadeRange = 30.0f;

	/* How strong shadows are */
	float mShadowStrength = 0.7f;

	void updateUbo();

public:
	/** width of the shadow map texture */
	std::int32_t mapWidth = 2048;

	/** height of the shadow map texture */
	std::int32_t mapHeight = 2048;

	/** width of the shadow box */
	float width = 75.0f;

	/** height of the shadow box */
	float height = 75.0f;

	/** depth of the shadow box */
	float depth = 220.0f;

	/**
	 * If true, objects use a special and fast shader when
	 * they are rendered for shadow mapping. If false, they use
	 * their normal shader. Using the fast shader improves performance
	 * but transparent textures are not reported in the shadow.
	 */
	bool useFastShader = false;

	ShadowMappingSettings(const ShadowMappingSettings& sms) = delete;

	ShadowMappingSettings& operator=(const ShadowMappingSettings& sms) = delete;

	ShadowMappingSettings() = default;

	/** Called by RenderSystem */
	void init();

	/**
	 * Sets depth and the value at which shadows disappear.
	 * This method should only be called when depth changes considerably.
	 * @pram shadowDistance the value for depth
	 */
	void setShadowDistance(float shadowDistance);

	/**
	 * Sets the range in which shadows disappear.
	 * When distance < depth - range shadows are completely visible,
	 * when depth - range < distance < depth shadows fade out
	 */
	void setShadowFadeRange(float range);

	/**
	 * How strong shadows are.
	 * A value of 0 means shadows are disabled.
	 * A value of 1 means only ambient light is not affected by shadows.
	 * @strength a value in [0, 1]
	 */
	void setShadowStrength(float stregth);

	float getShadowStrength() const;

	~ShadowMappingSettings();
};

