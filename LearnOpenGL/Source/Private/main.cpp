
#include "ModuleMinimal.h"
#include "RenderUtils.h"
#include "FileUtils.h"
#include "MeshUtils.h"
#include "ColorUtils.h"
#include "Scene.h"
#include "TextureUtils.h"

#include "ShaderProgram.h"
#include "Texture.h"
#include "Timer.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Mesh2D.h"
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"
#include "UniformBuffer.h"

#include "Cubemap.h"
#include "Skybox.h"

// Window
GLFWwindow* GWindow = nullptr;

uint16 GWindowWidth = 800;
uint16 GWindowHeight = 600;

// Global
float GLastSeconds = 0.f;
float GDeltaSeconds = 0.f;

// Key
bool GbShiftWasPressed = false;
bool GbAltWasPressed = false;

// Mouse
float GLastMouseX = GWindowWidth * 0.5f;
float GLastMouseY = GWindowHeight * 0.5f;

// Global instances
FCameraPtr GCamera;
FScenePtr GScene;

FSceneObjectPtr GSkyboxObject;

FFramebufferPtr GMSAAFramebuffer;
FFramebufferPtr GScreenFramebuffer;
FSceneObjectPtr GScreenObject;

FUniformBufferPtr GMatricesBuffer;

void MouseScrollChanged(GLFWwindow* window, double ScrollX, double ScrollY)
{
	GCamera->ProcessScroll((float)ScrollY);
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

	GCamera->ProcessMouseMove(offsetX, offsetY);
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

	// Log Info
	{
		std::cout << std::endl;
		std::cout << "Rendering information:" << std::endl;
		std::cout << "Graphic: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << std::endl;
	}

	// Features enable
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}

	// Features set
	{
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	return true;
}

bool PrepareSkybox(FSceneObjectPtr& OutSkyboxObj)
{
	OutSkyboxObj = FSkybox::Create(
		FCubemap::Create(
			NTextureUtils::GetFacesPathFromRoot(NFileUtils::ContentPath("Textures/Cubemaps/Skybox_high"))
		)
	)->AsShared();

	return true;
}

