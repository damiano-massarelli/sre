#include "rendering/GameObjectRenderer.h"
#include "Engine.h"
#include "cameras/CameraComponent.h"
#include "geometry/BoundingBox.h"
#include "geometry/Frustum.h"
#include "geometry/Intersections.h"
#include "rendering/materials/Material.h"
#include "rendering/mesh/Mesh.h"
#include <glad/glad.h>
#include <map>
#include <tuple>
#include <unordered_map>

void GameObjectRenderer::draw(const Mesh* mesh) {
    glBindVertexArray(mesh->mVao);

    if (mesh->mUsesIndices)
        glDrawElements(mesh->mDrawMode, mesh->mIndicesNumber, GL_UNSIGNED_INT, (void*)0);
    else
        glDrawArrays(mesh->mDrawMode, 0, mesh->mVertexNumber);

    glBindVertexArray(0);
}

void GameObjectRenderer::drawMeshes(const std::unordered_map<Material*, std::vector<DrawData>>& material2mesh) {
    std::map<float, std::pair<Material*, DrawData>> orderedRender;
    for (const auto& [material, drawData] : material2mesh) {
        if (material->needsOrderedRendering()) {
            for (const auto& data : drawData) {
                float order = material->renderOrder(data.position);
                orderedRender[order] = std::make_pair(material, data);
            }
        } else {
            material->use();
            for (const auto& data : drawData) {
                material->shader.setMat4(material->getModelLocation(), data.toWorld);
                material->shader.setMat3(material->getNormalModelLocation(), data.toWorldForNormals);
                draw(data.mesh);
            }

            material->after();
        }
    }

    // render meshed that need to be rendered in a specific order
    for (auto ord = orderedRender.rbegin(); ord != orderedRender.rend(); ++ord) {
        auto& [material, drawData] = ord->second;
        material->use();
        material->shader.setMat4(material->getModelLocation(), drawData.toWorld);
        material->shader.setMat3(material->getNormalModelLocation(), drawData.toWorldForNormals);
        draw(drawData.mesh);
        material->after();
    }
}

void GameObjectRenderer::render() {
    std::map<float, std::tuple<Mesh, MaterialPtr, glm::mat4, glm::mat4>> orderedRender;

    std::unordered_map<Material*, std::vector<DrawData>> material2mesh;

    const Frustum cameraFrustum = Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getViewFrutsum();
    for (auto& go : Engine::gameObjectManager.mGameObjects) {
        const BoundingBox goBB = go.transform.getBoundingBox();
        if (goBB.isValid() && boxFrustumIntersection(goBB, cameraFrustum) == IntersectionTestResult::OUTSIDE) {
            continue;
        }

        for (std::size_t meshIndex = 0; meshIndex < go.mMeshes.size(); meshIndex++) {

            Material* meshMaterial = go.mMaterials[meshIndex].get();

            const Mesh* mesh = &(go.mMeshes[meshIndex]);

            // do not render this mesh if its material does not support the
            // current render phase
            if ((meshMaterial->unSupportedRenderPhases & Engine::renderSys.getRenderPhase()) != RenderDomain::NONE)
                continue;

            // use a default material if required
            meshMaterial = mForcedMaterial ? mForcedMaterial.get() : meshMaterial;

            auto it = material2mesh.find(meshMaterial);
            if (it == material2mesh.end()) {
                material2mesh[meshMaterial] = { { mesh,
                    go.transform.modelToWorld(),
                    go.transform.modelToWorldForNormals(),
                    go.transform.getPosition() } };
                material2mesh[meshMaterial].reserve(50);
            } else {
                it->second.push_back({ mesh,
                    go.transform.modelToWorld(),
                    go.transform.modelToWorldForNormals(),
                    go.transform.getPosition() });
            }
        }
    }

    drawMeshes(material2mesh);
}

void GameObjectRenderer::forceMaterial(const MaterialPtr& material) {
    mForcedMaterial = material;
}
