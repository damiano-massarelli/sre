#ifndef LIGHTMATERIAL_H
#define LIGHTMATERIAL_H
#include "Material.h"
#include <glm/common.hpp>

/** Material used by lights or props
  * This material has a single color and is not affected by lights */
class LightMaterial : public Material
{
    public:
        glm::vec3 color{1.0f, 1.0f, 1.0f};

        LightMaterial();

        virtual void use() override;

        virtual ~LightMaterial() = default;
};

#endif // LIGHTMATERIAL_H
