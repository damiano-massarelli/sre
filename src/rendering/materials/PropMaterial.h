#ifndef LIGHTMATERIAL_H
#define LIGHTMATERIAL_H
#include "Material.h"
#include <glm/common.hpp>

/** Material used by lights and props.
  * This material has a single color and is not affected by lights.
  * If showNormals is true (constructor) only the vertex normals of a Mesh are shown */
class PropMaterial : public Material
{
private:
	std::uint32_t mColorLocation = 0;

public:
	glm::vec3 color{ 1.0f, 1.0f, 1.0f };

	PropMaterial(bool showNormals = false);

	virtual void use() override;

	virtual std::size_t hash() const override;

	virtual bool equalsTo(const Material* rhs) const override;

	virtual ~PropMaterial() = default;
};

#endif // LIGHTMATERIAL_H
