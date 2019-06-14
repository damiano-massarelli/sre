#pragma once

#include <glm/glm.hpp>
#include <cstdint>

/**
 * Basic settings for shadow mapping.
 */
struct ShadowMappingSettings
{
	/** width of the shadow map texture */
	std::int32_t mapWidth = 2048;

	/** height of the shadow map texture */
	std::int32_t mapHeight = 2048;

	/** width of the shadow box */
	float width = 75.0f;

	/** height of the shadow box */
	float height = 75.0f;

	/** depth of the shadow box */
	float depth = 75.0f;
};

