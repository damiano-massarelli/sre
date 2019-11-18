#ifndef HEIGHTMAPTERRAINHEIGHTPROVIDER_H
#define HEIGHTMAPTERRAINHEIGHTPROVIDER_H
#include "terrain/TerrainHeightProvider.h"
#include <cstdint>
#include <string>
#include <glm/vec3.hpp>

class HeightMapTerrainHeightProvider : public TerrainHeightProvider
{
    private:
        int mWidth = 0;
        int mHeight = 0;

        float mMinHeight;
        float mMaxHeight;

        std::uint8_t* mHeightData;

    public:
        HeightMapTerrainHeightProvider(const std::string& heightMapPath, float minHeight = 0.0f, float maxHeight = 1.0f);

        virtual float get(float x, float z) const override;

        virtual glm::vec3 getNormal(float x, float z) const override;

        ~HeightMapTerrainHeightProvider();
};

#endif // HEIGHTMAPTERRAINHEIGHTPROVIDER_H
