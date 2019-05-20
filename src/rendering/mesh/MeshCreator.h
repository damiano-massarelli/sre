#ifndef MESHCREATOR_H
#define MESHCREATOR_H
#include "Engine.h"
#include "Mesh.h"

class MeshCreator
{
    public:
        /**
          * Creates a game object representing axis gizmo */
        static GameObjectEH axisGizmo();

        static Mesh cube();
};

#endif // MESHCREATOR_H
