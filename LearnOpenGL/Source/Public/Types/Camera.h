
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

private: // Constructors

	explicit FCamera(); // Default camera in 0.0.0 pos
	explicit FCamera(const glm::vec3& InPosition, const glm::vec3& InRotation);
	
public: // Destructors
	
	virtual ~FCamera();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FCamera> Create()
	{ return MakeShareable(new FCamera()); }
	
	FORCEINLINE static TSharedPtr<FCamera> Create(const glm::vec3& Position, const glm::vec3& Rotation)
	{ return MakeShareable(new FCamera(Position, Rotation)); }

public: // Getters for render

	FORCEINLINE float GetFieldOfView() const { return FieldOfView; }
	glm::mat4 GetViewMatrix() const;

public: // Sensitivity

	FORCEINLINE float GetLookSensitivity() const { return LookSensitivity; }
	FORCEINLINE void SetLookSensitivity(float Value) { LookSensitivity = Value; }

	FORCEINLINE float GetMoveSensitivity() const { return MoveSensitivity; }
	FORCEINLINE void SetMoveSensitivity(float Value) { MoveSensitivity = Value; }

public: // Position and rotation

	FORCEINLINE const glm::vec3& GetPosition() const { return Position; }
	FORCEINLINE const glm::vec3& GetRotation() const { return Rotation; }

	void SetPosition(const glm::vec3& Value);
	void SetRotation(const glm::vec3& Value);

public: // Cache getters

	FORCEINLINE glm::vec3 GetFront() const { return CameraCache[0]; }
	FORCEINLINE glm::vec3 GetRight() const { return CameraCache[1]; }
	FORCEINLINE glm::vec3 GetLeft() const { return -1.f * CameraCache[1]; }
	FORCEINLINE glm::vec3 GetUp() const { return CameraCache[2]; }

public: // Input

	FORCEINLINE bool GetShouldProcessInput() const { return bShouldProcessInput; }
	FORCEINLINE void SetShouldProcessInput(bool Value) { bShouldProcessInput = Value; }

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

private: // Configuration

	uint8 bShouldProcessInput : 1;

};

typedef TSharedPtr<FCamera> FCameraPtr;