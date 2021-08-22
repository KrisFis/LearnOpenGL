
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

FCamera::FCamera()
	: Position(glm::vec3(0.f, 0.f, 0.f))
	, Rotation(glm::vec3(0.f, 0.f, 0.f))
	, FieldOfView(45.f)
	, LookSensitivity(0.1f)
	, MoveSensitivity(0.5f)
{
	UpdateCache();
}

FCamera::FCamera(const glm::vec3 &InPosition, const glm::vec3 &InRotation)
	: Position(InPosition)
	, Rotation(InRotation)
	, FieldOfView(45.f)
	, LookSensitivity(0.1f)
	, MoveSensitivity(0.5f)
{
	UpdateCache();
}

FCamera::~FCamera()
{}

glm::mat4 FCamera::GetViewMatrix() const
{
	return glm::lookAt(Position, Position + CameraCache[0], CameraCache[2]);
}

void FCamera::SetPosition(const glm::vec3& Value)
{
	Position = Value;
}

void FCamera::SetRotation(const glm::vec3& Value)
{
	Rotation = Value;
	UpdateCache();
}

void FCamera::ProcessMoveInput(const ECameraMoveDirection Direction, float DeltaSeconds)
{
	const float velocity = MoveSensitivity * DeltaSeconds;
	switch (Direction)
	{
		case ECameraMoveDirection::Forward:
			Position += CameraCache[0] * velocity;
			break;
		case ECameraMoveDirection::Backward:
			Position -= CameraCache[0] * velocity;
			break;
		case ECameraMoveDirection::Left:
			Position -= CameraCache[1] * velocity;
			break;
		case ECameraMoveDirection::Right:
			Position += CameraCache[1] * velocity;
			break;
	}
}

void FCamera::ProcessMouseMove(float OffsetX, float OffsetY, bool ConstrainPitch)
{
	Rotation.x += OffsetX * LookSensitivity;
	Rotation.y += OffsetY * LookSensitivity;

	if(ConstrainPitch)
	{
		Rotation.y = glm::clamp(Rotation.y, -MaxPitch, MaxPitch);
	}

	UpdateCache();
}

void FCamera::ProcessScroll(float Value)
{
	FieldOfView -= Value;
	FieldOfView = glm::clamp(FieldOfView, 1.f, 45.f);
}

void FCamera::UpdateCache()
{
	// Front
 	CameraCache[0] = glm::normalize(glm::vec3(
 		glm::cos(glm::radians(Rotation.x)) * glm::cos(glm::radians(Rotation.y)),
 	 	glm::sin(glm::radians(Rotation.y)),
 	 	glm::sin(glm::radians(Rotation.x)) * glm::cos(glm::radians(Rotation.y))
 	));

 	// Right
 	CameraCache[1] = glm::normalize(glm::cross(CameraCache[0], glm::vec3(0.f, 1.f, 0.f)));

	// Up
	CameraCache[2] = glm::normalize(glm::cross(CameraCache[1], CameraCache[0]));
}
