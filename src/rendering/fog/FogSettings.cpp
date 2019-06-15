#include "FogSettings.h"
#include <glad/glad.h>
#include "RenderSystem.h"
#include <glm/gtc/type_ptr.hpp>

void FogSettings::setFogColor(const glm::vec3& fogColor)
{
	mFogColor = fogColor;
	updateUbo();
}

void FogSettings::setInverseDistance(float invDistance)
{
	mInvereDistance = invDistance;
	updateUbo();
}

void FogSettings::setRapidity(float rapidity)
{
	mRapidity = rapidity;
	updateUbo();
}

void FogSettings::updateUbo()
{
	glBindBuffer(GL_UNIFORM_BUFFER, mFogUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(mFogColor));

	glm::vec2 fogParams{ mInvereDistance, mRapidity };
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec2), glm::value_ptr(fogParams));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void FogSettings::init()
{
	/* Uniform buffer object set up for fog */
	glGenBuffers(1, &mFogUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, mFogUbo);
	// 32 bytes: a vec3 (16) and a vec2 (16)
	glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, RenderSystem::FOG_UNIFORM_BLOCK_INDEX, mFogUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	updateUbo();
}

FogSettings::~FogSettings()
{
	glDeleteBuffers(1, &mFogUbo);
}
