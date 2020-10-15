#include "rendering/mesh/MeshCreator.h"
#include "Engine.h"
#include "geometry/BoundingBox.h"
#include "rendering/materials/LineMaterial.h"
#include "rendering/mesh/MeshLoader.h"
#include <cmath>
#include <cstdint>
#include <memory>

constexpr float pi = 3.14159265359f;

float axisGizmoVertices[]{
    // position          // colors			 // uvs       // tangents
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,

    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,

    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
};

static float cubeVertices[]{
    // positions          // normals           // uvs        // tangents
    // back
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.0f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,

    // front
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,

    // left
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,

    // right
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,

    // bottom
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    -0.5f,

    // top
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
};

GameObjectEH MeshCreator::axisGizmo() {
    Mesh gizmo = MeshLoader::createMesh(axisGizmoVertices, 6, nullptr, 0, GL_LINES);
    auto go = Engine::gameObjectManager.createGameObject(gizmo, std::make_shared<LineMaterial>());
    go->name = "axisGizmo";
    return go;
}

Mesh MeshCreator::cube() {
    Mesh cubeMesh = MeshLoader::createMesh(cubeVertices, 36, nullptr, 0);
    return cubeMesh;
}

Mesh MeshCreator::cylinder(float radius, std::uint32_t resolution) {
    std::vector<std::uint32_t> indices;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> tangents;

    // bottom face
    float slice = (2 * pi) / resolution;
    for (std::uint32_t i = 0; i < resolution; ++i) {
        float x = radius * std::cos(slice * i);
        float z = radius * std::sin(slice * i);

        positions.insert(positions.end(), { x, -.5f, z });

        normals.insert(normals.end(), { 0.0f, -1.0f, 0.0f });

        tangents.insert(tangents.end(), { 1.f, 0.f, 0.f });

        uvs.insert(uvs.end(), { (x + 1) / 2, (z + 1) / 2 });

        indices.insert(indices.end(), { (i + 1) % resolution, resolution, i });
    }

    positions.insert(positions.end(), { 0.0f, -.5f, 0.0f });
    normals.insert(normals.end(), { 0.0f, -1.0f, 0.0f });
    uvs.insert(uvs.end(), { 0.5f, 0.5f });
    tangents.insert(tangents.end(), { 1.f, 0.f, 0.f });

    // top face
    std::uint32_t current = resolution + 1;
    for (std::uint32_t i = 0; i < resolution; ++i) {
        float x = radius * std::cos(slice * i);
        float z = radius * std::sin(slice * i);

        positions.insert(positions.end(), { x, .5f, z });

        normals.insert(normals.end(), { 0.0f, 1.0f, 0.0f });

        tangents.insert(tangents.end(), { 1.f, 0.f, 0.f });

        uvs.insert(uvs.end(), { (x + 1) / 2, 1 - (z + 1) / 2 });

        indices.insert(indices.end(), { i + current, resolution + current, (i + 1) % resolution + current });
    }

    positions.insert(positions.end(), { 0.0f, .5f, 0.0f });
    normals.insert(normals.end(), { 0.0f, 1.0f, 0.0f });
    uvs.insert(uvs.end(), { 0.5f, 0.5f });
    tangents.insert(tangents.end(), { 1.f, 0.f, 0.f });

    // body
    current = 2 * resolution + 2;
    for (std::uint32_t i = 0; i <= resolution; ++i) {
        const float cos = std::cos(slice * i);
        const float sin = std::sin(slice * i);
        const float x = radius * cos;
        const float z = radius * sin;
        const float xNext = std::cos(slice * (i + 1)) * radius;
        const float zNext = std::sin(slice * (i + 1)) * radius;

        positions.insert(positions.end(), { x, .5f, z });
        positions.insert(positions.end(), { x, -.5f, z });

        normals.insert(normals.end(), { cos, 0.0f, sin });
        normals.insert(normals.end(), { cos, 0.0f, sin });
        tangents.insert(tangents.end(), { xNext - x, 0.f, zNext - z });
        tangents.insert(tangents.end(), { xNext - x, 0.f, zNext - z });

        uvs.insert(uvs.end(), { 1.f - static_cast<float>(i) / resolution, 1.0f });
        uvs.insert(uvs.end(), { 1.f - static_cast<float>(i) / resolution, 0.0f });

        if (i != resolution) {
            indices.insert(indices.end(), { (i + 1) * 2 + current, (i * 2 + 1) + current, i * 2 + current });

            indices.insert(indices.end(), { (i + 1) * 2 + 1 + current, (i * 2 + 1) + current, (i + 1) * 2 + current });
        }
    }

    // Order matter: check the vertex buffer layout in the PBR VS
    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(uvs.data(), uvs.size(), 2);
    loader.loadData(tangents.data(), tangents.size(), 3);
    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, false);

    return loader.getMesh(0, static_cast<std::uint32_t>(indices.size()));
}

