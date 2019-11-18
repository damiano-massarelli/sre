#ifndef SKYBOXMATERIAL_H
#define SKYBOXMATERIAL_H
#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"
#include <glm/common.hpp>

/**
  * Material for a skybox.
  * Use RenderSystem.setSkybox() to use a GameObject as a skybox.
  * The chosen GameObject must have a SkyboxMaterial. */
class SkyboxMaterial : public Material
{
    private:
        Texture mCubemap;

    public:
        /**
          * Creates a new SkyboxMaterial using the specified cubemap.
          * @param cubemap the cubemap to use
          * @sa Texture.loadCubamapFromFile() */
        SkyboxMaterial(const Texture& cubemap);

        virtual void use() override;

        virtual void after() override;

        virtual bool needsOrderedRendering() override {return true;};

        virtual float renderOrder(const glm::vec3& position) override;

		virtual std::size_t hash() const override;

		virtual bool equalsTo(const Material* rhs) const override;

        virtual ~SkyboxMaterial() = default;
};

#endif // SKYBOXMATERIAL_H
