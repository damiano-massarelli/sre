#include "DisplayCameraFrustumComponent.h"
#include "cameras/CameraComponent.h"
#include "Engine.h"
#include "rendering/mesh/MeshLoader.h"
#include <algorithm>
#include <vector>

DisplayCameraFrustumComponent::DisplayCameraFrustumComponent(const GameObjectEH& go, const glm::vec3& color)
	: Component{ go }
{
	// prop material in wireframe mode
	mMaterial = std::make_shared<PropMaterial>(true, false);
	mMaterial->color = color;

	mContainer = Engine::gameObjectManager.createGameObject();

	mEnterFrameCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
}

void DisplayCameraFrustumComponent::onEvent(SDL_Event e)
{
	auto cameraComponent = gameObject->getComponent<CameraComponent>();
	if (!cameraComponent) {
		return;
	}
	
	const std::array<glm::vec3, 8>& vertices = cameraComponent->getViewFrutsum().getVertices();
	std::vector<float> positions;
	std::vector<float> normals;

	// converts the array of vectors to arrays of floats
	std::for_each(vertices.begin(), vertices.end(), [&positions, &normals](auto& pos) {
		positions.insert(positions.end(), {pos.x, pos.y, pos.z});
		normals.insert(normals.end(), { 0.0f, 1.0f, 0.0f });
	});

	// indices of all triangles composing the frustum mesh
	constexpr std::array<std::uint32_t, 36> indices{
		3, 1, 2, // far
		3, 2, 0, // far

		5, 7, 4, // near
		5, 4, 6, // near

		6, 4, 0, // top
		6, 0, 2, // top

		1, 3, 7, // bottom
		1, 7, 5, // bottom

		1, 5, 6, // left
		1, 6, 2, // left

		7, 3, 0, // right
		7, 0, 4  // right
	};

	MeshLoader loader;
	loader.loadData(positions.data(), positions.size(), 3); // positions
	loader.loadData(normals.data(), normals.size(), 3); // normals just because they are needed by the shader
	loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, false);

	Mesh mesh = loader.getMesh(0, static_cast<std::uint32_t>(indices.size()));

	mContainer->removeAllMeshes();
	mContainer->addMesh(mesh, mMaterial);
}
