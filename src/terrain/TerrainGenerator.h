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

	bool mIncludeTangentSpace;

public:
	/**
	 * Sets up a terrain generator.
	 * @param hVertex the number of horizontal vertices
	 * @param vVertex the number of vertical vertices
	 * @pram width the width of the terrain
	 * @param depth the depth of the terrain
	 */
    TerrainGenerator(std::uint32_t hVertex = 50, std::uint32_t vVertex = 50, std::uint32_t width = 50, std::uint32_t depth = 50);

	/**
	 * Sets the number of times the texture of the ground is repeated.
	 * @param horizontal the number of times the texture is repeated horizontally
	 * @param vertical the number of times the texture is repeated vertically
	 */
    void setTextureTilesNumber(float horizontal, float vertical);

	/**
	 * Creates the terrain Mesh.
	 * @param heightProvider a TerrainHeightProvider that specifies the height of the terrain.
	 * @return the terrain Mesh
	 */
    Mesh createTerrain(const TerrainHeightProvider& heightProvider);

	/**
	 * Specifies whether tangent space data should be included.
	 * @param include whether we want tangent space data or not
	 */
	void includeTangentSpace(bool include);

	void addGeoMipMapComponent(const GameObjectEH& go);

	virtual ~TerrainGenerator() = default;
};

#endif // TERRAINGENERATOR_H
