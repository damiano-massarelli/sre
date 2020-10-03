#ifndef MESHCREATOR_H
#define MESHCREATOR_H
#include "rendering/mesh/Mesh.h"
#include "gameobject/GameObjectEH.h"
#include <cstdint>



class MeshCreator
{
public:
    MeshCreator() = delete;

    /**
      * Creates a game object representing axis gizmo */
    static GameObjectEH axisGizmo();

    /**
      * Creates a new cube Mesh.
      * @return a cube Mesh */
    static Mesh cube();

    /**
      * Creates a new cylinder Mesh.
      * @param radius the radius of the cylinder
      * @param resolution the number of slices composing the cylinder
      * @return a cylinder Mesh */
    static Mesh cylinder(float radius = 0.5f, std::uint32_t resolution = 50);

    /**
      * Creates a new cone Mesh.
      * @param radius the radius of the cone
      * @param resolution the number of slices composing the cone
      * @return a cone Mesh */
    static Mesh cone(float radius = 0.5f, std::uint32_t resolution = 50);

    /**
      * Creates a new sphere Mesh.
      * @param radius the radius of the sphere
      * @param sectors the number of horizontal subdivisions
      * @param stacks the number of vertical subdivisions
      * @return a sphere Mesh */
    static Mesh sphere(float radius = 0.5f, std::uint32_t sectors = 20, std::uint32_t stacks = 20);

	/**
	  * Creates a plane.
      * @return a plane Mesh
      */
	static Mesh plane();
};

#endif // MESHCREATOR_H
