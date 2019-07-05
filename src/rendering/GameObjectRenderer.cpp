#include "GameObjectRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"
#include <map>
#include <tuple>
#include <glad/glad.h>

void GameObjectRenderer::draw(const Mesh& mesh)
{
    glBindVertexArray(mesh.mVao);

    if (mesh.mUsesIndices)
        glDrawElements(mesh.mDrawMode, mesh.mIndicesNumber, GL_UNSIGNED_INT, (void *)0);
    else
        glDrawArrays(mesh.mDrawMode, 0, mesh.mVertexNumber);

    glBindVertexArray(0);
}

void GameObjectRenderer::render(const std::vector<GameObject>& gameObjects)
{
    std::map<float, std::tuple<Mesh, MaterialPtr, glm::mat4>> orderedRender;
    for (auto const& go : gameObjects) {
        for (std::size_t meshIndex = 0; meshIndex < go.mMeshes.size(); meshIndex++) {

			const MaterialPtr& meshMaterial = go.mMaterials[meshIndex];

			// do not render this mesh if its material does not support the current render phase
			if ((meshMaterial->unSupportedRenderPhases & Engine::renderSys.getRenderPhase()))
				continue;

			// if mForcedMaterial != nullptr then use it
			const MaterialPtr& material = mForcedMaterial ? mForcedMaterial : meshMaterial;
            const Mesh& mesh = go.mMeshes[meshIndex];
            if (material) {
				if (material->needsOrderedRendering()) {
					float order = material->renderOrder(go.transform.getPosition());
					orderedRender[order] = std::make_tuple(mesh, material, go.transform.modelToWorld());
					continue;
				}
				else {
					Material* mat = material.get();

					mat->use();
					mat->shader.setMat4(mat->getModelLocation(), go.transform.modelToWorld());
					draw(mesh);
					mat->after();
				}
            }            
        }
    }

    // render meshed that need to be rendered in a specific order
    for (auto ord = orderedRender.rbegin(); ord != orderedRender.rend(); ++ord) {
        auto& [mesh, mat, toWorld] = ord->second;
		mat->use();
		mat->shader.setMat4(mat->getModelLocation(), toWorld);
        draw(mesh);
		mat->after();
    }
}

void GameObjectRenderer::forceMaterial(const MaterialPtr& material)
{
	mForcedMaterial = material;
}
