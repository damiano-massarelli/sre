#ifndef LINEMATERIAL_H
#define LINEMATERIAL_H
#include "Material.h"

class LineMaterial : public Material
{
    public:
        LineMaterial();

        virtual void use() override;

        virtual void after() override;

        virtual ~LineMaterial();
};

#endif // LINEMATERIAL_H
