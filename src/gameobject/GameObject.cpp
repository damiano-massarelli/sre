#include "GameObject.h"
#include "Engine.h"

GameObject::GameObject(const Mesh& mesh, MaterialPtr material)
{
    mMeshes.push_back(mesh);
    mMaterials.push_back(material);
}

void GameObject::addMesh(const Mesh& mesh, const MaterialPtr& material)
{
    mMeshes.push_back(mesh);
    mMaterials.push_back(material);
}

const std::vector<Mesh>& GameObject::getMeshes() const
{
	return mMeshes;
}

void GameObject::addComponent(const std::shared_ptr<Component>& component)
{
    mComponents.push_back(component);
}

void GameObject::cleanUp()
{

}

GameObject::~GameObject()
{
    //dtor
}
