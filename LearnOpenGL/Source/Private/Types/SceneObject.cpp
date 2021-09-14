
#include "SceneObject.h"

glm::mat4 FTransform::CalculateModelMatrix() const
{
	return glm::translate(
		glm::rotate(
			glm::scale(glm::mat4(1.f), Scale),
			glm::radians(360.f),
			Rotation
		),
		Position
	);
}
