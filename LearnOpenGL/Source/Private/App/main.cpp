
#include "ModuleMinimal.h"
#include "RenderUtils.h"
#include "FileUtils.h"
#include "MeshUtils.h"
#include "ColorUtils.h"
#include "Scene.h"
#include "TextureUtils.h"
#include "ShaderUtils.h"

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

// Window
uint16 GInitWindowWidth = 800;
uint16 GInitWindowHeight = 600;

GLFWwindow* GWindow = nullptr;

float GGamma = 1.5f; // 2.2f is best for most of the monitors

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

FFramebufferPtr GMSAAFramebuffer;
FFramebufferPtr GScreenFramebuffer;
FSceneObjectPtr GScreenObject;
FDepthCubemapPtr GShadowCubemap;

// TEST
uint16 GShadowResolution = 1024;
glm::vec2 GShadowClipPlane = {1.f, 25.f};
glm::mat4 GShadowTransforms[6];
uint8 GShadowCubemapIndex = 10;

glm::vec3 GLightPos = glm::vec3(-6.f, 10.f, 1.5f);
glm::vec4 GLightColor = NColors::White.ToVec4();
bool GUseBlinn = true;
bool GUseShadow = true;

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
				return NShaderUtils::GetSTD140Size<float>();
			default:
				ENSURE_NO_ENTRY();
				return 0;
		}
	}
};

struct FShaderMainType
{
	enum EType : uint8 { Invalid = 0, Mesh, Screen, Skybox, ShadowCubemap };
	
	static TArray<FUniformBufferMainType::EType> GetSupportedUniforms(EType Type)
	{
		switch (Type)
		{
			case Mesh:
				return { FUniformBufferMainType::Matrices, FUniformBufferMainType::Light };
			case Screen:
				return { FUniformBufferMainType::PostProcess };
			case Skybox:
				return { FUniformBufferMainType::Matrices };
			default:
				return {};
		}
	}
};

typedef FShaderMainType::EType EShaderMainType;
typedef FUniformBufferMainType::EType EUniformBufferMainType;

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
	FApplication::Get().SetWindowSize(Width, Height);
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

    BindEvents(OutWindow);

	glfwSetInputMode(OutWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
			NTextureUtils::GetFacesPathFromRoot(NFileUtils::ContentPath("Textures/Cubemaps/Skybox_high"), "jpg", false)
		)
	)->AsShared();

	return true;
}

bool PrepareScreenScene(FSceneObjectPtr& OutScreenObj, FFramebufferPtr& OutMSAAFramebuffer, FFramebufferPtr& OutScreenFramebuffer)
{
	uint16 windowWidth, windowHeight;
	FApplication::Get().GetWindowSize(windowWidth, windowHeight);

	// MSAA
	{
		FRenderTexturePtr multisampledTextureTarget = FRenderTexture::Create(windowWidth, windowHeight, ERenderTargetType::Color, 4);
		FRenderBufferPtr multisampledBufferTarget = FRenderBuffer::Create(windowWidth, windowHeight, ERenderTargetType::DepthAndStencil, 4);
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
		FRenderTexturePtr sceneTextureTarget = FRenderTexture::Create(windowWidth, windowHeight, ERenderTargetType::Color);
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

bool PrepareScene(FScenePtr& OutScene, FDepthCubemapPtr& OutShadowCubemap)
{
	FTexturePtr rocksFloorTexture = FTexture::Create(NFileUtils::ContentPath("Textures/ground.jpg").c_str(), ETextureType::Diffuse);
	FTexturePtr wallTexture = FTexture::Create(NFileUtils::ContentPath("Textures/Default/wall128x128.png").c_str(), ETextureType::Diffuse);
	FTexturePtr container = FTexture::Create(NFileUtils::ContentPath("Textures/container2.png").c_str(), ETextureType::Diffuse);
	FTexturePtr grassTexture = FTexture::Create(NFileUtils::ContentPath("Textures/grass.png").c_str(), ETextureType::Diffuse, false, true);
	FTexturePtr awesomeFace = FTexture::Create(NFileUtils::ContentPath("Textures/awesomeface.png").c_str(), ETextureType::Diffuse);
	if(!rocksFloorTexture->IsInitialized() || !wallTexture->IsInitialized() || !container->IsInitialized()|| !grassTexture->IsInitialized() || !awesomeFace->IsInitialized())
	{
		return false;
	}
	
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
	
	sceneObjects.push_back(NMeshUtils::ConstructSphere({awesomeFace}));
	sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::LightYellow);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			GLightPos,
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
	});
	
	sceneObjects.push_back(NMeshUtils::ConstructCube({container}));
	sceneObjects[sceneObjects.size() - 1]->SetOutlineSize(0.025f);
	sceneObjects[sceneObjects.size() - 1]->SetOutlineColor(NColors::Navy);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{2.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{1.f, 1.f, 1.f}
	});

	// LEFT
	sceneObjects.push_back(NMeshUtils::ConstructCube({container}));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{-2.f, 2.f, -2.f},
			{-45.f, 0.f, 0.f},
			{1.f, 1.f, 1.f}
	});

	// RIGHT
	sceneObjects.push_back(NMeshUtils::ConstructCube({container}));
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{-2.f, 1.f, 2.f},
			{0.f, -45.f, 0.f},
			{1.f, 1.f, 1.f}
	});
	
	sceneObjects.push_back(NMeshUtils::ConstructPlane({grassTexture}));
	sceneObjects[sceneObjects.size() - 1]->SetCullFaces(false);
	sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{-6.f, -0.499f, 0.f},
			{-90.f, 0.f, 90.f},
			{0.5f, 0.5f, 0.5f}
	});
	
	OutScene = FScene::Create(sceneObjects);
	OutShadowCubemap = FDepthCubemap::Create(GShadowResolution, GShadowResolution);
	
	return true;
}

