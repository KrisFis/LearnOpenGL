
#include "ModuleMinimal.h"
#include "RenderUtils.h"

#include "Shader.h"
#include "Texture.h"
#include "Timer.h"
#include "Camera.h"

#include <iostream>
#include <vector>

// Window
GLFWwindow* GWindow = nullptr;

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

// Test
constexpr glm::vec3 GLightPos = {1.2f, 1.f, 2.f};

uint16 GetFramesPerSecond()
{
	return (uint16)std::floor(60.f / GDeltaSeconds);
}

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

bool CreateAndLinkProgram(const FBufferId& VertexShaderId, const FBufferId& FragmentShaderId, FBufferId* OutProgram)
{
	FBufferId resultProgramId;
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

bool CompileShader(const GLenum Type, const char* ShaderName, const char* Source, FBufferId* OutId)
{
	FBufferId resultShaderId = glCreateShader(Type);
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

void ProcessInput()
{
	const bool shiftWasPreviouslyPressed = GbShiftWasPressed;
	GbShiftWasPressed = (glfwGetKey(GWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	const bool shiftIsPressed = GbShiftWasPressed;
	const bool shiftWasJustPressed = !shiftWasPreviouslyPressed && GbShiftWasPressed;
	const bool shiftWasJustReleased = shiftWasPreviouslyPressed && !GbShiftWasPressed;

	if (glfwGetKey(GWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS && shiftWasJustPressed)
		glfwSetWindowShouldClose(GWindow, true);

	if (glfwGetKey(GWindow, GLFW_KEY_F1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(GWindow, GLFW_KEY_F2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Rotation && Translation && Opacity
	{
		if(shiftWasJustPressed)
			GCamera.SetMoveSensitivity(GCamera.GetMoveSensitivity()*2.f);
		if(shiftWasJustReleased)
			GCamera.SetMoveSensitivity(GCamera.GetMoveSensitivity()*0.5f);

		if (glfwGetKey(GWindow, GLFW_KEY_W) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Forward, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_S) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Backward, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_A) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Left, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_D) == GLFW_PRESS)
			GCamera.ProcessMoveInput(ECameraMoveDirection::Right, GDeltaSeconds);
	}
}

void BindEvents(GLFWwindow* Window)
{
	glfwSetFramebufferSizeCallback(Window, &WindowSizeChanged);
	glfwSetCursorPosCallback(Window, &MousePositionChanged);
	glfwSetScrollCallback(Window, &MouseScrollChanged);
}

FBufferId CreateAndBindCube()
{
	constexpr float vertices[] = {
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	FBufferId resultId = NRenderUtils::GenerateBuffer();
	NRenderUtils::BindBuffer(GL_ARRAY_BUFFER, resultId);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	NRenderUtils::UnbindBuffer(GL_ARRAY_BUFFER);

	return resultId;
}

void SetupAttributesForCube(const FBufferId& VertexArrayId, const FBufferId& BufferId)
{
	NRenderUtils::BindVertexArray(VertexArrayId);
	NRenderUtils::BindBuffer(GL_ARRAY_BUFFER, BufferId);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	NRenderUtils::UnbindBuffer(GL_ARRAY_BUFFER);
	NRenderUtils::UnbindVertexArray();
}

bool CreateInitWindow(GLFWwindow*& OutWindow)
{
	if (!glfwInit())
	{
		std::cout << "failed to initialize GLFW" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	OutWindow = glfwCreateWindow(GWindowWidth, GWindowHeight, "LearnOpenGL", nullptr, nullptr);
	if (!OutWindow)
	{
		std::cout << "Failed to create window" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(OutWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

    BindEvents(OutWindow);

	glfwSetInputMode(OutWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glViewport(0, 0, GWindowWidth, GWindowHeight);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void InitRender(FBufferId VBO)
{
	// Camera
	{
		GCamera.SetPosition({0.f, 0.f,3.f});
		GCamera.SetRotation({-90.f, 0.f, 0.f});

		GCamera.SetLookSensitivity(0.1f);
		GCamera.SetMoveSensitivity(0.5f);
	}

	// Buffer
	{
		NRenderUtils::BindBuffer(GL_VERTEX_ARRAY, VBO);
	}
}

void ProcessRender(FShader LightningShader, FShader LightObjShader, FVertexArrayId LightningVAO, FVertexArrayId LightObjVAO)
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Mat4
	const glm::mat4 projection = glm::perspective(glm::radians(GCamera.GetFieldOfView()), (float)GWindowWidth / (float)GWindowHeight, 0.1f, 100.f);
	const glm::mat4 view = GCamera.GetViewMatrix();

	// Lightning
	{
		LightningShader.Use();

		LightningShader.SetMat4("view", GCamera.GetViewMatrix());
		LightningShader.SetMat4("projection", projection);
		LightningShader.SetMat4("model", glm::mat4(1.f));

		LightningShader.SetVec3("lightPos", GLightPos);
		LightningShader.SetVec3("viewPos", GCamera.GetPosition());

		LightningShader.SetVec3("objectColor", {1.f, 0.5f, 0.31f});
		LightningShader.SetVec3("lightColor", {1.f, 1.f, 1.f});

		NRenderUtils::BindVertexArray(LightningVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Light Obj
	{
		LightObjShader.Use();

		LightObjShader.SetMat4("view", GCamera.GetViewMatrix());
		LightObjShader.SetMat4("projection", projection);
		LightObjShader.SetMat4("model",
			glm::scale(
				glm::translate(glm::mat4(1.f), GLightPos),
				glm::vec3(0.2f)
		));

		NRenderUtils::BindVertexArray(LightObjVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void EngineTick()
{
	std::string resultTitle;
	resultTitle.append("LearnOpenGL: FPS [");
	resultTitle.append(std::to_string(GetFramesPerSecond()));
	resultTitle.append("]");

	glfwSetWindowTitle(GWindow, resultTitle.c_str());
}

int32 GuardedMain()
{
	if (!CreateInitWindow(GWindow))
	{
		return -1;
	}

	FBufferId VBO = CreateAndBindCube();
	auto VAOs = NRenderUtils::GenerateVertexArrays(2);
	for(FVertexArrayId VAO : VAOs)
		SetupAttributesForCube(VAO, VBO);

	FShader shaders[] = {
		{"Lighting.vert", "Lighting.frag"},
		{"LightObj.vert", "LightObj.frag"}};
	if(!shaders[0].IsInitialized() || !shaders[1].IsInitialized())
	{
		return -2;
	}

	InitRender(VBO);

	// Main render loop
	FTimer frameTimer;
	while (!glfwWindowShouldClose(GWindow))
	{
		GDeltaSeconds = GLastSeconds;
		GLastSeconds = (float)frameTimer.GetSeconds();

		frameTimer.Reset();
		frameTimer.Start();

		EngineTick();
		ProcessInput();
		ProcessRender(shaders[0], shaders[1], VAOs[0], VAOs[1]);

		glfwSwapBuffers(GWindow);
		glfwPollEvents();

		frameTimer.Stop();
	}

	return 0;
}

int32 main()
{
	// Init
	{
		NRenderUtils::Init();
		NRenderUtils::AllowUnbind(false);
	}

	const int32 result = GuardedMain();

	// Shutdown
	{
		NRenderUtils::Shutdown();
		glfwTerminate();
	}

	// End print
	{
		if(result == 0)
			std::cout << "Application ended successfully" << std::endl;
		else
			std::cout << "Application ended with error code " << result << std::endl;
	}

	return result;
}