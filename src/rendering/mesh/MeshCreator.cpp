#include "MeshCreator.h"
#include "MeshLoader.h"
#include "LineMaterial.h"
#include <memory>
#include <cstdint>

float axisGizmoVertices[] {
    // position          // colors
    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    0.5f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

    0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,

    0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    0.0f, 0.0f, 0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f
};

GameObjectEH MeshCreator::axisGizmo()
{
    Mesh gizmo = MeshLoader::createMesh(axisGizmoVertices, 6, nullptr, 0, GL_LINES);
    return Engine::renderSys.createGameObject(gizmo, std::make_shared<LineMaterial>());
}
