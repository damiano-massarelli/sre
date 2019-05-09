#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader.h"
#include <string>
#include <memory>
#include <vector>

class Material
{
    public:
        Shader shader;

        Material(const std::string& vertexShader, const std::string& fragmentShader);

        Material(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& fragmentShaders);

        /**
          * Sets this material as the current material for rendering
          */
        virtual void use() = 0;


        virtual ~Material();
};

using MaterialPtr = std::shared_ptr<Material>;

#endif // MATERIAL_H
