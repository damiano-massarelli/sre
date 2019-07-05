#pragma once

/**
 * Describes the rendering phases in which a material can be used.
 * It is also used by the RenderSystem to specify the current phase.
 * An object can be rendered multiple times.
 * For instance, during shadow mapping each mesh is
 * rendered twice. Some objects should not be rendered
 * in certain phases. The skybox, as an example, should
 * not be rendered during shadow mapping.
 */
enum RenderPhase {
	NONE				= 0,
	FORWARD_RENDERING	= 1 << 0,
	SHADOW_MAPPING		= 1 << 1,
	DEFERRED_RENDERING  = 1 << 2,
	WATER				= 1 << 3,
	ALL					= ~0
};