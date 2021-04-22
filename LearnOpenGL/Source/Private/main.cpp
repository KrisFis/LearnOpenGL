
#include <iostream>
#include <vector>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

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

void ProcessDraw(GLFWwindow* Window, const FId& ProgramId, const FId& VertexArrayId)
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Defines which program to use
	glUseProgram(ProgramId);

	// Binds vertex array
	glBindVertexArray(VertexArrayId);

	// TRIANGLES
	// glDrawArrays(GL_TRIANGLES, 0, 3);

	// ELEMENTS
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool CreateAndBindTriangle(const FId& VertexArrayId, FId* OutBufferId)
{
	// Triangle in center
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
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

	// Defines how data should be represented
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// Activates data representation
	glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////////////////////////////

	// Unbind vertex array
	glBindVertexArray(0);

	if (OutBufferId) *OutBufferId = resultVertexBufferId;
	return true;
}

bool CreateAndBindSquare(const FId& VertexArrayId, std::vector<FId>* OutBuffersIds)
{
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	FId resultElementsBufferId, resultVertexBufferId;

	glGenBuffers(1, &resultElementsBufferId);
	glGenBuffers(1, &resultVertexBufferId);

	glBindVertexArray(VertexArrayId);

	// Start of VERTEX ARRAY
	//////////////////////////////////////////////////////////////////////////

	glBindBuffer(GL_ARRAY_BUFFER, resultVertexBufferId);

	// Start of VERTEX BUFFER
	//////////////////////////////////////////////////////////////////////////

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resultElementsBufferId);

	// Start of ELEMENT BUFFER
	//////////////////////////////////////////////////////////////////////////

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	// End of ELEMENT BUFFER

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // DONT UNBOUND !

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////////////////////////////
	// End of VERTEX BUFFER

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//////////////////////////////////////////////////////////////////////////
	// End of VERTEX ARRAY

	glBindVertexArray(0);

	return true;
}

bool CreateMainVertexArray(FId* OutArrayId)
{
	FId resultArrayId;
	glGenVertexArrays(1, &resultArrayId);

	if (OutArrayId) *OutArrayId = resultArrayId;
	return true;
}

bool CreateMainWindow(GLFWwindow** OutWindow)
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

	glViewport(0, 0, 800, 600);

	if (OutWindow) *OutWindow = resultWindow;
	return true;
}

bool CreateMainProgram(FId* OutProgramId)
{
	FId vertexShaderId, fragmentShaderId;

	// Vertex
	{
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";

		if (!CompileShader(GL_VERTEX_SHADER, "VERTEX", vertexShaderSource, &vertexShaderId))
		{
			return false;
		}
	}

	// Fragment
	{
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\0";

		if (!CompileShader(GL_FRAGMENT_SHADER, "FRAGMENT", fragmentShaderSource, &fragmentShaderId))
		{
			return false;
		}
	}

	FId resultProgramId;
	if (!CreateAndLinkProgram(vertexShaderId, fragmentShaderId, &resultProgramId))
	{
		return false;
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	if(OutProgramId) *OutProgramId = resultProgramId;
	return true;
}

int RenderMain()
{
	GLFWwindow* mainWindow;
	if (!CreateMainWindow(&mainWindow))
	{
		glfwTerminate();
		return -1;
	}

	FId mainProgramId;
	if (!CreateMainProgram(&mainProgramId))
	{
		glfwTerminate();
		return -2;
	}

	FId mainVertexArrayId;
	if (!CreateMainVertexArray(&mainVertexArrayId))
	{
		glfwTerminate();
		return -3;
	}

// 	if (!CreateAndBindTriangle(mainVertexArrayId, nullptr /* WE DO NOT CHANGE BUFFERS NOW */))
// 	{
// 		glfwTerminate();
// 		return -4;
// 	}

	if (!CreateAndBindSquare(mainVertexArrayId, nullptr /* WE DO NOT CHANGE BUFFERS NOW */))
	{
		glfwTerminate();
		return -4;
	}

	// Main render loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		ProcessInput(mainWindow);
		ProcessDraw(mainWindow, mainProgramId, mainVertexArrayId);

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	// Not necessary
// 	glDeleteVertexArrays(1, &mainVertexArrayId);
// 	glDeleteBuffers(1, &triangleVertexBuffleId);
// 	glDeleteProgram(mainProgramId);
// 	glfwDestroyWindow(mainWindow);

	glfwTerminate();

	return 0;
}

int main(void)
{
	int errorCode = RenderMain();
	if (errorCode)
	{
		// Give time to look
		std::cout << "Program exited with Error [" << errorCode << "]" << std::endl;
		system("pause");
	}

	return errorCode;
}