#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H
#include "GameObjectEH.h"
#include "Mesh.h"
#include "TerrainHeightProvider.h"
#include "GeoMipMappingComponent.h"
#include <cstdint>

/** Generates Mesh%es that can be used as terrain.
  * It is possible to specify the width and the depth of
  * the terrain as well as the number of vertical and horizontal
  * vertices composing it.
  * The height of each vertex is provided using a subcalss of TerrainHeightProvider
  * @sa TerrainHeightProvider
  * @sa HeightMapTerrainHeightProvider */
class TerrainGenerator
{
    private:
        /// number of Horizontal vertices
        std::uint32_t mHVertex;

        /// number of Vertical vertices
        std::uint32_t mVVertex;

        /// width of the map
        std::uint32_t mWidth;

        /// depth of the map
        std::uint32_t mDepth;

        /// how many times the terrain texture is repeated horizontally
        float mHTerrainTexutreTiles = 40;

        /// how many times the terrain texture is repeated vertically
        float mVTerrainTextureTiles = 40;

    public:
        TerrainGenerator(std::uint32_t hVertex = 50, std::uint32_t vVertex = 50, std::uint32_t width = 50, std::uint32_t depth = 50);

        void setTextureTilesNumber(float horizontal, float vertical);

        Mesh createTerrain(const TerrainHeightProvider& heightProvider);

		void addGeoMipMapComponent(const GameObjectEH& go);

		virtual ~TerrainGenerator() = default;
};

#endif // TERRAINGENERATOR_H