bool PrepareScreenScene(FSceneObjectPtr& OutScreenObj, FFramebufferPtr& OutMSAAFramebuffer, FFramebufferPtr& OutScreenFramebuffer)
{
	// MSAA
	{
		FRenderTexturePtr multisampledTextureTarget = FRenderTexture::Create(GWindowWidth, GWindowHeight, ERenderTargetType::Color, 4);
		FRenderBufferPtr multisampledBufferTarget = FRenderBuffer::Create(GWindowWidth, GWindowHeight, ERenderTargetType::DepthAndStencil, 4);
		if(!multisampledTextureTarget->IsInitialized() || !multisampledBufferTarget->IsInitialized())
		{
			return false;
		}
	
		OutMSAAFramebuffer = FFramebuffer::Create();
		OutMSAAFramebuffer->Attach(GL_FRAMEBUFFER, multisampledTextureTarget->AsShared());
		OutMSAAFramebuffer->Attach(GL_FRAMEBUFFER, multisampledBufferTarget->AsShared());
	}
	
	// SCREEN
	{
		FRenderTexturePtr sceneTextureTarget = FRenderTexture::Create(GWindowWidth, GWindowHeight, ERenderTargetType::Color);
		if(!sceneTextureTarget->IsInitialized())
		{
			return false;
		}
		
		FTexturePtr screenQuad = FTexture::Create(sceneTextureTarget.Get(), ETextureType::Diffuse);
		if(!screenQuad->IsInitialized())
		{
			return false;
		}
		
		OutScreenFramebuffer = FFramebuffer::Create();
		OutScreenFramebuffer->Attach(GL_FRAMEBUFFER, sceneTextureTarget->AsShared());
	
		static const TArray<FMesh2DVertex> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			{ glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{ glm::vec2( -1.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
			{ glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{ glm::vec2( -1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{ glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{ glm::vec2( 1.0f,  1.0f), glm::vec2(1.0f, 1.)}
		};
		
		OutScreenObj = FMesh2D::Create(quadVertices, {screenQuad})->AsShared();
		OutScreenObj->SetCullFaces(false);
	}
	
	return true;
}

bool PrepareScene(FScenePtr& OutScene)
{
	FTexturePtr rocksFloorTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/floor_rocks.jpg").c_str(), ETextureType::Diffuse);
	FTexturePtr wallTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/wall128x128.png").c_str(), ETextureType::Diffuse);
	FTexturePtr brickTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/bricksx64.png").c_str(), ETextureType::Diffuse);
	FTexturePtr carpetTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/carpet.png").c_str(), ETextureType::Diffuse);
	FTexturePtr grassTexture = FTexture::Create(NFileUtils::ContentPath("Textures/grass.png").c_str(), ETextureType::Diffuse, true);
	if(!wallTexture->IsInitialized() || !brickTexture->IsInitialized() || !carpetTexture->IsInitialized() || !grassTexture->IsInitialized())
	{
		return false;
	}
	
	TArray<FSceneObjectPtr> sceneObjects;
	sceneObjects.push_back(NMeshUtils::ConstructPlane(rocksFloorTexture));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
		{0.f, -1.f, 0.f},
		{0.f, 0.f, 0.f},
		{10.f, 1.f, 10.f}
	});

	sceneObjects.push_back(NMeshUtils::ConstructSphere(wallTexture));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{10.f, 10.f, 10.f},
			{0.f, 0.f, 0.f},
			{2.f, 2.f, 2.f}
	});
	
	sceneObjects.push_back(NMeshUtils::ConstructCube(brickTexture));
	sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::Navy);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{10.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{0.25f, 1.f, 5.f}
	});

	sceneObjects.push_back(NMeshUtils::ConstructCube(brickTexture));
	sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::Navy);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{8.f, 0.f, 2.f},
			{0.f, 0.f, 0.f},
			{0.25f, 1.f, 2.f}
	});

	sceneObjects.push_back(NMeshUtils::ConstructCube(brickTexture));
	sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::Navy);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{6.f, 0.f, -2.f},
			{0.f, 0.f, 0.f},
			{0.25f, 1.f, 2.f}
	});

	sceneObjects.push_back(NMeshUtils::ConstructCube(brickTexture));
	sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::Navy);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{4.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{1.25f, 1.f, 0.25f}
	});
	
	sceneObjects.push_back(NMeshUtils::ConstructPlane(grassTexture));
	sceneObjects[sceneObjects.size() - 1]->SetCullFaces(false);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{2.f, -0.499f, 0.f},
			{-90.f, 0.f, 90.f},
			{0.5f, 0.5f, 0.5f}
	});
	
	OutScene = FScene::Create(sceneObjects);
	return true;
}

bool PrepareShaders(TArray<FShaderProgramPtr>& OutShaders, FUniformBufferPtr& OutMatBuffer)
{
	OutShaders.push_back(FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Mesh.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Mesh.frag").c_str()));
	OutShaders.push_back(FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Screen.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Screen.frag").c_str()));
	OutShaders.push_back(FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Skybox.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Skybox.frag").c_str()));
	
	if(!(OutShaders[0]->IsInitialized() && OutShaders[1]->IsInitialized() && OutShaders[2]->IsInitialized())) return false;

	OutMatBuffer = FUniformBuffer::Create(0, 2 * sizeof(glm::mat4));
	OutShaders[0]->SetUniformBuffer("Matrices", OutMatBuffer.GetRef());
	OutShaders[2]->SetUniformBuffer("Matrices", OutMatBuffer.GetRef());
	
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
			const bool isProcessingInput = GCamera->GetShouldProcessInput();

			GCamera->SetShouldProcessInput(!isProcessingInput);
			glfwSetInputMode(GWindow, GLFW_CURSOR, (isProcessingInput) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		}
	}

	// Rotation && Translation && Opacity
	{
		if(shiftWasJustPressed)
			GCamera->SetMoveSensitivity(GCamera->GetMoveSensitivity()*2.f);
		if(shiftWasJustReleased)
			GCamera->SetMoveSensitivity(GCamera->GetMoveSensitivity()*0.5f);

		if (glfwGetKey(GWindow, GLFW_KEY_W) == GLFW_PRESS)
			GCamera->ProcessMoveInput(ECameraMoveDirection::Forward, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_S) == GLFW_PRESS)
			GCamera->ProcessMoveInput(ECameraMoveDirection::Backward, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_A) == GLFW_PRESS)
			GCamera->ProcessMoveInput(ECameraMoveDirection::Left, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_D) == GLFW_PRESS)
			GCamera->ProcessMoveInput(ECameraMoveDirection::Right, GDeltaSeconds);
	}
}

