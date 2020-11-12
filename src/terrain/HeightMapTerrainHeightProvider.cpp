#include "terrain/HeightMapTerrainHeightProvider.h"
#include <glm/glm.hpp>
#include <iostream>
#include <stb_image.h>

HeightMapTerrainHeightProvider::HeightMapTerrainHeightProvider(
    const std::string& heightMapPath, float minHeight, float maxHeight)
    : mMinHeight{ minHeight }
    , mMaxHeight{ maxHeight } {
    stbi_set_flip_vertically_on_load(false);
    int cmp;
    mHeightData = stbi_load(heightMapPath.c_str(), &mWidth, &mHeight, &cmp, STBI_grey);
    if (mHeightData == nullptr)
        std::cerr << "Cannot read height map " << heightMapPath << "\n";
}

float HeightMapTerrainHeightProvider::get(float x, float z) const {
    if (mHeightData == nullptr)
        return 0.0f;

    // clamp
    if (x < 0)
        x = 0;
    if (x > 1)
        x = 1;

    if (z < 0)
        z = 0;
    if (z > 1)
        z = 1;

    const int zi = static_cast<int>(z * (mHeight - 1));
    const int xi = static_cast<int>(x * (mWidth - 1));
    const int index = zi * mWidth + xi;

    float f = (mHeightData[index] / 255.0f);
    return mMaxHeight * f + mMinHeight * (1.0f - f);
}

glm::vec3 HeightMapTerrainHeightProvider::getNormal(float x, float z, float terrainWidth, float terrainHeight) const {
    // get the position of nearby pixels
    const float heightL = get(x - 1.0F / mWidth, z);
    const float heightR = get(x + 1.0F / mWidth, z);
    const float heightU = get(x, z + 1.0F / mHeight);
    const float heightD = get(x, z - 1.0F / mHeight);

    const float widthDelta = (1.F / mWidth) * terrainWidth;
    const float heightDelta = (1.F / mHeight) * terrainHeight;

    const auto normal = glm::vec3{ (heightR - heightL) / widthDelta, 2.F, (heightD - heightU) / heightDelta };
    return glm::normalize(normal);
}

HeightMapTerrainHeightProvider::~HeightMapTerrainHeightProvider() {
    if (mHeightData)
        stbi_image_free(mHeightData);
}
