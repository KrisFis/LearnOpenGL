
#include "ModuleMinimal.h"
#include "RenderUtils.h"
#include "ColorUtils.h"

#include "Shader.h"
#include "Texture.h"
#include "Timer.h"
#include "Camera.h"
#include "Color.h"

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

// Test
constexpr float GCubeVertices[] = {
	// positions          // normals           // texture coords
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

constexpr glm::vec3 GCubePositions[] = {
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

constexpr glm::vec3 GPointLightPositions[] =
{
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};

constexpr FColor GPointLightColors[] =
{
	{ 139, 100, 255 },
	{ 255, 164, 116 },
	{ 88, 255, 95 },
	{ 255, 255, 25 }
};

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

void BindEvents(GLFWwindow* Window)
{
	glfwSetFramebufferSizeCallback(Window, &WindowSizeChanged);
	glfwSetCursorPosCallback(Window, &MousePositionChanged);
	glfwSetScrollCallback(Window, &MouseScrollChanged);
}

FBufferId CreateAndBindCube()
{
	FBufferId resultId = NRenderUtils::GenerateBuffer();
	NRenderUtils::BindBuffer(GL_ARRAY_BUFFER, resultId);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GCubeVertices), GCubeVertices, GL_STATIC_DRAW);

	NRenderUtils::UnbindBuffer(GL_ARRAY_BUFFER);

	return resultId;
}

void SetupAttributesForCube(const FBufferId& VertexArrayId, const FBufferId& BufferId)
{
	NRenderUtils::BindVertexArray(VertexArrayId);
	NRenderUtils::BindBuffer(GL_ARRAY_BUFFER, BufferId);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

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

void InitRender(FShader* Shaders, FTexture* Textures, FVertexArrayId* VAOs, FBufferId* VBOs)
{
	// Camera
	{
		GCamera.SetPosition({0.f, 0.f,3.f});
		GCamera.SetRotation({-90.f, 0.f, 0.f});

		GCamera.SetLookSensitivity(0.1f);
		GCamera.SetMoveSensitivity(0.5f);
	}

	// Material -> Lights
	{
		Shaders[0].Use();

		Shaders[0].SetInt32("material.diffuse", 0);
		Shaders[0].SetInt32("material.specular", 1);
		Shaders[0].SetInt32("material.emission", 2);

		Shaders[0].SetVec3("dirLight.direction", {-0.2f, -1.0f, -0.3f });
		Shaders[0].SetVec3("dirLight.ambient", {0.05f, 0.05f, 0.05f });
		Shaders[0].SetVec3("dirLight.diffuse", {0.4f, 0.4f, 0.4f });
		Shaders[0].SetVec3("dirLight.specular", {0.5f, 0.5f, 0.5f });

		for(uint8 i = 0; i < 4; ++i)
		{
			std::string nameOfLight = "pointLights[";
			nameOfLight.append(std::to_string(i));

			Shaders[0].SetVec3(std::string(nameOfLight + "].position").c_str(), GPointLightPositions[1]);
			Shaders[0].SetVec3(std::string(nameOfLight + "].ambient").c_str(), { 0.05f, 0.05f, 0.05f });
			Shaders[0].SetVec3(std::string(nameOfLight + "].diffuse").c_str(), { 0.8f, 0.8f, 0.8f});
			Shaders[0].SetVec3(std::string(nameOfLight + "].specular").c_str(), { 1.0f, 1.0f, 1.0f});
			Shaders[0].SetFloat(std::string(nameOfLight + "].constant").c_str(), 1.0f);
			Shaders[0].SetFloat(std::string(nameOfLight + "].linear").c_str(), 0.09f);
			Shaders[0].SetFloat(std::string(nameOfLight + "].quadratic").c_str(), 0.032f);
		}

		Shaders[0].SetVec3("spotLight.ambient", {0.0f, 0.0f, 0.0f});
		Shaders[0].SetVec3("spotLight.diffuse", {1.0f, 1.0f, 1.0f});
		Shaders[0].SetVec3("spotLight.specular", {1.0f, 1.0f, 1.0f});
		Shaders[0].SetFloat("spotLight.constant", 1.0f);
		Shaders[0].SetFloat("spotLight.linear", 0.09f);
		Shaders[0].SetFloat("spotLight.quadratic", 0.032f);
		Shaders[0].SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		Shaders[0].SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	}

	// Buffer
	{
		NRenderUtils::BindBuffer(GL_VERTEX_ARRAY, VBOs[0]);
	}
}

void ProcessRender(FShader* Shaders, FTexture* Textures, FVertexArrayId* VAOs)
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Mat4
	const glm::mat4 projection = glm::perspective(glm::radians(GCamera.GetFieldOfView()), (float)GWindowWidth / (float)GWindowHeight, 0.1f, 100.f);
	const glm::mat4 view = GCamera.GetViewMatrix();

	// Lightning
	{
		Shaders[0].Use();

		Shaders[0].SetVec3("spotLight.position", GCamera.GetPosition());
		Shaders[0].SetVec3("spotLight.direction", GCamera.GetFront());

		Shaders[0].SetMat4("view", view);
		Shaders[0].SetMat4("projection", projection);
		Shaders[0].SetMat4("model", glm::mat4(1.f));

		Shaders[0].SetVec3("viewPos", GCamera.GetPosition());

		Shaders[0].SetFloat("material.shininess", 64.f);

		for(uint8 i = 0; i < 3; ++i)
			Textures[i].Use(i);

		NRenderUtils::BindVertexArray(VAOs[0]);
		for (uint8 i = 0; i < 10; ++i)
		{
			glm::mat4 model =
				glm::rotate(
					glm::translate(glm::mat4(1.f), GCubePositions[i]),
					glm::radians(20.f * i),
					glm::vec3(1.f, 0.3f, 0.5f)
			);

			glm::mat3 normal = glm::transpose(glm::inverse(model));

			Shaders[0].SetMat4("model", model);
			Shaders[0].SetMat3("normalMatrix", normal);

		  glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// Light Obj
	{
		Shaders[1].Use();

		Shaders[1].SetMat4("projection", projection);
		Shaders[1].SetMat4("view", view);

		NRenderUtils::BindVertexArray(VAOs[1]);
		for (uint8 i = 0; i < 4; ++i)
		{
			Shaders[1].SetVec3("color", GPointLightColors[i].ToVec3());

			Shaders[1].SetMat4("model",
				glm::scale(
					glm::translate(glm::mat4(1.f), GPointLightPositions[i]),
					glm::vec3(0.2f)
			));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
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

	FTexture textures[] = {
			{"container2.png"},
			{"container2_specular.png"},
			{"matrix.jpg"}};
	if(!textures[0].IsInitialized() || !textures[1].IsInitialized() || !textures[2].IsInitialized())
	{
		return -3;
	}

	InitRender(&shaders[0], &textures[0], &VAOs[0], &VBO);

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
		ProcessRender(&shaders[0], &textures[0], &VAOs[0]);

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