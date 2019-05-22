#include "TerrainGenerator.h"
#include "MeshLoader.h"
#include <vector>
#include <glm/common.hpp>

TerrainGenerator::TerrainGenerator(std::uint32_t hVertex, std::uint32_t vVertex, std::uint32_t width, std::uint32_t depth)
    : mHVertex{hVertex}, mVVertex{vVertex}, mWidth{width}, mDepth{depth}
{

}

void TerrainGenerator::setTextureTilesNumber(float horizontal, float vertical)
{
    mHTerrainTexutreTiles = horizontal;
    mVTerrainTextureTiles = vertical;
}

Mesh TerrainGenerator::createTerrain(const TerrainHeightProvider& heightProvider)
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;

    std::vector<std::uint32_t> indices;

    for (std::uint32_t v = 0; v <= mVVertex; ++v) {
        float vPercent = (float) v / mVVertex;
        float vPos = -(mDepth / 2.0f) + mDepth * vPercent;
        for (std::uint32_t h = 0; h <= mHVertex; ++h) {
            float hPercent = (float) h / mHVertex;
            float hPos = -(mWidth / 2.0f) + mWidth * hPercent;

            positions.insert(positions.end(), {hPos, heightProvider.get(hPercent, vPercent), vPos});

            glm::vec3 normal = heightProvider.getNormal(hPercent, vPercent);
            normals.insert(normals.end(), {normal.x, normal.y, normal.z});
            uvs.insert(uvs.end(), {hPercent * mHTerrainTexutreTiles, 1 - vPercent * mVTerrainTextureTiles});

            if (v != mVVertex && h != mHVertex) {
                indices.insert(indices.end(), {
                               // first tri
                               h + (mHVertex + 1) * v,
                               h + (mHVertex + 1) * (v + 1),
                               h + 1 + (mHVertex + 1) * v,

                               // second tri
                               h + 1 + (mVVertex + 1) * v,
                               h + (mVVertex + 1) * (v + 1),
                               h + 1 + (mVVertex + 1) * (v + 1),
                            });
            }
        }
    }

    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(uvs.data(), uvs.size(), 2);
    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT);

    return loader.getMesh(0, indices.size());
}
