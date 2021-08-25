
#pragma once

#include "ModuleMinimal.h"

enum class ECameraMoveDirection
{
	Forward,
	Backward,
	Left,
	Right
};

class FCamera
{

public: // Static

	static constexpr float MaxPitch = 89.f;

public: // Constructors

	FCamera(); // Default camera in 0.0.0 pos
	FCamera(const glm::vec3& InPosition, const glm::vec3& InRotation);
	virtual ~FCamera();

public: // Getters for render

	inline float GetFieldOfView() const { return FieldOfView; }
	glm::mat4 GetViewMatrix() const;

public: // Sensitivity

	inline float GetLookSensitivity() const { return LookSensitivity; }
	inline void SetLookSensitivity(float Value) { LookSensitivity = Value; }

	inline float GetMoveSensitivity() const { return MoveSensitivity; }
	inline void SetMoveSensitivity(float Value) { MoveSensitivity = Value; }

public: // Position and rotation

	inline const glm::vec3& GetPosition() const { return Position; }
	inline const glm::vec3& GetRotation() const { return Rotation; }

	void SetPosition(const glm::vec3& Value);
	void SetRotation(const glm::vec3& Value);

public: // Cache getters

	inline glm::vec3 GetFront() const { return CameraCache[0]; }
	inline glm::vec3 GetRight() const { return CameraCache[1]; }
	inline glm::vec3 GetLeft() const { return -1.f * CameraCache[1]; }
	inline glm::vec3 GetUp() const { return CameraCache[2]; }

public: // Input

	void ProcessMoveInput(const ECameraMoveDirection Direction, float DeltaSeconds);
	void ProcessMouseMove(float OffsetX, float OffsetY, bool ConstrainPitch = true);
	void ProcessScroll(float Value);

private: // Calculation helpers

	void UpdateCache();

private: // Fields

	glm::vec3 Position;
	glm::vec3 Rotation;

	// Front, Right, Up
	glm::vec3 CameraCache[3];

	float FieldOfView;
	float LookSensitivity;
	float MoveSensitivity;
};