#pragma once
#include "rendering/materials/Material.h"

/**
 * Material used when rendering for Shadow Mapping
 */
class ShadowMapMaterial :
	public Material
{
public:
	ShadowMapMaterial();

	virtual void use() override;

	virtual void after() override;

	virtual ~ShadowMapMaterial() = default;
};

