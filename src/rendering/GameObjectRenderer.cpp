#include "GameObjectRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"
#include <map>
#include <glad/glad.h>

void GameObjectRenderer::draw(Mesh mesh, MaterialPtr material)
{
    material->use();
    glBindVertexArray(mesh.mVao);

    if (mesh.mUsesIndices)
        glDrawElements(mesh.mDrawMode, mesh.mIndicesNumber, GL_UNSIGNED_INT, (void *)0);
    else
        glDrawArrays(mesh.mDrawMode, 0, mesh.mVertexNumber);

    glBindVertexArray(0);

    material->after();
}
#include <iostream>
void GameObjectRenderer::render(const std::vector<GameObject>& gameObjects)
{
    std::map<float, std::pair<Mesh, MaterialPtr>> orderedRender;
    for (auto const& go : gameObjects) {
        for (std::size_t meshIndex = 0; meshIndex < go.mMeshes.size(); meshIndex++) {

			const MaterialPtr& meshMaterial = go.mMaterials[meshIndex];

			// do not render this mesh if its material does not support the current render phase
			if ((meshMaterial->supportedRenderPhases & Engine::renderSys.getRenderPhase()) == 0)
				continue;

			// if mForcedMaterial != nullptr then use it
			const MaterialPtr& material = mForcedMaterial ? mForcedMaterial : meshMaterial;
            const Mesh& mesh = go.mMeshes[meshIndex];
            if (material) {
                Material* mat = material.get();

				// TODO optimization draw should set the model transform matrix
                mat->use();
                mat->shader.setMat4(mat->getModelLocation(), go.transform.modelToWorld());
				mat->after();
            }

            if (material->needsOrderedRendering()) {
                float order = material->renderOrder(go.transform.getPosition());
                orderedRender[order] = std::make_pair(mesh, material);
                continue;
            }

            draw(mesh, material);
        }
    }

    // render meshed that need to be rendered in a specific order
    for (auto ord = orderedRender.rbegin(); ord != orderedRender.rend(); ++ord) {
        auto data = ord->second;
        auto mesh = data.first;
        auto material = data.second;

        draw(mesh, material);
    }
}

void GameObjectRenderer::forceMaterial(const MaterialPtr& material)
{
	mForcedMaterial = material;
}
