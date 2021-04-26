
#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#include "Shader.h"

// typedefs
typedef unsigned int FId;

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
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);

	if (glfwGetKey(Window, GLFW_KEY_F1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(Window, GLFW_KEY_F2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void BindEvents(GLFWwindow* Window)
{
	glfwSetFramebufferSizeCallback(Window, &WindowSizeChanged);
}

void ProcessDraw(FShader ShaderToUse, const FId& VertexArrayId, const FId TexturesIds[2])
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Defines which program to use
	ShaderToUse.Apply();

	glActiveTexture(GL_TEXTURE0);
 	glBindTexture(GL_TEXTURE_2D, TexturesIds[0]);

 	glActiveTexture(GL_TEXTURE1);
 	glBindTexture(GL_TEXTURE_2D, TexturesIds[1]);

	// Binds vertex array
	glBindVertexArray(VertexArrayId);

	// TRIANGLES
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool CreateAndBindTexture(const char* TextureFilePath, FId* OutTextureId)
{
	FId resultTextureId;
	glGenTextures(1, &resultTextureId);
	glBindTexture(GL_TEXTURE_2D, resultTextureId);
	
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(TextureFilePath, &width, &height, &nrChannels, 0);
	if(data)
	{
		if(data[0] == '\0')
		{
			std::cout << "Texture load failed [" << TextureFilePath << "] [EMPTY]" << std::endl;
		}
		else if(nrChannels < 3 || nrChannels > 4)
		{
			std::cout << "Texture load failed [" << TextureFilePath << "] [NOT_SUPPORTED_CHANNELS]" << std::endl;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, (nrChannels == STBI_rgb_alpha) ? GL_RGBA : GL_RGB, width, height, 0, (nrChannels == STBI_rgb_alpha) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);

			std::cout << "Texture load successful [" << TextureFilePath << "]" << std::endl;

			if(OutTextureId) *OutTextureId = resultTextureId;
			return true;
		}
	}
	else
	{
		std::cout << "Texture load failed [" << TextureFilePath << "] [NOT_FOUND]" << std::endl;
	}

	return false;
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
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

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

 	FId texturesIds[2];
 	if( !CreateAndBindTexture("../../Content/Textures/container.jpg", &texturesIds[0]) ||
 		!CreateAndBindTexture("../../Content/Textures/awesomeface.png", &texturesIds[1]))
	{
		glfwTerminate();
		return -4;
	}

 	FShader testShader("TestShader.vert", "TestShader.frag");
 	if(!testShader.IsInitialized())
	{
		glfwTerminate();
		return -4;
	}

	// Texture samplers
	testShader.SetInt("texture1", 0);
	testShader.SetInt("texture2", 1);

	// Main render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		ProcessInput(mainWindow);
		ProcessDraw(testShader, vertexArrayId, texturesIds);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}