
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

bool GUseBlinn = true;
float GExposure = 0.15f;
bool GWireframeMode = false;
int32 GBloomIterations = 5;

bool GUIDemoVisible = false;
bool GUIDetailsInViewport = true;
bool GUIDebugVisible = false;
bool GUIResetLayout = true;

// ~ TEST

struct FLightInfo
{
	uint64 BlockId;
	FColor Color;
	
	float Constant;
	float Linear;
	float Quadratic;
} GLights[3];

// TEST

struct FFramebufferMainType
{
	enum EType : uint8 { Invalid = 0, GBuffer };
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
	enum EType : uint8 { Invalid = 0, Mesh, Screen, Skybox, Blur };
	
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

typedef FFramebufferMainType::EType EFramebufferMainType;
typedef FUniformBufferMainType::EType EUniformBufferMainType;
typedef FShaderMainType::EType EShaderMainType;

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

	// GBuffer
	{
		FRenderTexturePtr positionTarget = FRenderTexture::Create(
				windowWidth,
				windowHeight,
				ERenderTargetAttachmentType::Color,
			ERenderTextureColorFlag::Float16 | ERenderTextureColorFlag::WithAlpha
		);
		
		FRenderTexturePtr normalTarget = FRenderTexture::Create(
				windowWidth,
				windowHeight,
				ERenderTargetAttachmentType::Color,
			ERenderTextureColorFlag::Float16 | ERenderTextureColorFlag::WithAlpha
		);
		
		FRenderTexturePtr albedoWithSpecTarget = FRenderTexture::Create(
				windowWidth,
				windowHeight,
				ERenderTargetAttachmentType::Color,
				ERenderTextureColorFlag::WithAlpha
		);
		
		if(!positionTarget->IsInitialized() || !normalTarget->IsInitialized() || !albedoWithSpecTarget->IsInitialized())
		{
			return false;
		}
		
		FRenderBufferPtr depthStencilTarget = FRenderBuffer::Create(
				windowWidth,
				windowHeight,
				ERenderTargetAttachmentType::DepthAndStencil
		);
		
		if(!depthStencilTarget->IsInitialized())
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
		fb.first->second->Attach(depthStencilTarget->AsShared());
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
	if(!blankTexture->IsInitialized() || !rocksFloorTexture->IsInitialized() || !wallTexture->IsInitialized() || !container->IsInitialized())
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
	
	// CENTER
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
	
	// LIGHTS
	{
		sceneObjects.push_back(NMeshUtils::ConstructCube({blankTexture}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{1.5f, 0.1f, 3.f},
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
		});
		GLights[0] = {sceneObjects.size() - 1, NColors::Red * 2.f , 0.f, 0.f, 0.032f};
	
		sceneObjects.push_back(NMeshUtils::ConstructCube({blankTexture}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{10.5f, 5.f, 0.f},
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
		});
		GLights[1] = {sceneObjects.size() - 1, NColors::White * 10.f, 0.f, 0.f, 0.032f };
		
		sceneObjects.push_back(NMeshUtils::ConstructCube({blankTexture}));
		sceneObjects[sceneObjects.size() - 1]->SetTransform({
			{1.5f, 0.1f, -3.f},
			{0.f, 0.f, 0.f},
			{0.25f, 0.25f, 0.25f}
		});
		GLights[2] = {sceneObjects.size() - 1, NColors::Blue * 2.f, 0.f, 0.f, 0.032f };
	}

	OutScene = FScene::Create(sceneObjects);
	
	return true;
}

bool PrepareShaders(TFastMap<EShaderMainType, FShaderProgramPtr>& OutShaders, TFastMap<EUniformBufferMainType, FUniformBufferPtr>& OutUniforms)
{
	// Compile and set shaders
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

bool InitRender(TFastMap<EUniformBufferMainType, FUniformBufferPtr>& Uniforms)
{
	return true;
}

bool InitUI()
{
	return true;
}

void ProcessRender(TFastMap<EShaderMainType, FShaderProgramPtr>& Shaders, TFastMap<EUniformBufferMainType, FUniformBufferPtr>& Uniforms, TFastMap<EFramebufferMainType, FFramebufferPtr>& Framebuffers)
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
	
	Uniforms[EUniformBufferMainType::PostProcess]->SetValue(0, GGamma);
	Uniforms[EUniformBufferMainType::PostProcess]->SetValue(NShaderUtils::GetSTD140Size<float>(), GExposure);
	
	// Scene
	// * To custom framebuffer
	{
		Framebuffers[EFramebufferMainType::GBuffer]->Enable();

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
				
				for(uint8 i = 0; i < 3; ++i)
				{
					const FString uniformName = "lights[" + std::to_string(i) + "]";
				
					Shaders[EShaderMainType::Mesh]->SetVec3(FString(uniformName + ".position").c_str(), GScene->GetObjectByIdx(GLights[i].BlockId)->GetTransform().Position);
					Shaders[EShaderMainType::Mesh]->SetVec3(FString(uniformName + ".diffuse").c_str(), GLights[i].Color.ToVec4() * 0.5f);
					Shaders[EShaderMainType::Mesh]->SetVec3(FString(uniformName + ".ambient").c_str(), GLights[i].Color.ToVec4() * 0.05f);
					Shaders[EShaderMainType::Mesh]->SetVec3(FString(uniformName + ".specular").c_str(), {0.05f, 0.05f, 0.05f});
					
					Shaders[EShaderMainType::Mesh]->SetFloat(FString(uniformName + ".constant").c_str(), GLights[i].Constant);
					Shaders[EShaderMainType::Mesh]->SetFloat(FString(uniformName + ".linear").c_str(), GLights[i].Linear);
					Shaders[EShaderMainType::Mesh]->SetFloat(FString(uniformName + ".quadratic").c_str(), GLights[i].Quadratic);
				}
			}
			
			GScene->Draw(Shaders[EShaderMainType::Mesh], GCamera);
			
			Shaders[EShaderMainType::Mesh]->Disable();
		}
		
