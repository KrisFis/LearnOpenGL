
#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Texture.h"
#include "Timer.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// typedefs
typedef unsigned int FId;

// Window
unsigned short GWindowWidth = 800;
unsigned short GWindowHeight = 600;

// Global
float GLastSeconds = 0.f;
float GDeltaSeconds = 0.f;

// Key
bool GbShiftWasPressed = true;

// Mouse
float GLastMouseX = GWindowWidth * 0.5f;
float GLastMouseY = GWindowHeight * 0.5f;

// Camera
FCamera GCamera;

void MouseScrollChanged(GLFWwindow* window, double ScrollX, double ScrollY)
{
	GCamera.ProcessScroll((float)ScrollY);
}

void MousePositionChanged(GLFWwindow* window, double MouseX, double MouseY)
{
	static bool firstHit = true;
	if(firstHit)
	{
		GLastMouseX = (float)MouseX;
		GLastMouseY = (float)MouseY;
		firstHit = false;
	}

	float offsetX = (float)MouseX - GLastMouseX;
	float offsetY = GLastMouseY - (float)MouseY; // reversed since y-coordinates range from bottom to top
	GLastMouseX = (float)MouseX;
	GLastMouseY = (float)MouseY;

	GCamera.ProcessMouseMove(offsetX, offsetY);
}

void WindowSizeChanged(GLFWwindow* Window, int Width, int Height)
{
	GWindowWidth = Width;
	GWindowHeight = Height;

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
	const bool shiftWasPreviouslyPressed = GbShiftWasPressed;
	GbShiftWasPressed = (glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	const bool shiftWasJustPressed = !shiftWasPreviouslyPressed && GbShiftWasPressed;
	const bool shiftWasJustReleased = shiftWasPreviouslyPressed && !GbShiftWasPressed;

	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && shiftWasJustPressed)
		glfwSetWindowShouldClose(Window, true);

	if (glfwGetKey(Window, GLFW_KEY_F1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(Window, GLFW_KEY_F2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Rotation && Translation && Opacity
	{
		if(shiftWasJustPressed)
			GCamera.SetMoveSensitivity(GCamera.GetMoveSensitivity()*2.f);
		if(shiftWasJustReleased)
			GCamera.SetMoveSensitivity(GCamera.GetMoveSensitivity()*0.5f);

		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Forward, GDeltaSeconds);
		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Backward, GDeltaSeconds);
		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Left, GDeltaSeconds);
		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Right, GDeltaSeconds);
	}
}

void BindEvents(GLFWwindow* Window)
{
	glfwSetFramebufferSizeCallback(Window, &WindowSizeChanged);
	glfwSetCursorPosCallback(Window, &MousePositionChanged);
	glfwSetScrollCallback(Window, &MouseScrollChanged);
}

bool CreateAndBindCube(const FId& VertexArrayId, FId* OutBufferId)
{
	float vertices[] = {
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	FId resultBufferId;
	glGenBuffers(1, &resultBufferId);

	glBindVertexArray(VertexArrayId);

	glBindBuffer(GL_ARRAY_BUFFER, resultBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	if (OutBufferId) *OutBufferId = resultBufferId;
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

	GLFWwindow* resultWindow = glfwCreateWindow(GWindowWidth, GWindowHeight, "LearnOpenGL", nullptr, nullptr);
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

	glfwSetInputMode(resultWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glViewport(0, 0, GWindowWidth, GWindowHeight);
	glEnable(GL_DEPTH_TEST);

	if (OutWindow) *OutWindow = resultWindow;
	return true;
}

void InitDraw(FShader& Shader, FTexture Textures[2])
{
	Shader.Use();

	// Textures
	{
		Textures[0].Use(0);
		Textures[1].Use(1);

		Shader.SetInt("texture1", 0);
		Shader.SetInt("texture2", 1);
	}

	// Camera
	{
		GCamera.SetPosition({0.f, 0.f,3.f});
		GCamera.SetRotation({-90.f, 0.f, 0.f});

		GCamera.SetLookSensitivity(0.1f);
		GCamera.SetMoveSensitivity(0.5f);
	}
}

void DrawCubes(FShader& Shader)
{
	static const glm::vec3 cubePositions[] = {
	    glm::vec3( 0.0f,  0.0f,  0.0f),
	    glm::vec3( 2.0f,  5.0f, -15.0f),
	    glm::vec3(-1.5f, -2.2f, -2.5f),
	    glm::vec3(-3.8f, -2.0f, -12.3f),
	    glm::vec3( 2.4f, -0.4f, -3.5f),
	    glm::vec3(-1.7f,  3.0f, -7.5f),
	    glm::vec3( 1.3f, -2.0f, -2.5f),
	    glm::vec3( 1.5f,  2.0f, -2.5f),
	    glm::vec3( 1.5f,  0.2f, -1.5f),
	    glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	for(unsigned char i = 0; i < 10; ++i)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
		Shader.SetMatrix4("model", model);

		// TRIANGLES
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void ProcessDraw(const FId& VertexArrayId, FShader& ShaderToUse)
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Camera
	{
		ShaderToUse.SetMatrix4("view", GCamera.GetViewMatrix());

		glm::mat4 projection = glm::perspective(glm::radians(GCamera.GetFieldOfView()), (float)GWindowWidth / (float)GWindowHeight, 0.1f, 100.f);
		ShaderToUse.SetMatrix4("projection", projection);
	}

	// Binds vertex array
	glBindVertexArray(VertexArrayId);
	DrawCubes(ShaderToUse);
}

void UpdateDeltaSeconds(const FTimer& Timer)
{
	GDeltaSeconds = GLastSeconds;
	GLastSeconds = (float)Timer.GetSeconds();
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

 	if (!CreateAndBindCube(vertexArrayId, nullptr /* WE DO NOT CHANGE BUFFERS NOW */))
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

	InitDraw(testShader, testTextures);

	// Main render loop
	FTimer frameTimer;
	while (!glfwWindowShouldClose(mainWindow))
	{
		UpdateDeltaSeconds(frameTimer);

		frameTimer.Reset();
		frameTimer.Start();

		ProcessInput(mainWindow);
		ProcessDraw(vertexArrayId, testShader);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();

		frameTimer.Stop();
	}

	glfwTerminate();
	return 0;
}