void ProcessRender(const TArray<FShaderProgramPtr>& Shaders)
{
	// Init values
	const glm::mat4 projection = glm::perspective(glm::radians(GCamera->GetFieldOfView()), (float)GWindowWidth / (float)GWindowHeight, 0.1f, 100.f);
	const glm::mat4 view = GCamera->GetViewMatrix();
	
	GMatricesBuffer->SetValue(0, projection);
	GMatricesBuffer->SetValue(sizeof(glm::mat4), view);
	
	// Scene
	// * To custom framebuffer
	{
		GMSAAFramebuffer->Enable();

		// Setup
		{
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		
		// Draw skybox
		{
			Shaders[2]->Enable();
			GSkyboxObject->Draw(Shaders[2]);
			Shaders[2]->Disable();
		}
	
		// Draw scene
		{
			Shaders[0]->Enable();
			Shaders[0]->SetMat4("model", glm::mat4(1.f));
			GScene->Draw(Shaders[0], GCamera);
			Shaders[0]->Disable();
		}
		
		GMSAAFramebuffer->Disable();
	}

	// Screen rendering
	{
		static FFramebufferCopyArgs copyArgs;
		if(copyArgs.Source.Size.x == 0)
		{
			copyArgs.Source.Pos = { 0, 0 };
			copyArgs.Source.Size = { GWindowWidth, GWindowHeight };
			
			copyArgs.Destination = copyArgs.Source;
			
			copyArgs.DataType = FFramebufferCopyArgs::DT_Color;
			copyArgs.FilterType = FFramebufferCopyArgs::FT_Nearest;
		}
		
		GMSAAFramebuffer->CopyTo(GScreenFramebuffer, copyArgs);
	
		// Setup
		{
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		// Draw
		{
			Shaders[1]->Enable();

			GScreenObject->Draw(Shaders[1]);
			
			Shaders[1]->Disable();
		}
	}
}

void EngineTick()
{
	// Update FPS
	{
		FString resultTitle;
		resultTitle.append("LearnOpenGL: FPS [");
		resultTitle.append(std::to_string((uint16)std::floor(60.f / GDeltaSeconds)));
		resultTitle.append("]");
	
		glfwSetWindowTitle(GWindow, resultTitle.c_str());
	}
	
	// Position log
	{
		//const glm::vec3& currentPos = GCamera->GetPosition(); 
		//std::cout << "Current position: [" << currentPos.x << ',' << currentPos.y << ',' << currentPos.z << ']' << std::endl;
	}
}

int32 GuardedMain()
{
	GCamera = FCamera::Create();
	
	if (!CreateInitWindow(GWindow))
	{
		return -1;
	}

	TArray<FShaderProgramPtr> Shaders;
	if(!PrepareShaders(Shaders, GMatricesBuffer))
	{
		return -2;
	}
	
	if(!PrepareScreenScene(GScreenObject, GMSAAFramebuffer, GScreenFramebuffer))
	{
		return -3;
	}
	
	if(!PrepareScene(GScene))
	{
		return -4;
	}
	
	if(!PrepareSkybox(GSkyboxObject))
	{
		return -5;
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
		ProcessRender(Shaders);

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