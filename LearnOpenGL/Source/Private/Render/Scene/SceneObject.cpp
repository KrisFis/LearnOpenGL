
#include "SceneObject.h"

glm::mat4 FTransform::CalculateModelMatrix() const
{
	// IN ORDER -> T R S
	glm::mat4 resultMatrix = glm::scale(glm::mat4(1.f), Scale);
	resultMatrix = glm::translate(resultMatrix, Position / Scale);
	if(Rotation.x != 0) resultMatrix = glm::rotate(resultMatrix, glm::radians(Rotation.x), {1.f, 0.f, 0.f});
	if(Rotation.y != 0) resultMatrix = glm::rotate(resultMatrix, glm::radians(Rotation.y), {0.f, 1.f, 0.f});
	if(Rotation.z != 0) resultMatrix = glm::rotate(resultMatrix, glm::radians(Rotation.z), {0.f, 0.f, 1.f});
	return resultMatrix;
}