		Framebuffers[EFramebufferMainType::GBuffer]->Disable();
	}
	
	// Screen rendering
	{
		// Setup
		{
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		// Draw quad
		{
			Shaders[EShaderMainType::Screen]->Enable();
			
			TArray<FTexturePtr> gTextures = NRenderTargetUtils::TryGetAsTextures(
				Framebuffers[EFramebufferMainType::GBuffer],
				ERenderTargetAttachmentType::Color,
				ETextureType::Diffuse
			);
			
			ENSURE(gTextures.size() == 3);
			
			// Setup shaders
			{
				Shaders[EShaderMainType::Screen]->SetInt32("GPosition", 0);
				Shaders[EShaderMainType::Screen]->SetInt32("GNormal", 1);
				Shaders[EShaderMainType::Screen]->SetInt32("GAlbedoSpecular", 2);
			}
			
			gTextures[0]->Use(0);
			gTextures[1]->Use(1);
			gTextures[2]->Use(2);
			
			GScreenObject->Draw(Shaders[EShaderMainType::Screen]);
			
			gTextures[2]->Clear();
			gTextures[1]->Clear();
			gTextures[0]->Clear();
			
			Shaders[EShaderMainType::Screen]->Disable();
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

				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("Settings"))
			{
				ImGui::Checkbox("Details in viewport", &GUIDetailsInViewport);
				ImGui::Checkbox("Debug visible", &GUIDebugVisible);
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
			const float currentFramerate = 60.f / GDeltaSeconds;

			static ImVec2 defaultPos(FApplication::Get().GetWindowWidth() - 255.f, 20.f);
			static ImVec2 defaultSize(255.f, 105.f);

			if(GUIResetLayout)
			{
				ImGui::SetNextWindowPos(defaultPos);
				ImGui::SetNextWindowSize(defaultSize);
			}

			ImGui::Begin("Details");
			ImGui::Text("FPS: %.1f (%.3f ms)", currentFramerate, 1000.0f / currentFramerate);
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
			if(ImGui::CollapsingHeader("Lights"))
			{
				ImGui::Indent();

				for(uint8 i = 0; i < 3; ++i)
				{
					if(ImGui::CollapsingHeader(FString("[" + std::to_string(i) + "]").c_str()))
					{
						colorPickerVisible = true;

						ImGui::Indent();

						glm::vec3 positionCopy = GScene->GetObjectByIdx(GLights[i].BlockId)->GetTransform().Position;
						glm::vec4 colorCopy = GLights[i].Color.ToVec4();
						
						if(ImGui::SliderFloat3("Position", &positionCopy.x, -25.f, 25.f))
						{
							auto obj = GScene->GetObjectByIdx(GLights[i].BlockId);
							FTransform objTransform = obj->GetTransform();
							objTransform.Position = positionCopy;
							obj->SetTransform(objTransform);
						}
						
						ImGui::SliderFloat("Constant", &GLights[i].Constant, 0.f, 10.f);
						ImGui::SliderFloat("Linear", &GLights[i].Linear, 0.f, 2.f);
						ImGui::SliderFloat("Quadratic", &GLights[i].Quadratic, 0.f, 2.f);
						
						ImGui::Spacing();
						
						if(ImGui::ColorPicker4("Color", &colorCopy.x,
							ImGuiColorEditFlags_HDR |
							ImGuiColorEditFlags_NoSidePreview |
							ImGuiColorEditFlags_AlphaBar))
						{
							GLights[i].Color = FColor::FromVec4(colorCopy);
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
	
	return true;
}

void EngineTick()
{
	// Update FPS
	{
//		FString resultTitle;
//		resultTitle.append("LearnOpenGL: FPS [");
//		resultTitle.append(std::to_string((uint16)std::floor(60.f / GDeltaSeconds)));
//		resultTitle.append("]");
//
//		glfwSetWindowTitle(GWindow, resultTitle.c_str());
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

	if(!CreateInitUI(GWindow))
	{
		return -2;
	}

	TFastMap<EShaderMainType, FShaderProgramPtr> Shaders;
	TFastMap<EUniformBufferMainType, FUniformBufferPtr> Uniforms;
	if(!PrepareShaders(Shaders, Uniforms))
	{
		return -3;
	}
	
	TFastMap<EFramebufferMainType, FFramebufferPtr> Framebuffers;
	if(!PrepareFBs(Framebuffers))
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
	
	if(!InitRender(Uniforms))
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
		GDeltaSeconds = GLastSeconds;
		GLastSeconds = (float)frameTimer.GetSeconds();

		frameTimer.Reset();
		frameTimer.Start();

		EngineTick();
		ProcessInput();
		ProcessRender(Shaders, Uniforms, Framebuffers);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ProcessUIRender();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(GWindow);
		glfwPollEvents();

		frameTimer.Stop();
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