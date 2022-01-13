
#include "ModuleMinimal.h"
#include "RenderUtils.h"
#include "FileUtils.h"
#include "MeshUtils.h"
#include "ColorConsts.h"
#include "Scene.h"
#include "TextureUtils.h"
#include "ShaderUtils.h"
#include "RenderTargetUtils.h"

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
#include "DepthMap.h"
#include "DepthCubemap.h"

#include <random>
#include <cmath>

// Window
uint16 GInitWindowWidth = 1360;
uint16 GInitWindowHeight = 765;

GLFWwindow* GWindow = nullptr;

float GGamma = 1.5f; // 2.2f is best for most of the monitors

bool GViewportCapturesInput = (BUILD_DEBUG != 1);

// Global
float GLastSeconds = 0.f;
float GDeltaSeconds = 0.f;

// Key
bool GbShiftWasPressed = false;
bool GbAltWasPressed = false;
bool GbBWasPressed = false;
bool GbVWasPressed = false;

// Mouse
float GLastMouseX = GInitWindowWidth * 0.5f;
float GLastMouseY = GInitWindowHeight * 0.5f;

// Global instances
FCameraPtr GCamera;
FScenePtr GScene;

FSceneObjectPtr GSkyboxObject;
FSceneObjectPtr GScreenObject;

// SSAO
FTexturePtr GSSAONoiseTexture;
glm::vec3 GSSAOKernel[64];

bool GUseBlinn = true;
float GExposure = 0.15f;
bool GWireframeMode = false;
int32 GBloomIterations = 5;
bool GShouldReloadShaders = false;

uint16 GUIPreviousFPS = 0;
float GUIRefreshFPSEverySec = 0.5f;
bool GUIDemoVisible = false;
bool GUIDetailsInViewport = true;
bool GUIDebugVisible = false;
bool GUIResetLayout = true;

// ~ TEST

uint16 GNormalMappingObjIdx = 0;
uint16 GParalaxMappingObjIdx = 0;

struct FPointLightInfo
{
	uint16 BlockId;
	FColor Color;
	
	float Constant;
	float Linear;
	float Quadratic;
} GPointLights[3];

struct FDirectionalLightInfo
{
	glm::vec3 Direction;
	FColor Color;
} GDirLight;

// TEST

struct FFramebufferMainType
{
	enum EType : uint8 { Invalid = 0, SSAO, GBuffer };
};

struct FUniformBufferMainType
{
	enum EType : uint8 { Invalid = 0, Matrices, Light, PostProcess};
	
	static const char* ToString(EType Type)
	{
		switch (Type)
		{
			case Matrices:
				return "UMatrices";
			case Light:
				return "ULight";
			case PostProcess:
				return "UPostProcess";
			default:
				ENSURE_NO_ENTRY();
				return "";
		}
	}
	
	static int16 GetBindPoint(EType Type)
	{
		return (int16)(Type)-1;
	}
	
	static uint32 GetSize(EType Type)
	{
		switch (Type)
		{
			case Matrices:
				return NShaderUtils::GetSTD140Size<glm::mat4>() * 2;
			case Light:
				return NShaderUtils::GetSTD140Size<glm::vec4>() + NShaderUtils::GetSTD140Size<bool>();
			case PostProcess:
				return NShaderUtils::GetSTD140Size<float>() * 2;
			default:
				ENSURE_NO_ENTRY();
				return 0;
		}
	}
};

struct FShaderMainType
{
	enum EType : uint8 { Invalid = 0, Mesh, Screen, Skybox, Blur, SSAO };
	
	static TArray<FUniformBufferMainType::EType> GetSupportedUniforms(EType Type)
	{
		switch (Type)
		{
			case SSAO:
			case Mesh:
				return { FUniformBufferMainType::Matrices };
			case Screen:
				return { FUniformBufferMainType::Light, FUniformBufferMainType::PostProcess };
			case Skybox:
				return { FUniformBufferMainType::Matrices };
			default:
				return {};
		}
	}
};

typedef FFramebufferMainType::EType EFramebufferMainType;
typedef FUniformBufferMainType::EType EUniformBufferMainType;
typedef FShaderMainType::EType EShaderMainType;

TFastMap<EShaderMainType, FShaderProgramPtr> GShaders;
TFastMap<EUniformBufferMainType, FUniformBufferPtr> GUniforms;
TFastMap<EFramebufferMainType, FFramebufferPtr> GFramebuffers;

void MouseScrollChanged(GLFWwindow* window, double ScrollX, double ScrollY)
{
	if(GViewportCapturesInput)
	{
		GCamera->ProcessScroll((float)ScrollY);
	}
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
	FApplication::Get().SetWindowSize(Width, Height);
}