Mesh MeshCreator::cone(float radius, std::uint32_t resolution) {
    std::vector<std::uint32_t> indices;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> uvs;

    // bottom face
    float slice = (2 * pi) / resolution;
    for (std::uint32_t i = 0; i < resolution; ++i) {
        float x = radius * std::cos(slice * i);
        float z = radius * std::sin(slice * i);

        positions.insert(positions.end(), { x, -.5f, z });

        normals.insert(normals.end(), { 0.0f, -1.0f, 0.0f });

        uvs.insert(uvs.end(), { (x + 1) / 2, (z + 1) / 2 });

        tangents.insert(tangents.end(), { 1.f, 0.f, 0.f });

        indices.insert(indices.end(), { (i + 1) % resolution, resolution, i });
    }

    positions.insert(positions.end(), { 0.0f, -.5f, 0.0f });
    normals.insert(normals.end(), { 0.0f, -1.0f, 0.0f });
    uvs.insert(uvs.end(), { 0.5f, 0.5f });
    tangents.insert(tangents.end(), { 1.f, 0.f, 0.f });

    const float bodyNormalY = std::sin(pi / 2 - std::atan2(1.0f, radius));

    // body
    std::uint32_t current = resolution + 1;
    for (std::uint32_t i = 0; i <= resolution; ++i) {
        const float cos = std::cos(slice * i);
        const float sin = std::sin(slice * i);
        const float x = radius * cos;
        const float z = radius * sin;
        const float xNext = radius * std::cos(slice * (i + 1));
        const float zNext = radius * std::sin(slice * (i + 1));

        positions.insert(positions.end(), { x, -.5f, z });
        positions.insert(positions.end(), { 0.f, .5f, 0.f });

        glm::vec3 normal{ cos, bodyNormalY, sin };
        normal = glm::normalize(normal);
        normals.insert(normals.end(), { normal.x, normal.y, normal.z });
        normals.insert(normals.end(), { 0.f, 1.f, 0.f });

        tangents.insert(tangents.end(), { xNext - x, 0.f, zNext - z });
        tangents.insert(tangents.end(), { xNext - x, 0.f, zNext - z });

        uvs.insert(uvs.end(), { (float)i / resolution, 1.0f });
        uvs.insert(uvs.end(), { 0.5f, 0.5f });

        if (i != resolution) {
            indices.insert(indices.end(),
                { i * 2 + current,
                    (i * 2 + 1) + current,  // top
                    (i * 2 + 2) + current });
        }
    }

    // Order matter: check the vertex buffer layout in the PBR VS
    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(uvs.data(), uvs.size(), 2);
    loader.loadData(tangents.data(), tangents.size(), 3);
    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, false);

    return loader.getMesh(0, static_cast<std::uint32_t>(indices.size()));
}

Mesh MeshCreator::sphere(float radius, std::uint32_t sectors, std::uint32_t stacks) {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> tangents;

    std::vector<std::uint32_t> indices;

    for (std::uint32_t i = 0; i < stacks; ++i) {
        float phi = -pi / 2 + pi * ((float)i / (stacks - 1));
        float y = radius * std::sin(phi);

        for (std::uint32_t j = 0; j <= sectors; ++j) {
            float theta = 2 * pi * ((float)j / sectors);

            float x = radius * std::cos(phi) * std::cos(theta);
            float z = radius * std::cos(phi) * std::sin(theta);

            positions.insert(positions.end(), { x, y, z });
            normals.insert(normals.end(), { x, y, z });
            uvs.insert(uvs.end(), { 1.f - static_cast<float>(j) / sectors, static_cast<float>(i) / (stacks - 1) });

            float thetaNext = 2 * pi * ((float)(j + 1) / sectors);

            float xNext = radius * std::cos(phi) * std::cos(thetaNext);
            float zNext = radius * std::cos(phi) * std::sin(thetaNext);

            tangents.insert(tangents.end(), { xNext - x, 0.0f, zNext - z });

            if (j != sectors && i != stacks - 1) {
                indices.insert(indices.end(),
                    { // first triangle
                        j + i * (sectors + 1),
                        j + (i + 1) * (sectors + 1),
                        j + 1 + i * (sectors + 1),

                        // second triangle
                        j + 1 + i * (sectors + 1),
                        j + (i + 1) * (sectors + 1),
                        j + 1 + (i + 1) * (sectors + 1) });
            }
        }
    }

    // Order matter: check the vertex buffer layout in the PBR VS
    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(uvs.data(), uvs.size(), 2);
    loader.loadData(tangents.data(), tangents.size(), 3);
    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, false);

    return loader.getMesh(0, static_cast<std::uint32_t>(indices.size()));
}

Mesh MeshCreator::plane() {
    std::vector<float> positions{ .5f, .5f, 0.0f, .5f, -.5f, 0.0f, -.5f, -.5f, 0.0f, -.5f, .5f, 0.0f };

    std::vector<float> uvs{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

    std::vector<float> normals{
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
    };

    std::vector<std::uint32_t> indices{ 2, 1, 0, 2, 0, 3 };

    std::vector<float> tangents{
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
    };

    // Order matter: check the vertex buffer layout in the PBR VS
    MeshLoader loader;
    loader.loadData(positions.data(), positions.size(), 3);
    loader.loadData(normals.data(), normals.size(), 3);
    loader.loadData(uvs.data(), uvs.size(), 2);
    loader.loadData(tangents.data(), tangents.size(), 3);

    loader.loadData(indices.data(), indices.size(), 0, GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, false);

    return loader.getMesh(0, static_cast<std::uint32_t>(indices.size()));
}
