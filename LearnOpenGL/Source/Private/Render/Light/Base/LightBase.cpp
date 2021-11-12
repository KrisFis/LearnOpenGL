
#include "LightBase.h"
#include "ShaderProgram.h"

ILight::~ILight() {}

void ILight::SetupDraw(const TSharedPtr<FShaderProgram>& Shader)
{
	ENSURE_RET(Shader->IsEnabled());
	
	
}

