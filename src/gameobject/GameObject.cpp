#include "gameobject/GameObject.h"
#include "Engine.h"

void GameObject::addMesh(const Mesh& mesh, const MaterialPtr& material)
{
    mMeshes.push_back(mesh);
    mMaterials.push_back(material);

	// new mesh added, we need to update the bb
	transform.updateMeshBoundingBox();
}

const std::vector<Mesh>& GameObject::getMeshes() const
{
	return mMeshes;
}

const std::vector<MaterialPtr>& GameObject::getMaterials() const
{
	return mMaterials;
}

void GameObject::removeAllMeshes()
{
	mMeshes.clear();
	mMaterials.clear();
	transform.updateMeshBoundingBox();
}

void GameObject::addComponent(const std::shared_ptr<Component>& component)
{
    mComponents.push_back(component);
}


GameObject::~GameObject()
{
    //dtor
}
