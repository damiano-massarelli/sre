#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H
#include "Material.h"
#include "Texture.h"
#include <glm/common.hpp>
#include <memory>

class BlinnPhongMaterial : public Material
{
    private:
        Texture diffuseMap;
        Texture specularMap;

    public:
        BlinnPhongMaterial();

        void setDiffuseMap(const Texture& texture);
        void setSpecularMap(const Texture& texture);

        glm::vec3 diffuseColor{1.0f, 1.0f, 1.0f};
        glm::vec3 specularColor{1.0f, 1.0f, 1.0f};
        float shininess = 32.0f;

        float opacity = 1.0f;

        virtual void use() override;

        virtual void after() override;

        virtual bool needsOrderedRendering() override;

        virtual float renderOrder(const glm::vec3& position) override;

        virtual ~BlinnPhongMaterial();
};

using BlinnPhongMaterialPtr = std::shared_ptr<BlinnPhongMaterial>;

class BlinnPhongMaterialBuilder {
    private:
        std::string diffuseMapPath;
        std::string specularMapPath;

        Texture diffuseMap;
        Texture specularMap;

        bool specularColorSet = false;
        glm::vec3 specularColor{1.0f, 1.0f, 1.0f};

        bool diffuseColorSet = false;
        glm::vec3 diffuseColor{1.0f, 1.0f, 1.0f};

        float shininess = 32;

    public:
        BlinnPhongMaterialPtr build() {
            auto material = std::make_shared<BlinnPhongMaterial>();

            if (diffuseMapPath != "")
                material->setDiffuseMap(Texture::loadFromFile(diffuseMapPath));

            if (diffuseMap) {
                material->setDiffuseMap(diffuseMap);
                diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
            }

            if (specularMapPath != "") {
                material->setSpecularMap(Texture::loadFromFile(specularMapPath));
            }

            if (specularMap) {
                material->setSpecularMap(specularMap);
                specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
            }

            material->diffuseColor = diffuseColor;
            material->specularColor = specularColor;
            material->shininess = shininess;

            return material;
        }

        BlinnPhongMaterialBuilder& setDiffuseMap(const std::string& path) {
            diffuseMapPath = path;
            return *this;
        }

        BlinnPhongMaterialBuilder& setDiffuseMap(const Texture& texture) {
            diffuseMap = texture;
            return *this;
        }

        BlinnPhongMaterialBuilder& setSpecularMap(const std::string& path) {
            specularMapPath = path;
            return *this;
        }

        BlinnPhongMaterialBuilder& setSpecularMap(const Texture& texture) {
            specularMap = texture;
            return *this;
        }

        BlinnPhongMaterialBuilder& setShininess(float s) {
            shininess = s;
            return *this;
        }

        BlinnPhongMaterialBuilder& setDiffuseColor(const glm::vec3& color) {
            diffuseColor = color;
            diffuseColorSet = true;
            return *this;
        }

        BlinnPhongMaterialBuilder& setSpecularColor(const glm::vec3& color) {
            specularColor = color;
            specularColorSet = true;
            return *this;
        }
};

#endif // PHONGMATERIAL_H
