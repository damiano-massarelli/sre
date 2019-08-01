#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H
#include "GameObject.h"
#include "Material.h"

/*
* Add hash and equal to so that they can be
* used in unordered_map
*/
namespace std {

	template <>
	struct hash<Material*> {
		std::size_t operator()(const Material* material) const {
			return material->hash();
		}
	};

	template<>
	struct equal_to<Material*> {
		bool operator()(const Material* lhs, const Material* rhs) const {
			return lhs->equalsTo(rhs);
		}
	};

}

class GameObjectRenderer
{
friend class Engine;

private:
	struct DrawData {
		const Mesh* mesh;
		glm::mat4 toWorld;
		glm::mat4 toWorldForNormals;
		glm::vec3 position;
	};

	/**
	 * When this material is set it will be used to draw meshes.
	 * This is useful when rendering for shadows and so on */
	MaterialPtr mForcedMaterial = nullptr;

    /** Actually renders a Mesh its corresponding material should be in use */
    void draw(const Mesh* mesh);

    GameObjectRenderer() = default;

	void drawMeshes(const std::unordered_map<Material*, std::vector<DrawData>>& material2mesh);

public:
    GameObjectRenderer(const GameObjectRenderer& gor) = delete;

    GameObjectRenderer& operator=(const GameObjectRenderer& gor) = delete;

    /**
	 * Renders all the GameObject%s managed by GameObjectManager. */
    void render();

	/**
	 * Set the material to use for rendering.
	 * If this material is nullptr then the specific material
	 * of each mesh is used.
	 * @param material the material to use for rendering (nullptr to disable)
	 */
	void forceMaterial(const MaterialPtr& material);

    virtual ~GameObjectRenderer() = default;
};

#endif // GAMEOBJECTRENDERER_H
