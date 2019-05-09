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

void GameObject::addComponent(const std::shared_ptr<Component>& component)
{
    mComponents.push_back(component);
}

void GameObject::remove()
{
    // informs the parent that it is not its child anymore
    transform.removeParent();

    for (auto& child : transform.getChildren())
        Engine::renderSys.remove(child);

    // removes all meshes from memory
    for (auto& mesh : mMeshes)
        mesh.cleanUp();
}

GameObject::~GameObject()
{
    //dtor
}
