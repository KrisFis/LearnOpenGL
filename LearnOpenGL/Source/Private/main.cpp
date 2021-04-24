
#include <iostream>
#include <vector>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

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

void ProcessDraw(FShader ShaderToUse, const FId& VertexArrayId)
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

	// Binds vertex array
	glBindVertexArray(VertexArrayId);

	// TRIANGLES
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

bool CreateAndBindTriangle(const FId& VertexArrayId, FId* OutBufferId)
{
	// Triangle in center
	float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
	};

	FId resultVertexBufferId;
	glGenBuffers(1, &resultVertexBufferId);

	// From now on do changes to vertex array
	glBindVertexArray(VertexArrayId);

	// Within VERTEX ARRAY
	//////////////////////////////////////////////////////////////////////////

	// Binds buffer to ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, resultVertexBufferId);

	// Fills data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Defines how POSITION should be represented
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// Defines how COLOR should be represented
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6* sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	//////////////////////////////////////////////////////////////////////////

	// Unbind vertex array
	glBindVertexArray(0);

	if (OutBufferId) *OutBufferId = resultVertexBufferId;
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

int RenderMain()
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

 	if (!CreateAndBindTriangle(vertexArrayId, nullptr /* WE DO NOT CHANGE BUFFERS NOW */))
 	{
 		glfwTerminate();
 		return -3;
 	}

 	FShader testShader("TestShader.vs", "TestShader.fs");
 	if(!testShader.IsInitialized())
	{
		glfwTerminate();
		return -4;
	}

	// Main render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		ProcessInput(mainWindow);
		ProcessDraw(testShader, vertexArrayId);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

int main()
{
	int errorCode = RenderMain();
	if (errorCode)
	{
		// Give time to look
		std::cout << "Error occurred [" << errorCode << "]" << std::endl;
		system("pause");
	}

	return errorCode;
}