void GLFWError(int32 ErrorCode, const char* Message)
{
	std::cout << "GLFW error occurs: [" << ErrorCode << "] " << Message << std::endl;
	ENSURE_NO_ENTRY();
}

bool CreateInitWindow(GLFWwindow*& OutWindow)
{
	glfwSetErrorCallback(&GLFWError);

	if (!glfwInit())
	{
		std::cout << "failed to initialize GLFW" << std::endl;
		return false;
	}

	// DEBUG context
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	OutWindow = glfwCreateWindow(GInitWindowWidth, GInitWindowHeight, "LearnOpenGL", nullptr, nullptr);
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

	glfwSetFramebufferSizeCallback(OutWindow, &WindowSizeChanged);
	glfwSetCursorPosCallback(OutWindow, &MousePositionChanged);
	glfwSetScrollCallback(OutWindow, &MouseScrollChanged);

	glfwSetInputMode(OutWindow, GLFW_CURSOR, GViewportCapturesInput ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	FApplication::Get().SetWindowSize(GInitWindowWidth, GInitWindowHeight);

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
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
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

bool CreateInitUI(GLFWwindow* Window)
{
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(Window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGui::StyleColorsDark();

	return true;
}

bool PrepareSkybox(FSceneObjectPtr& OutSkyboxObj)
{
	OutSkyboxObj = FSkybox::Create(
		FCubemap::Create(
			NTextureUtils::GetFacesPathFromRoot(NFileUtils::ContentPath("Textures/Cubemaps/Skybox_high"), "jpg", false)
		)
	)->AsShared();

	return true;
}

bool PrepareFBs(TFastMap<EFramebufferMainType, FFramebufferPtr>& OutFramebuffers)
{
	uint16 windowWidth, windowHeight;
	FApplication::Get().GetWindowSize(windowWidth, windowHeight);

	// SSAO
	{
		FRenderTexturePtr colorTarget = FRenderTexture::Create(
			windowWidth,
			windowHeight,
			ERenderTargetAttachmentType::Color,
			ERenderTextureFlag::Size8 | ERenderTextureFlag::Float | ERenderTextureFlag::R
		);

		if(!colorTarget->IsInitialized())
		{
			return false;
		}

		auto fb = OutFramebuffers.insert({
			EFramebufferMainType::SSAO,
			FFramebuffer::Create()
		});
		
		fb.first->second->Attach(colorTarget->AsShared());
	}

	// GBuffer
	{
		// TODO(kristian.fisera): Position can be determined from depth
		// * see: https://mynameismjp.wordpress.com/2010/09/05/position-from-depth-3/
		FRenderTexturePtr positionTarget = FRenderTexture::Create(
			windowWidth,
			windowHeight,
			ERenderTargetAttachmentType::Color,
			ERenderTextureFlag::Size16 | ERenderTextureFlag::Float | ERenderTextureFlag::RGBA
		);
		
		FRenderTexturePtr normalTarget = FRenderTexture::Create(
			windowWidth,
			windowHeight,
			ERenderTargetAttachmentType::Color,
			ERenderTextureFlag::Size16 | ERenderTextureFlag::Float | ERenderTextureFlag::RGBA
		);
		
		FRenderTexturePtr albedoWithSpecTarget = FRenderTexture::Create(
			windowWidth,
			windowHeight,
			ERenderTargetAttachmentType::Color
		);
		
		if(!positionTarget->IsInitialized() || !normalTarget->IsInitialized() || !albedoWithSpecTarget->IsInitialized())
		{
			return false;
		}
		
		FRenderBufferPtr depthTarget = FRenderBuffer::Create(
				windowWidth,
				windowHeight,
				ERenderTargetAttachmentType::DepthOnly
		);
		
		if(!depthTarget->IsInitialized())
		{
			return false;
		}
		
		auto fb = OutFramebuffers.insert({
			EFramebufferMainType::GBuffer,
			FFramebuffer::Create()
		});
		
		fb.first->second->Attach(positionTarget->AsShared());
		fb.first->second->Attach(normalTarget->AsShared());
		fb.first->second->Attach(albedoWithSpecTarget->AsShared());
		fb.first->second->Attach(depthTarget->AsShared());
	}
	
	// Quad
	{
		static const TArray<FMesh2DVertex> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			{ glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{ glm::vec2( -1.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
			{ glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{ glm::vec2( -1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{ glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{ glm::vec2( 1.0f,  1.0f), glm::vec2(1.0f, 1.)}
		};
		
		GScreenObject = FMesh2D::Create(quadVertices, {})->AsShared();
		GScreenObject->SetCullFaces(false);
	}
	
	return true;
}

bool PrepareScene(FScenePtr& OutScene)
{
	FTexturePtr blankTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/blank.jpg").c_str(), ETextureType::Diffuse);
	FTexturePtr rocksFloorTexture = FTexture::Create(NFileUtils::ContentPath("Textures/ground.jpg").c_str(), ETextureType::Diffuse);
	FTexturePtr wallTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/wall128x128.png").c_str(), ETextureType::Diffuse);
	FTexturePtr container = FTexture::Create(NFileUtils::ContentPath("Textures/container2.png").c_str(), ETextureType::Diffuse);
	FTexturePtr containerSpecular = FTexture::Create(NFileUtils::ContentPath("Textures/container2_specular.png").c_str(), ETextureType::Specular);
	
	FTexturePtr brickWall = FTexture::Create(NFileUtils::ContentPath("Textures/brickWall.jpg").c_str(), ETextureType::Diffuse);
	FTexturePtr brickWallNormal = FTexture::Create(NFileUtils::ContentPath("Textures/brickWall_normal.jpg").c_str(), ETextureType::Normals);
	
	FTexturePtr bricks = FTexture::Create(NFileUtils::ContentPath("Textures/bricks.jpg").c_str(), ETextureType::Diffuse);
	FTexturePtr bricksNormal = FTexture::Create(NFileUtils::ContentPath("Textures/bricks_normal.jpg").c_str(), ETextureType::Normals, true, true);
	FTexturePtr bricksHeight = FTexture::Create(NFileUtils::ContentPath("Textures/bricks_height.jpg").c_str(), ETextureType::Height, true, true);
	
	FTexturePtr toy = FTexture::Create(NFileUtils::ContentPath("Textures/wood.png").c_str(), ETextureType::Diffuse);
	FTexturePtr toyNormal = FTexture::Create(NFileUtils::ContentPath("Textures/toy_box_normal.png").c_str(), ETextureType::Normals, true, true);
	FTexturePtr toyHeight = FTexture::Create(NFileUtils::ContentPath("Textures/toy_box_height.png").c_str(), ETextureType::Height, true, true);	
	
	if( !blankTexture->IsInitialized() ||
		!rocksFloorTexture->IsInitialized() ||
		!wallTexture->IsInitialized() ||
		!container->IsInitialized() ||
		!containerSpecular->IsInitialized() ||
		!brickWall->IsInitialized() ||
		!brickWallNormal->IsInitialized() ||
		!bricks->IsInitialized() ||
		!bricksNormal->IsInitialized() ||
		!bricksHeight->IsInitialized() ||
		!toy->IsInitialized() ||
		!toyNormal->IsInitialized() ||
		!toyHeight->IsInitialized())
	{
		return false;
	}
	
	FModelPtr backpack = FModel::Create(NFileUtils::ContentPath("Models/Backpack/backpack.obj").c_str());

	if( !backpack->IsInitialized() )
	{
		return false;
	}

	// HELPER for scene
	// X -> forward
	// Y -> up
	// Z -> right
	
	TArray<FSceneObjectPtr> sceneObjects;
	sceneObjects.push_back(NMeshUtils::ConstructPlane({rocksFloorTexture}));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
		{0.f, -1.f, 0.f},
		{0.f, 0.f, 0.f},
		{25.f, 1.f, 25.f}
	});

	sceneObjects.push_back(NMeshUtils::ConstructSphere({wallTexture}));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
		{10.f, 10.f, 10.f},
		{0.f, 0.f, 0.f},
		{2.f, 2.f, 2.f}
	});
	
	// CENTER
	sceneObjects.push_back(NMeshUtils::ConstructCube({container, containerSpecular}));
	//sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	//sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::Navy);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
		{2.f, 0.f, 0.f},
		{0.f, 0.f, 0.f},
		{1.f, 1.f, 1.f}
	});

	// LEFT
	sceneObjects.push_back(NMeshUtils::ConstructCube({container, containerSpecular}));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
		{-2.f, 2.f, -2.f},
		{-45.f, 0.f, 0.f},
		{1.f, 1.f, 1.f}
	});

	// RIGHT
	sceneObjects.push_back(NMeshUtils::ConstructCube({container, containerSpecular}));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
		{-2.f, 1.f, 2.f},
		{0.f, -45.f, 0.f},
		{1.f, 1.f, 1.f}
	});

	// BAG
	{
		sceneObjects.push_back(backpack->AsShared());
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{3.35f, -0.35f, 0.f},
			{0.f, 90.f, 0.f},
			{0.35f, 0.35f, 0.35f}
		});
	}

	// NORMAL MAPPING TEST
	{
		sceneObjects.push_back(NMeshUtils::ConstructCube({brickWall, brickWallNormal}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{15.f, 2.5f, -2.f},
			{90.f, 0.f, 0.f},
			{1.f, 1.f, 1.f}
		});
		
		GNormalMappingObjIdx = (uint16)sceneObjects.size() - 1;
	}

	// PARALAX MAPPING TEST
	{
		sceneObjects.push_back(NMeshUtils::ConstructCube({bricks, bricksNormal, bricksHeight}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{15.f, 2.5f, 2.f},
			{90.f, 0.f, 0.f},
			{1.f, 1.f, 1.f}
		});
		
		GParalaxMappingObjIdx = (uint8)sceneObjects.size() - 1;
		
		sceneObjects.push_back(NMeshUtils::ConstructPlane({toy, toyNormal, toyHeight}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{17.f, 0.f, 0.f},
			{0.f, 0.f, 90.f},
			{1.f, 1.f, 1.f}
		});
	}
	
	// DIRECTIONAL LIGHT
	{
		GDirLight = { {0.f, -0.5f, -0.5f}, NColors::White };
	}

	// POINT LIGHTS
	{
		sceneObjects.push_back(NMeshUtils::ConstructCube({blankTexture}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{1.5f, 0.1f, 3.f},
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
		});
		GPointLights[0] = {(uint16)(sceneObjects.size() - 1), NColors::White * 2.f , 0.f, 0.f, 0.032f};
	
		sceneObjects.push_back(NMeshUtils::ConstructCube({blankTexture}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{10.5f, 5.f, 0.f},
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
		});
		GPointLights[1] = {(uint16)(sceneObjects.size() - 1), NColors::White * 6.f, 0.f, 0.f, 0.032f };
		
		sceneObjects.push_back(NMeshUtils::ConstructCube({blankTexture}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{1.5f, 0.1f, -3.f},
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
		});
		GPointLights[2] = {(uint16)(sceneObjects.size() - 1), NColors::White * 2.f, 0.f, 0.f, 0.032f };
	}

	OutScene = FScene::Create(sceneObjects);
	
	return true;
}

