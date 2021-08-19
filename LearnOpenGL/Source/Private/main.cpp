
#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Texture.h"
#include "Timer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// typedefs
typedef unsigned int FId;

static float DeltaSeconds = 0.f;

// Test
static float Opacity = 0.2f;
static float Rotation = 0.f;
static float PositionY = 0.f, PositionX = 0.f;

void WindowSizeChanged(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

bool CreateAndLinkProgram(const FId& VertexShaderId, const FId& FragmentShaderId, FId* OutProgram)
{
	FId resultProgramId;
	resultProgramId = glCreateProgram();
	glAttachShader(resultProgramId, VertexShaderId);
	glAttachShader(resultProgramId, FragmentShaderId);
	glLinkProgram(resultProgramId);

	int success;
	glGetProgramiv(resultProgramId, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(resultProgramId, 512, nullptr, infoLog);
		std::cout << "Program link failed\n" << infoLog << std::endl;
		return false;
	}

	if (OutProgram) *OutProgram = resultProgramId;
	return true;
}

bool CompileShader(const GLenum Type, const char* ShaderName, const char* Source, FId* OutId)
{
	FId resultShaderId = glCreateShader(Type);
	glShaderSource(resultShaderId, 1, &Source, nullptr);
	glCompileShader(resultShaderId);

	int success;
	glGetShaderiv(resultShaderId, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(resultShaderId, 512, nullptr, infoLog);
		std::cout << "[" << ShaderName << "] Shader compilation failed\n" << infoLog << std::endl;
		return false;
	}

	std::cout << "[" << ShaderName << "] Shader compiled successfully!" << std::endl;

	if(OutId) *OutId = resultShaderId;
	return true;
}

void ProcessInput(GLFWwindow* Window)
{
	const bool shiftPressed = (glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && shiftPressed)
		glfwSetWindowShouldClose(Window, true);

	if (glfwGetKey(Window, GLFW_KEY_F1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(Window, GLFW_KEY_F2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Opacity
	{

	}

	// Rotation && Translation && Opacity
	{
		if(shiftPressed)
		{
			const float rotationChange = 0.25f * DeltaSeconds;
			if(glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				Rotation += rotationChange;
			}
			else if(glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				Rotation -= rotationChange;
			}

			const float opacityChance = ((shiftPressed) ? 0.1f : 0.05f) * DeltaSeconds;
			if(glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				Opacity = (Opacity < opacityChance) ? 0.f : Opacity - opacityChance;
			}
			else if(glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				Opacity = (Opacity > 1.f - opacityChance) ? 1.f : Opacity + opacityChance;
			}
		}
		else
		{
			const float positionChange = 0.1f * DeltaSeconds;
			if(glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				PositionX += positionChange;
			}
			if(glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				PositionY -= positionChange;
			}
			if(glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				PositionX -= positionChange;
			}
			if(glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				PositionY += positionChange;
			}

			PositionY = glm::clamp(PositionY, -1.f, 1.f);
			PositionX = glm::clamp(PositionX, -1.f, 1.f);
		}
	}
}

void BindEvents(GLFWwindow* Window)
{
	glfwSetFramebufferSizeCallback(Window, &WindowSizeChanged);
}

void ProcessDraw(const FId& VertexArrayId, FShader ShaderToUse, FTexture TexturesToUse[2])
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Defines which program to use
	ShaderToUse.Use();

	// Textures
	{
		ShaderToUse.SetInt("texture1", 0);
		ShaderToUse.SetInt("texture2", 1);
		ShaderToUse.SetFloat("lerpValue", Opacity);

		TexturesToUse[0].Use(0);
		TexturesToUse[1].Use(1);
	}

	// Trans Mat
	{
		glm::mat4 trans = glm::mat4(1.f);
		trans =	glm::rotate(trans, Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		trans = glm::translate(trans, glm::vec3(PositionY, PositionX, 0.f));

		ShaderToUse.SetMatrix4("transform", trans);
	}

	// Binds vertex array
	glBindVertexArray(VertexArrayId);

	// TRIANGLES
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool CreateAndBindSquare(const FId& VertexArrayId, FId* OutBuffersIds[2])
{
	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
	};

	unsigned int indices[] = {
		 0, 1, 3, // first triangle
		 1, 2, 3  // second triangle
	};

	FId resultBuffersIds[2];
	glGenBuffers(2, resultBuffersIds);

	glBindVertexArray(VertexArrayId);

	glBindBuffer(GL_ARRAY_BUFFER, resultBuffersIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resultBuffersIds[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind vertex array
	glBindVertexArray(0);

	if (OutBuffersIds) *OutBuffersIds = resultBuffersIds;
	return true;
}

bool CreateVertexArray(FId* OutArrayId)
{
	FId resultArrayId;
	glGenVertexArrays(1, &resultArrayId);

	if (OutArrayId) *OutArrayId = resultArrayId;
	return true;
}

bool CreateInitWindow(GLFWwindow** OutWindow)
{
	if (!glfwInit()) 
	{
		std::cout << "failed to initialize GLFW" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* resultWindow = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (!resultWindow)
	{
		std::cout << "Failed to create window" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(resultWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

    BindEvents(resultWindow);
	glViewport(0, 0, 800, 600);

	if (OutWindow) *OutWindow = resultWindow;
	return true;
}

int main()
{
	GLFWwindow* mainWindow;
	if (!CreateInitWindow(&mainWindow))
	{
		glfwTerminate();
		return -1;
	}

	FId vertexArrayId;
	if (!CreateVertexArray(&vertexArrayId))
	{
		glfwTerminate();
		return -2;
	}

 	if (!CreateAndBindSquare(vertexArrayId, nullptr /* WE DO NOT CHANGE BUFFERS NOW */))
 	{
 		glfwTerminate();
 		return -3;
 	}

 	FTexture testTextures[] = {FTexture("container.jpg", ETextureType::JPEG), FTexture("awesomeface.png", ETextureType::PNG)};
 	if(!testTextures[0].IsInitialized() || !testTextures[1].IsInitialized())
	{
		glfwTerminate();
		return -4;
	}

 	FShader testShader("TestShader.vert", "TestShader.frag");
 	if(!testShader.IsInitialized())
	{
		glfwTerminate();
		return -5;
	}

	// Main render loop
	FTimer frameTimer;
	while (!glfwWindowShouldClose(mainWindow))
	{
		frameTimer.Start();

		ProcessInput(mainWindow);
		ProcessDraw(vertexArrayId, testShader, testTextures);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();

		frameTimer.Stop();
		DeltaSeconds = (float)frameTimer.GetSeconds();
		frameTimer.Reset();
	}

	glfwTerminate();
	return 0;
}