
#include "ModuleMinimal.h"
#include "RenderUtils.h"
#include "FileUtils.h"
#include "MeshUtils.h"
#include "ColorUtils.h"
#include "Scene.h"

#include "Shader.h"
#include "Timer.h"
#include "Camera.h"
#include "Model.h"

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
bool GbShiftWasPressed = false;
bool GbAltWasPressed = false;

// Mouse
float GLastMouseX = GWindowWidth * 0.5f;
float GLastMouseY = GWindowHeight * 0.5f;

// Camera
FCamera GCamera;
FScene GScene;

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

void BindEvents(GLFWwindow* Window)
{
	glfwSetFramebufferSizeCallback(Window, &WindowSizeChanged);
	glfwSetCursorPosCallback(Window, &MousePositionChanged);
	glfwSetScrollCallback(Window, &MouseScrollChanged);
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

	// TESTS
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
	}
	
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	return true;
}

bool ConstructScene(FScene& OutScene)
{
	FTexture rocksFloorTexture = { NFileUtils::ContentPath("Textures/Default/floor_rocks.jpg").c_str(), ETextureType::Diffuse };
	FTexture wallTexture = { NFileUtils::ContentPath("Textures/Default/wall128x128.png").c_str(), ETextureType::Diffuse };
	FTexture brickTexture = { NFileUtils::ContentPath("Textures/Default/bricksx64.png").c_str(), ETextureType::Diffuse };
	FTexture carpetTexture = { NFileUtils::ContentPath("Textures/Default/carpet.png").c_str(), ETextureType::Diffuse };
	if(!wallTexture.IsInitialized() || !brickTexture.IsInitialized() || !carpetTexture.IsInitialized())
	{
		return false;
	}
	
	std::vector<FMeshPtr> meshes;
	meshes.push_back(NMeshUtils::ConstructPlane(rocksFloorTexture));
	meshes[meshes.size()-1]->SetTransform({
		{0.f, -1.f, 0.f},
		{0.f, 0.f, 0.f},
		{10.f, 1.f, 10.f}
	});

	meshes.push_back(NMeshUtils::ConstructSphere(wallTexture));
	meshes[meshes.size()-1]->SetTransform({
			{10.f, 10.f, 10.f},
			{0.f, 0.f, 0.f},
			{2.f, 2.f, 2.f}
	});
	
	meshes.push_back(NMeshUtils::ConstructCube(brickTexture));
	meshes[meshes.size()-1]->SetOutlineSize(0.025f);
	meshes[meshes.size()-1]->SetOutlineColor(NColors::Blue);
	meshes[meshes.size()-1]->SetTransform({
			{10.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{0.25f, 1.f, 5.f}
	});

	meshes.push_back(NMeshUtils::ConstructCube(brickTexture));
	meshes[meshes.size()-1]->SetOutlineSize(0.025f);
	meshes[meshes.size()-1]->SetOutlineColor(NColors::Blue);
	meshes[meshes.size()-1]->SetTransform({
			{8.f, 0.f, 2.f},
			{0.f, 0.f, 0.f},
			{0.25f, 1.f, 2.f}
	});

	meshes.push_back(NMeshUtils::ConstructCube(brickTexture));
	meshes[meshes.size()-1]->SetOutlineSize(0.025f);
	meshes[meshes.size()-1]->SetOutlineColor(NColors::Blue);
	meshes[meshes.size()-1]->SetTransform({
			{6.f, 0.f, -2.f},
			{0.f, 0.f, 0.f},
			{0.25f, 1.f, 2.f}
	});

	meshes.push_back(NMeshUtils::ConstructCube(brickTexture));
	meshes[meshes.size()-1]->SetOutlineSize(0.025f);
	meshes[meshes.size()-1]->SetOutlineColor(NColors::Blue);
	meshes[meshes.size()-1]->SetTransform({
			{4.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{1.25f, 1.f, 0.25f}
	});
	
	OutScene.AddMeshes(meshes);
	return true;
}

void ProcessInput()
{
	const bool shiftWasPreviouslyPressed = GbShiftWasPressed;
	GbShiftWasPressed = (glfwGetKey(GWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	const bool shiftWasJustPressed = !shiftWasPreviouslyPressed && GbShiftWasPressed;
	const bool shiftWasJustReleased = shiftWasPreviouslyPressed && !GbShiftWasPressed;

	const bool altWasPreviouslyPressed = GbAltWasPressed;
	GbAltWasPressed = (glfwGetKey(GWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);

	const bool altWasJustPressed = !altWasPreviouslyPressed && GbAltWasPressed;
	const bool altWasJustReleased = altWasPreviouslyPressed && !GbAltWasPressed;

	if (glfwGetKey(GWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS && GbShiftWasPressed)
		glfwSetWindowShouldClose(GWindow, true);

	if (glfwGetKey(GWindow, GLFW_KEY_F1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(GWindow, GLFW_KEY_F2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Cursor
	{
		if(altWasJustPressed)
		{
			const bool isProcessingInput = GCamera.GetShouldProcessInput();

			GCamera.SetShouldProcessInput(!isProcessingInput);
			glfwSetInputMode(GWindow, GLFW_CURSOR, (isProcessingInput) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		}
	}

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

void ProcessRender(FShaderProgram& Shader)
{
	// Setup render tick
	{
		// Default clear color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		// Disable stencil write
		glStencilMask(0x0);
	}
	
	// Mat4
	const glm::mat4 projection = glm::perspective(glm::radians(GCamera.GetFieldOfView()), (float)GWindowWidth / (float)GWindowHeight, 0.1f, 100.f);
	const glm::mat4 view = GCamera.GetViewMatrix();

	Shader.Use();

	Shader.SetMat4("view", view);
	Shader.SetMat4("projection", projection);
	Shader.SetMat4("model", glm::mat4(1.f));
	
	GScene.Draw(Shader);
}

void EngineTick()
{
	// Update FPS
	{
		std::string resultTitle;
		resultTitle.append("LearnOpenGL: FPS [");
		resultTitle.append(std::to_string(GetFramesPerSecond()));
		resultTitle.append("]");
	
		glfwSetWindowTitle(GWindow, resultTitle.c_str());
	}
	
	// Position log
	{
		const glm::vec3& currentPos = GCamera.GetPosition(); 
		std::cout << "Current position: [" << currentPos.x << ',' << currentPos.y << ',' << currentPos.z << ']' << std::endl;
	}
}

int32 GuardedMain()
{
	if (!CreateInitWindow(GWindow))
	{
		return -1;
	}

	FShaderProgram shaderToUse = { NFileUtils::ContentPath("Shaders/Vertex/Mesh.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Mesh.frag").c_str() };
	if(!shaderToUse.IsInitialized())
	{
		return -2;
	}
	
	if(!ConstructScene(GScene))
	{
		return -3;
	}

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
		ProcessRender(shaderToUse);

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