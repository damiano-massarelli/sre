#ifndef TERRAINHEIGHTPROVIDER_H
#define TERRAINHEIGHTPROVIDER_H
#include <glm/vec3.hpp>

class TerrainHeightProvider {
public:
    TerrainHeightProvider();

    virtual float get(float x, float z) const = 0;

    virtual glm::vec3 getNormal(float x, float z, float terrainWidth, float terrainHeight) const = 0;

    virtual ~TerrainHeightProvider();
};

#endif  // TERRAINHEIGHTPROVIDER_H
