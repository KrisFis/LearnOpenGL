
#include "Application.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

FApplication::FApplication()
	: WindowWidth(0)
	, WindowHeight(0)
{}

FApplication::~FApplication()
{}

FApplication& FApplication::Get()
{
	static FApplication staticStatus = FApplication();
	return staticStatus;
}

void FApplication::SetWindowSize(uint16 Width, uint16 Height)
{
	WindowWidth = Width;
	WindowHeight = Height;
	
	glViewport(0, 0, WindowWidth, WindowHeight);
}
