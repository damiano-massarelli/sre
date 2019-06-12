#pragma once
#include "Material.h"

/**
 * Material used when rendering for Shadow Mapping
 */
class ShadowMapMaterial :
	public Material
{
public:
	ShadowMapMaterial();

	virtual void use() override;

	virtual ~ShadowMapMaterial() = default;
};

