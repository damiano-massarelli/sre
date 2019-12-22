#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "HandleList.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <cstdint>
#include <vector>
#include <memory>

class Component;

class GameObject
{
    friend class GameObjectRenderer;

    private:
        std::vector<std::shared_ptr<Component>> mComponents;

        std::vector<Mesh> mMeshes;
        std::vector<MaterialPtr> mMaterials;

    public:
		/** position, scale and orientation of this GameObject */
        Transform transform;

		/** the name of this GameObject */
        std::string name = "gameobject";

        GameObject() = default;

        void addMesh(const Mesh& mesh, const MaterialPtr& material);

		/**
		 * @return the meshes used by this GameObject
		 */
		const std::vector<Mesh>& getMeshes() const;

		/**
		 * @return the materials used by this GameObject
		 */
		const std::vector<MaterialPtr>& getMaterials() const;

		/**
		 * Removes all the Mesh%es of this GameObject;
		 */
		void removeAllMeshes();

        /**
          * Adds a component to this game object.
          * @param component the component to add (a shared_ptr to it) */
        void addComponent(const std::shared_ptr<Component>& component);

        /**
          * Gets the first component of type T
          * @tparam T the type of the component to get
          * @return a component of the specified type (a shared_ptr to it), nullptr if no component is found */
        template <typename T>
        std::shared_ptr<T> getComponent() {
            for (auto& cmp : mComponents) {
                std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(cmp);
                if (cast != nullptr)
                    return cast;
            }
            return nullptr;
        }


        virtual ~GameObject();
};

#endif // GAMEOBJECT_H