bool PrepareShaders(TFastMap<EShaderMainType, FShaderProgramPtr>& OutShaders, TFastMap<EUniformBufferMainType, FUniformBufferPtr>& OutUniforms)
{
	// Compile and set gshaders
	{
		OutShaders.insert({
			EShaderMainType::Mesh,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/MeshDeffered.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/MeshDeffered.frag").c_str())
		});
		
		OutShaders.insert({
			EShaderMainType::Screen,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/ScreenDeffered.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/ScreenDeffered.frag").c_str())
		});
		
		OutShaders.insert({
			EShaderMainType::Skybox,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Skybox.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Skybox.frag").c_str())
		});

		OutShaders.insert({
			EShaderMainType::SSAO,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/SSAODeffered.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/SSAODeffered.frag").c_str())
		});
	}
	
	for(const auto& shader : OutShaders)
	{
		if(!shader.second->IsInitialized()) return false;
	}

	// Uniform buffers
	{
		// Matrices
		{
			constexpr EUniformBufferMainType currentType = EUniformBufferMainType::Matrices;
			
			OutUniforms.insert({
				currentType,
				FUniformBuffer::Create(
					FUniformBufferMainType::GetBindPoint(currentType), 
					FUniformBufferMainType::GetSize(currentType)
				)
			});
			
			ENSURE_RET(OutShaders[EShaderMainType::SSAO]->SetUniformBuffer(
				FUniformBufferMainType::ToString(currentType), OutUniforms[currentType].GetRef()), 
			false);

			ENSURE_RET(OutShaders[EShaderMainType::Mesh]->SetUniformBuffer(
				FUniformBufferMainType::ToString(currentType), OutUniforms[currentType].GetRef()), 
			false);
			
			ENSURE_RET(OutShaders[EShaderMainType::Skybox]->SetUniformBuffer(
				FUniformBufferMainType::ToString(currentType), OutUniforms[currentType].GetRef()),
			false);
		}

		// Light
		{
			constexpr EUniformBufferMainType currentType = EUniformBufferMainType::Light;
			
			OutUniforms.insert({
				currentType,
				FUniformBuffer::Create(
					FUniformBufferMainType::GetBindPoint(currentType), 
					FUniformBufferMainType::GetSize(currentType)
				)
			});
		
			ENSURE_RET(OutShaders[EShaderMainType::Screen]->SetUniformBuffer(
				FUniformBufferMainType::ToString(currentType), OutUniforms[currentType].GetRef()), 
			false);
		}
		
		// PostProcess
		{
			constexpr EUniformBufferMainType currentType = EUniformBufferMainType::PostProcess;
			
			OutUniforms.insert({
				currentType,
				FUniformBuffer::Create(
					FUniformBufferMainType::GetBindPoint(currentType), 
					FUniformBufferMainType::GetSize(currentType)
				)
			});
		
			ENSURE_RET(OutShaders[EShaderMainType::Screen]->SetUniformBuffer(
				FUniformBufferMainType::ToString(currentType), OutUniforms[currentType].GetRef()), 
			false);
		}
	}

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

	// Cursor
	{
		if(altWasJustPressed)
		{
			GViewportCapturesInput = !GViewportCapturesInput;

			GCamera->SetShouldProcessInput(GViewportCapturesInput);
			glfwSetInputMode(GWindow, GLFW_CURSOR, GViewportCapturesInput ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}
	}

	// Rotation && Translation && Opacity
	{
		if(shiftWasJustPressed)
			GCamera->SetMoveSensitivity(GCamera->GetMoveSensitivity()*2.f);
		if(shiftWasJustReleased)
			GCamera->SetMoveSensitivity(GCamera->GetMoveSensitivity()*0.5f);

		if (glfwGetKey(GWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
			GCamera->ProcessMoveInput(ECameraMoveDirection::Up, GDeltaSeconds);
		if (glfwGetKey(GWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			GCamera->ProcessMoveInput(ECameraMoveDirection::Down, GDeltaSeconds);
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

bool InitRender(TFastMap<EUniformBufferMainType, FUniformBufferPtr>& GUniforms)
{
	return true;
}

bool InitUI()
{
	return true;
}

void ProcessRender()
{
	uint16 windowWidth, windowHeight;
	FApplication::Get().GetWindowSize(windowWidth, windowHeight);

	// TODO(kristian.fisera): Camera should hold projection (perspective, ortho)

	// Init values
	const glm::mat4 projection = glm::perspective(glm::radians(GCamera->GetFieldOfView()), (float)windowWidth / (float)windowHeight, 0.1f, 100.f);
	const glm::mat4 view = GCamera->GetViewMatrix();
	
	GUniforms[EUniformBufferMainType::Matrices]->SetValue(0, projection);
	GUniforms[EUniformBufferMainType::Matrices]->SetValue(NShaderUtils::GetSTD140Size<glm::mat4>(), view);
	
	GUniforms[EUniformBufferMainType::Light]->SetValue(0, GCamera->GetPosition());
	GUniforms[EUniformBufferMainType::Light]->SetValue(NShaderUtils::GetSTD140Size<glm::vec4>(), GUseBlinn);
	
	GUniforms[EUniformBufferMainType::PostProcess]->SetValue(0, GGamma);
	GUniforms[EUniformBufferMainType::PostProcess]->SetValue(NShaderUtils::GetSTD140Size<float>(), GExposure);
	
	// Scene
	// * To custom framebuffer
	{
		GFramebuffers[EFramebufferMainType::GBuffer]->Enable();

		// Setup
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		
		// Draw skybox
		{
/*			GShaders[EShaderMainType::Skybox]->Enable();
			GSkyboxObject->Draw(GShaders[EShaderMainType::Skybox]);
			GShaders[EShaderMainType::Skybox]->Disable();*/
		}
	
		// Draw scene
		{
			GShaders[EShaderMainType::Mesh]->Enable();

			// Setup guniforms
			{
				GShaders[EShaderMainType::Mesh]->SetVec3("viewPos", GCamera->GetPosition());
				
				GShaders[EShaderMainType::Mesh]->SetFloat("parallax.minLayers", 8.f);
				GShaders[EShaderMainType::Mesh]->SetFloat("parallax.maxLayers", 32.f);
				
				GShaders[EShaderMainType::Mesh]->SetFloat("parallax.scale", 0.1f);
			}
			
			GScene->Draw(GShaders[EShaderMainType::Mesh], GCamera);
			
			GShaders[EShaderMainType::Mesh]->Disable();
		}
		
		GFramebuffers[EFramebufferMainType::GBuffer]->Disable();
	}

			
	TArray<FTexturePtr> gTextures = NRenderTargetUtils::TryGetAsTextures(
		GFramebuffers[EFramebufferMainType::GBuffer],
		ERenderTargetAttachmentType::Color,
		ETextureType::Diffuse
	);
	
	ENSURE(gTextures.size() == 3);
	
	// SSAO
	{
		GFramebuffers[EFramebufferMainType::SSAO]->Enable();

		// Setup
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}

		// Draw
		{
			GShaders[EShaderMainType::SSAO]->Enable();

			// Setup textures
			{
				for(uint8 i = 0; i < 64; ++i)
					GShaders[EShaderMainType::SSAO]->SetVec3(FString("samples[" + std::to_string(i) + "]").c_str(), GSSAOKernel[i]);

				GShaders[EShaderMainType::SSAO]->SetInt32("gBuffer.position", 0);
				GShaders[EShaderMainType::SSAO]->SetInt32("gBuffer.normal", 1);

				GShaders[EShaderMainType::SSAO]->SetVec2("noise.scale", {windowWidth * 0.25f, windowHeight * 0.25f});
				GShaders[EShaderMainType::SSAO]->SetInt32("noise.map", 2);
			}

			GShaders[EShaderMainType::SSAO]->Disable();
		}

		gTextures[0]->Use(0);
		gTextures[1]->Use(1);
		GSSAONoiseTexture->Use(2);

		GScreenObject->Draw(GShaders[EShaderMainType::SSAO]);

		GSSAONoiseTexture->Clear();
		gTextures[0]->Clear();
		gTextures[1]->Clear();

		GFramebuffers[EFramebufferMainType::SSAO]->Disable();
	}

	FTexturePtr ssaoTexture = NRenderTargetUtils::TryGetAsTexture(
		GFramebuffers[EFramebufferMainType::SSAO],
		ERenderTargetAttachmentType::Color,
		ETextureType::Diffuse,
		0
	);

	// Screen rendering
	{
		// Setup
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// Draw quad
		{
			GShaders[EShaderMainType::Screen]->Enable();

			// Setup g-buffer
			{
				GShaders[EShaderMainType::Screen]->SetInt32("gBuffer.position", 0);
				GShaders[EShaderMainType::Screen]->SetInt32("gBuffer.normal", 1);
				GShaders[EShaderMainType::Screen]->SetInt32("gBuffer.albedoSpecular", 2);
			}

			// Setup SSAO
			{
				GShaders[EShaderMainType::Screen]->SetInt32("ssao.map", 3);
			}
			
			// Setup light
			{
				GShaders[EShaderMainType::Screen]->SetFloat("material.shininess", 8.f);

				GShaders[EShaderMainType::Screen]->SetVec3("dirLight.direction", GDirLight.Direction);
				GShaders[EShaderMainType::Screen]->SetVec3("dirLight.diffuse", GDirLight.Color.ToVec3() * 0.5f);
				GShaders[EShaderMainType::Screen]->SetVec3("dirLight.ambient", GDirLight.Color.ToVec3() * 0.05f);
				GShaders[EShaderMainType::Screen]->SetVec3("dirLight.specular", {0.05f, 0.05f, 0.05f});

				for(uint8 i = 0; i < 3; ++i)
				{
					const FString uniformName = "pointLight[" + std::to_string(i) + "]";
				
					GShaders[EShaderMainType::Screen]->SetVec3(FString(uniformName + ".position").c_str(), GScene->GetObjectByIdx(GPointLights[i].BlockId)->GetTransform().Position);
					GShaders[EShaderMainType::Screen]->SetVec3(FString(uniformName + ".diffuse").c_str(), GPointLights[i].Color.ToVec3() * 0.5f);
					GShaders[EShaderMainType::Screen]->SetVec3(FString(uniformName + ".ambient").c_str(), GPointLights[i].Color.ToVec3() * 0.05f);
					GShaders[EShaderMainType::Screen]->SetVec3(FString(uniformName + ".specular").c_str(), {0.05f, 0.05f, 0.05f});
					
					GShaders[EShaderMainType::Screen]->SetFloat(FString(uniformName + ".constant").c_str(), GPointLights[i].Constant);
					GShaders[EShaderMainType::Screen]->SetFloat(FString(uniformName + ".linear").c_str(), GPointLights[i].Linear);
					GShaders[EShaderMainType::Screen]->SetFloat(FString(uniformName + ".quadratic").c_str(), GPointLights[i].Quadratic);
				}
			}
			
			gTextures[0]->Use(0);
			gTextures[1]->Use(1);
			gTextures[2]->Use(2);
			ssaoTexture->Use(3);
			
			GScreenObject->Draw(GShaders[EShaderMainType::Screen]);
			
			ssaoTexture->Clear();
			gTextures[2]->Clear();
			gTextures[1]->Clear();
			gTextures[0]->Clear();
			
			GShaders[EShaderMainType::Screen]->Disable();
		}
	}
}

void ProcessUIRender()
{
	// Menu bar
	{
		if(ImGui::BeginMainMenuBar())
		{
			if(ImGui::BeginMenu("Rendering"))
			{
				if(ImGui::Checkbox("Wireframe", &GWireframeMode))
				{
					glPolygonMode(GL_FRONT_AND_BACK, GWireframeMode ? GL_LINE : GL_FILL);
				}

				if(ImGui::Button("Reload gshaders"))
				{
					GShouldReloadShaders = true;
				}

				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("Settings"))
			{
				ImGui::Checkbox("Details in viewport", &GUIDetailsInViewport);
				ImGui::Checkbox("Debug visible", &GUIDebugVisible);
				ImGui::InputFloat("FPS counter refresh", &GUIRefreshFPSEverySec);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if(ImGui::Button((!GUIDemoVisible) ? "Show DEMO" : "Hide DEMO"))
					GUIDemoVisible = !GUIDemoVisible;

				if(ImGui::Button("Reset Layout"))
					GUIResetLayout = true;

				ImGui::EndMenu();
			}

			if(ImGui::Button("Quit"))
			{
				glfwSetWindowShouldClose(GWindow, true);
			}

			ImGui::EndMainMenuBar();
		}
	}

	// Details
	{
		if(!GViewportCapturesInput || GUIDetailsInViewport)
		{
			static float currentWaitTime = 0;
			currentWaitTime += GDeltaSeconds;
			
			// Refresh each second 
			if(currentWaitTime >= GUIRefreshFPSEverySec)
			{
				currentWaitTime = 0.f;
				GUIPreviousFPS = (uint16)(60.f / GDeltaSeconds);
			}

			static ImVec2 defaultPos(FApplication::Get().GetWindowWidth() - 255.f, 20.f);
			static ImVec2 defaultSize(255.f, 105.f);

			if(GUIResetLayout)
			{
				ImGui::SetNextWindowPos(defaultPos);
				ImGui::SetNextWindowSize(defaultSize);
			}

			ImGui::Begin("Details");
			ImGui::Text("FPS: %d (%.3f ms)", GUIPreviousFPS, 1000.0f / GUIPreviousFPS);
			if(ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent();

				const glm::vec3& pos = GCamera->GetPosition();
				const glm::vec3& rot = GCamera->GetRotation();

				ImGui::Text("Position: [%.1f, %.1f, %.1f]", pos.x, pos.y, pos.z);
				ImGui::Text("Rotation: [%.1f, %.1f, %.1f]", rot.x, rot.y, rot.z);

				ImGui::Unindent();
			}
			ImGui::End();
		}
	}

	// Debug
	{
		if(!GViewportCapturesInput && GUIDebugVisible)
		{
			static const ImVec2 defaultPos(0.f, FApplication::Get().GetWindowHeight() - 300.f);
			static const ImVec2 defaultPosRaised(defaultPos.x, defaultPos.y - 150.f);

			static const ImVec2 defaultSize(475.f, 300.f);
			static const ImVec2 defaultSizeRaised(defaultSize.x, defaultSize.y + 150.f);

			static bool debugWindowSizeDirty = false;
			static bool colorPickerWasVisible = false;

			if(GUIResetLayout || debugWindowSizeDirty)
			{
				ImGui::SetNextWindowPos((colorPickerWasVisible) ? defaultPosRaised : defaultPos);
				ImGui::SetNextWindowSize((colorPickerWasVisible) ? defaultSizeRaised : defaultSize);

				debugWindowSizeDirty = false;
			}

			ImGui::Begin("Debug", &GUIDebugVisible);

			ImGui::Checkbox("Blinn shading", &GUseBlinn);
			ImGui::SliderFloat("Gamma", &GGamma, 0.f, 5.f);
			ImGui::SliderFloat("HDR exposure", &GExposure, 0.f, 5.f);
			ImGui::SliderInt("Bloom iterations", &GBloomIterations, 0, 10);

			bool colorPickerVisible = false;
			if(ImGui::CollapsingHeader("Point Lights"))
			{
				ImGui::Indent();

				if(ImGui::CollapsingHeader("Directional"))
				{
					ImGui::Indent();

					ImGui::SliderFloat3("Direction", &GDirLight.Direction.x, -1.f, 1.f);

					ImGui::Spacing();

					glm::vec4 colorCopy = GDirLight.Color.ToVec4();
					if(ImGui::ColorPicker4("Color", &colorCopy.x), ImGuiColorEditFlags_NoSidePreview)
					{
						GDirLight.Color = FColor::FromVec4(colorCopy);
					}

					ImGui::Unindent();
				}

				for(uint8 i = 0; i < 3; ++i)
				{
					if(ImGui::CollapsingHeader(FString("Point: " + std::to_string(i)).c_str()))
					{
						colorPickerVisible = true;

						ImGui::Indent();

						glm::vec3 positionCopy = GScene->GetObjectByIdx(GPointLights[i].BlockId)->GetTransform().Position;
						glm::vec4 colorCopy = GPointLights[i].Color.ToVec4();
						
						if(ImGui::SliderFloat3("Position", &positionCopy.x, -25.f, 25.f))
						{
							auto obj = GScene->GetObjectByIdx(GPointLights[i].BlockId);
							FTransform objTransform = obj->GetTransform();
							objTransform.Position = positionCopy;
							obj->SetTransform(objTransform);
						}
						
						ImGui::SliderFloat("Constant", &GPointLights[i].Constant, 0.f, 10.f);
						ImGui::SliderFloat("Linear", &GPointLights[i].Linear, 0.f, 2.f);
						ImGui::SliderFloat("Quadratic", &GPointLights[i].Quadratic, 0.f, 2.f);
						
						ImGui::Spacing();
						
						if(ImGui::ColorPicker4("Color", &colorCopy.x,
							ImGuiColorEditFlags_HDR |
							ImGuiColorEditFlags_NoSidePreview |
							ImGuiColorEditFlags_AlphaBar))
						{
							GPointLights[i].Color = FColor::FromVec4(colorCopy);
						}

						ImGui::Unindent();
					}
				}

				ImGui::Unindent();
			}

			if(colorPickerVisible != colorPickerWasVisible)
			{
				colorPickerWasVisible = colorPickerVisible;
				debugWindowSizeDirty = true;
			}

			ImGui::End();
		}
	}

	// Demo
	{
		if(GUIDemoVisible)
		{
			ImGui::ShowDemoWindow(&GUIDemoVisible);
		}
	}

	// Reset layout
	GUIResetLayout = false;
}

bool EngineInit()
{
	GCamera = FCamera::Create(
		{-16.5f, 2.5f, -5.f}, 
		{19.f, -6.5f, 0.f} 
	);

	GCamera->SetShouldProcessInput(BUILD_DEBUG != 1);
	
	// SSAO
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;

		// kernel
		{
			auto lerpFunc =
			[](float a, float b, float f)
			{
				return a + f * (b - a);
			};

			for (uint8 i = 0; i < 64; ++i)
			{
				glm::vec3 sample(
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator)
				);

				sample = glm::normalize(sample);
				sample *= randomFloats(generator);

				float scale = float(i) / 64.0;

				// scale samples s.t. they're more aligned to center of kernel
				scale = lerpFunc(0.1f, 1.f, scale * scale);
				sample *= scale;
				GSSAOKernel[i] = sample;
			}
		}

		// noise
		{
			TArray<glm::vec4> noiseData;
			for (uint8 i = 0; i < 16; ++i)
			{
				glm::vec4 noise(
					randomFloats(generator) * 2.0 - 1.0, 
					randomFloats(generator) * 2.0 - 1.0, 
					0.f, 0.f);

				noiseData.push_back(noise);
			}

			GSSAONoiseTexture = FTexture::Create(noiseData, ETextureType::Diffuse, true, true);
		}
	}

	return true;
}

void EngineTick()
{
	if(GShouldReloadShaders)
	{
		GShaders.clear();
		GUniforms.clear();

		PrepareShaders(GShaders, GUniforms);
		GShouldReloadShaders = false;
	}
	
	// Rotate over time
	{
		const float degrees = GDeltaSeconds * 10.f; // ten degrees in second
		
		// Normal mapping test
		{
			FSceneObjectPtr object = GScene->GetObjectByIdx(GNormalMappingObjIdx);
			FTransform transCopy = object->GetTransform();
			
			if(transCopy.Rotation.z - degrees < 0.f)
				transCopy.Rotation.z = transCopy.Rotation.z - degrees + 360.f;
			else
				transCopy.Rotation.z -= degrees;
				
			object->SetTransform(transCopy);
		}

		// Paralax mapping test
		{
			FSceneObjectPtr object = GScene->GetObjectByIdx(GParalaxMappingObjIdx);
			FTransform transCopy = object->GetTransform();
			
			if(transCopy.Rotation.z + degrees > 360.f)
				transCopy.Rotation.z = transCopy.Rotation.z + degrees - 360.f;
			else
				transCopy.Rotation.z += degrees;
				
			object->SetTransform(transCopy);
		}
	}
}

int32 GuardedMain()
{
	if (!CreateInitWindow(GWindow))
	{
		return -1;
	}

	if(!CreateInitUI(GWindow))
	{
		return -2;
	}
	if(!PrepareShaders(GShaders, GUniforms))
	{
		return -3;
	}
	
	if(!PrepareFBs(GFramebuffers))
	{
		return -4;
	}
	
	if(!PrepareScene(GScene))
	{
		return -5;
	}
	
	if(!PrepareSkybox(GSkyboxObject))
	{
		return -6;
	}

	if(!EngineInit())
	{
		return -7;
	}
	
	if(!InitRender(GUniforms))
	{
		return -8;
	}

	if(!InitUI())
	{
		return -9;
	}

	// Main render loop
	FTimer frameTimer;
	while (!glfwWindowShouldClose(GWindow))
	{
		frameTimer.Reset();
		frameTimer.Start();

		EngineTick();

		ProcessInput();
		ProcessRender();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ProcessUIRender();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(GWindow);
		glfwPollEvents();

		frameTimer.Stop();
		
		GDeltaSeconds = GLastSeconds;
		GLastSeconds = (float)frameTimer.GetSeconds();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

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