
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class FShaderProgram;

enum class ELightType : uint8
{
	Invalid = 0,
	
	Directional,
	Point,
	Spot
};

class ILight : public TSharedClass<ILight>
{

public: // Virtual destructor

	virtual ~ILight();

public: // Getters

	virtual bool IsInitialized() const = 0;

public: // Getter -> Color

	FORCEINLINE const glm::vec3& GetAmbient() const { return Color.Ambient; }
	FORCEINLINE const glm::vec3& GetDiffuse() const { return Color.Diffuse; }
	FORCEINLINE const glm::vec3& GetSpecular() const { return Color.Specular; }

public: // Setters -> Color

	FORCEINLINE void SetAmbient(const glm::vec3& Value) { Color.Ambient = Value; }
	FORCEINLINE void SetDiffuse(const glm::vec3& Value) { Color.Diffuse = Value; }
	FORCEINLINE void SetSpecular(const glm::vec3& Value) { Color.Specular = Value; }

public: // Draw methods

	virtual void SetupDraw(const TSharedPtr<FShaderProgram>& Shader);

private: // Fields

	struct FLightColorProperties
	{
		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;
	} Color;
};

typedef TSharedPtr<ILight> FLightPtr;