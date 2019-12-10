#include "DisplayBoundingBoxComponent.h"
#include "Engine.h"
#include "MeshCreator.h"
#include "PropMaterial.h"
#include <memory>

DisplayBoundingBoxComponent::DisplayBoundingBoxComponent(const GameObjectEH& go, const glm::vec3& bbColor)
	: Component{go}
{
	Mesh cube = MeshCreator::cube();

	// prop material in wireframe mode
	std::shared_ptr<PropMaterial> material = std::make_shared<PropMaterial>(true, false);
	material->color = bbColor;

	mBoundingBox = Engine::gameObjectManager.createGameObject(cube, material);

	mEnterFrameCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this);
}

void DisplayBoundingBoxComponent::onEvent(SDL_Event e)
{
	auto bb = gameObject->transform.getBoundingBox();

	mBoundingBox->transform.setPosition(gameObject->transform.getBoundingBox().getCenter());
	mBoundingBox->transform.setScale(gameObject->transform.getBoundingBox().getExtent() / 2.0f);
}

DisplayBoundingBoxComponent::~DisplayBoundingBoxComponent()
{
}
