#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H
#include "GameObject.h"
#include "Material.h"

class GameObjectRenderer
{
friend class Engine;

private:
	/**
	 * When this material is set it will be used to draw meshes.
	 * This is useful when rendering for shadows and so on */
	MaterialPtr mForcedMaterial = nullptr;

    /** Actually renders a Mesh its corresponding material should be in use */
    void draw(const Mesh& mesh);

    GameObjectRenderer() = default;

public:
    GameObjectRenderer(const GameObjectRenderer& gor) = delete;

    GameObjectRenderer& operator=(const GameObjectRenderer& gor) = delete;

    /**
	 * Renders a list of GameObject%s.
	 * @param gameObjects the GameObject%s to render */
    void render(const std::vector<GameObject>& gameObjects);

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
