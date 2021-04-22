
#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

void WindowSizeChanged(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

void ProcessInput(GLFWwindow* Window)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);
}

void ProcessRender(GLFWwindow* Window)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
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

	// Render LOOP
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		ProcessRender(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}