bool PrepareShaders(TFastMap<EShaderMainType, FShaderProgramPtr>& OutShaders, TFastMap<EUniformBufferMainType, FUniformBufferPtr>& OutUniforms)
{
	// Compile and set shaders
	{
		OutShaders.insert({
			EShaderMainType::Mesh,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Mesh.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Mesh.frag").c_str())
		});
		
		OutShaders.insert({
			EShaderMainType::Screen,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Screen.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Screen.frag").c_str())
		});
		
		OutShaders.insert({
			EShaderMainType::Skybox,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/Skybox.vert").c_str(), NFileUtils::ContentPath("Shaders/Fragment/Skybox.frag").c_str())
		});
		
		OutShaders.insert({
			EShaderMainType::ShadowCubemap,
			FShaderProgram::Create(NFileUtils::ContentPath("Shaders/Vertex/ShadowCubemap.vert").c_str(), NFileUtils::ContentPath("Shaders/Geometry/ShadowCubemap.geom").c_str(), NFileUtils::ContentPath("Shaders/Fragment/ShadowCubemap.frag").c_str())
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
		
			ENSURE_RET(OutShaders[EShaderMainType::Mesh]->SetUniformBuffer(
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

	// Testing
	{
		const bool BWasPreviouslyPressed = GbBWasPressed;
		GbBWasPressed = (glfwGetKey(GWindow, GLFW_KEY_B) == GLFW_PRESS);
	
		const bool BWasJustPressed = !BWasPreviouslyPressed && GbBWasPressed;
		const bool BWasJustReleased = BWasPreviouslyPressed && !GbBWasPressed;
	
		if (BWasJustPressed)
		{
			GUseBlinn = !GUseBlinn;
		}
		
		
		const bool VWasPreviouslyPressed = GbVWasPressed;
		GbVWasPressed = (glfwGetKey(GWindow, GLFW_KEY_V) == GLFW_PRESS);
	
		const bool VWasJustPressed = !VWasPreviouslyPressed && GbVWasPressed;
		const bool VWasJustReleased = VWasPreviouslyPressed && !GbVWasPressed;
	
		if (VWasJustPressed)
		{
			GUseShadow = !GUseShadow;
		}
	}
}

bool InitRender(TFastMap<EUniformBufferMainType, FUniformBufferPtr>& Uniforms)
{
	// GAMMA
	Uniforms[EUniformBufferMainType::PostProcess]->SetValue(0, GGamma);
	
	// Shadow
	{
		const glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.f, GShadowClipPlane.x, GShadowClipPlane.y);
		
		GShadowTransforms[0] = shadowProj * glm::lookAt(GLightPos, GLightPos * glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
		GShadowTransforms[1] = shadowProj * glm::lookAt(GLightPos, GLightPos * glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
		GShadowTransforms[2] = shadowProj * glm::lookAt(GLightPos, GLightPos * glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
		GShadowTransforms[3] = shadowProj * glm::lookAt(GLightPos, GLightPos * glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
		GShadowTransforms[4] = shadowProj * glm::lookAt(GLightPos, GLightPos * glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f));
		GShadowTransforms[5] = shadowProj * glm::lookAt(GLightPos, GLightPos * glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f));
	}
	
	return true;
}

void ProcessRender(TFastMap<EShaderMainType, FShaderProgramPtr>& Shaders, TFastMap<EUniformBufferMainType, FUniformBufferPtr>& Uniforms)
{
	uint16 windowWidth, windowHeight;
	FApplication::Get().GetWindowSize(windowWidth, windowHeight);

	// TODO(kristian.fisera): Camera should hold projection (perspective, ortho)

	// Init values
	const glm::mat4 projection = glm::perspective(glm::radians(GCamera->GetFieldOfView()), (float)windowWidth / (float)windowHeight, 0.1f, 100.f);
	const glm::mat4 view = GCamera->GetViewMatrix();
	
	Uniforms[EUniformBufferMainType::Matrices]->SetValue(0, projection);
	Uniforms[EUniformBufferMainType::Matrices]->SetValue(NShaderUtils::GetSTD140Size<glm::mat4>(), view);
	
	Uniforms[EUniformBufferMainType::Light]->SetValue(0, GCamera->GetPosition());
	Uniforms[EUniformBufferMainType::Light]->SetValue(NShaderUtils::GetSTD140Size<glm::vec4>(), GUseBlinn);
	
	// Scene
	// * To shadow map
	if(GUseShadow)
	{
		GShadowCubemap->Enable();
		
		// Setup
		{
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// Draw scene
		{
			Shaders[EShaderMainType::ShadowCubemap]->Enable();

			// Setup
			{
				Shaders[EShaderMainType::ShadowCubemap]->SetFloat("shadowFarPlane", GShadowClipPlane.y);
				Shaders[EShaderMainType::ShadowCubemap]->SetVec3("lightPos", GLightPos);
				
				for (uint8 i = 0; i < 6; ++i)
				{
					const FString resultName = "shadowMatrices[" + std::to_string(i) + "]";
					Shaders[EShaderMainType::ShadowCubemap]->SetMat4(resultName.c_str(), GShadowTransforms[i]);
				}
			}
			
			GScene->Draw(Shaders[EShaderMainType::ShadowCubemap], GCamera);
			
			Shaders[EShaderMainType::ShadowCubemap]->Disable();
		}
		
		
		GShadowCubemap->Disable();
	}
	
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
			Shaders[EShaderMainType::Skybox]->Enable();
			GSkyboxObject->Draw(Shaders[EShaderMainType::Skybox]);
			Shaders[EShaderMainType::Skybox]->Disable();
		}
	
		// Draw scene
		{
			Shaders[EShaderMainType::Mesh]->Enable();

			// Setup light
			{
				Shaders[EShaderMainType::Mesh]->SetFloat("material.shininess", 8.f);
				
				Shaders[EShaderMainType::Mesh]->SetVec3("light.position", GLightPos);
				Shaders[EShaderMainType::Mesh]->SetVec3("light.diffuse", glm::vec3(GLightColor) * glm::vec3(0.5f));
				Shaders[EShaderMainType::Mesh]->SetVec3("light.ambient", glm::vec3(GLightColor) * 0.05f);
				Shaders[EShaderMainType::Mesh]->SetVec3("light.specular", {1.0f, 1.0f, 1.0f});
				
				Shaders[EShaderMainType::ShadowCubemap]->SetFloat("shadowFarPlane", GShadowClipPlane.y);
				
				Shaders[EShaderMainType::Mesh]->SetInt32("shadowMap", GShadowCubemapIndex);
				Shaders[EShaderMainType::Mesh]->SetBool("useShadow", GUseShadow);
			}
			
			GShadowCubemap->UseCube(GShadowCubemapIndex);
			
			GScene->Draw(Shaders[EShaderMainType::Mesh], GCamera);
			
			GShadowCubemap->ClearCube();
			
			Shaders[EShaderMainType::Mesh]->Disable();
		}
		
		GMSAAFramebuffer->Disable();
	}

	// Screen rendering
	{
		static FFramebufferCopyArgs copyArgs;
		if(copyArgs.Source.Size.x == 0)
		{
			copyArgs.Source.Pos = { 0, 0 };
			copyArgs.Source.Size = { windowWidth, windowHeight };
			
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
			Shaders[EShaderMainType::Screen]->Enable();

			GScreenObject->Draw(Shaders[EShaderMainType::Screen]);
			
			Shaders[EShaderMainType::Screen]->Disable();
		}
	}
}

bool EngineInit()
{
	GCamera = FCamera::Create(
		{-16.5f, 2.5f, -5.f}, 
		{19.f, -6.5f, 0.f} 
	);
	
	return true;
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
	if (!CreateInitWindow(GWindow))
	{
		return -1;
	}

	TFastMap<EShaderMainType, FShaderProgramPtr> Shaders;
	TFastMap<EUniformBufferMainType, FUniformBufferPtr> Uniforms;
	if(!PrepareShaders(Shaders, Uniforms))
	{
		return -2;
	}
	
	if(!PrepareScreenScene(GScreenObject, GMSAAFramebuffer, GScreenFramebuffer))
	{
		return -3;
	}
	
	if(!PrepareScene(GScene, GShadowCubemap))
	{
		return -4;
	}
	
	if(!PrepareSkybox(GSkyboxObject))
	{
		return -5;
	}

	if(!EngineInit())
	{
		return -6;
	}
	
	if(!InitRender(Uniforms))
	{
		return -7;
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
		ProcessRender(Shaders, Uniforms);

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