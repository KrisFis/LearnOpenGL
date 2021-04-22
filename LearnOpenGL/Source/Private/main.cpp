
#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

void WindowSizeChanged(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

bool CreateAndLinkProgram(const unsigned int& VertexShaderId, const unsigned int& FragmentShaderId, unsigned int* OutProgram = nullptr)
{
	unsigned int programIdTmp;
	programIdTmp = glCreateProgram();
	glAttachShader(programIdTmp, VertexShaderId);
	glAttachShader(programIdTmp, FragmentShaderId);
	glLinkProgram(programIdTmp);

	int success;
	glGetProgramiv(programIdTmp, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(programIdTmp, 512, nullptr, infoLog);
		std::cout << "Program link failed\n" << infoLog << std::endl;
		return false;
	}

	if (OutProgram) *OutProgram = programIdTmp;
	return true;
}

bool CompileShader(const GLenum Type, const char* Source, unsigned int* OutId = nullptr)
{
	unsigned int shaderIdTmp = glCreateShader(Type);
	glShaderSource(shaderIdTmp, 1, &Source, nullptr);
	glCompileShader(shaderIdTmp);

	int success;
	glGetShaderiv(shaderIdTmp, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderIdTmp, 512, nullptr, infoLog);
		std::cout << "Shader compilation failed\n" << infoLog << std::endl;
		return false;
	}

	std::cout << "Shader compiled successfully!" << std::endl;

	if(OutId) *OutId = shaderIdTmp;
	return true;
}

void ProcessInput(GLFWwindow* Window)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);
}

void ProcessRender(GLFWwindow* Window, const unsigned int& DefaultProgramId, const unsigned int& VertexBufferId, const unsigned int& VertexArrayId)
{
	// Clear part
	{
		// Default clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clears screen with ClearColor
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Defines which program to use
	glUseProgram(DefaultProgramId);

	// Binds vertex array
	glBindVertexArray(VertexArrayId);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int RenderMain()
{
	GLFWwindow* window = nullptr;
	unsigned int defaultProgramId, defaultVertexBufferId, defaultVertexArrayId;

	// Init
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
		if (!window)
		{
			std::cout << "Failed to create window" << std::endl;
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -2;
		}

		glViewport(0, 0, 800, 600);

		glfwSetFramebufferSizeCallback(window, &WindowSizeChanged);
	}

	// Shaders
	{
		unsigned int vertexShaderId, fragmentShaderId;

		// Vertex
		{
			const char* vertexShaderSource = "#version 330 core\n"
				"layout (location = 0) in vec3 aPos;\n"
				"void main()\n"
				"{\n"
				"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
				"}\0";

			if (!CompileShader(GL_VERTEX_SHADER, vertexShaderSource, &vertexShaderId))
			{
				glfwTerminate();
				return -3;
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

			if (!CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, &fragmentShaderId))
			{
				glfwTerminate();
				return -4;
			}
		}

		if (!CreateAndLinkProgram(vertexShaderId, fragmentShaderId, &defaultProgramId))
		{
			glfwTerminate();
			return -5;
		}

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
	}

	// Defines triangle
	{
		// Triangle in center
		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		// VBO stands for "Vertex Buffer Object"
		glGenBuffers(1, &defaultVertexBufferId);

		// VAO stands for "Vertex Array Object"
		glGenVertexArrays(1, &defaultVertexArrayId);

		// From now on do changes to default vertex array
		glBindVertexArray(defaultVertexArrayId);
		{
			// Binds buffer to ARRAY_BUFFER
			glBindBuffer(GL_ARRAY_BUFFER, defaultVertexBufferId);

			// Fills data
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Defines how data should be represented
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

			// Activates data representation
			glEnableVertexAttribArray(0);
		}
	}

	// Render LOOP
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		ProcessRender(window, defaultProgramId, defaultVertexBufferId, defaultVertexArrayId);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &defaultVertexArrayId);
	glDeleteBuffers(1, &defaultVertexBufferId);
	glDeleteProgram(defaultProgramId);

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