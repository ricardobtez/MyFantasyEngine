#include <stdio.h>
#include <GLFW/glfw3.h>

#ifdef MFE_PLATFORM_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#ifndef MFE_PLATFORM_WEB
#include <stb_image.h>
#endif

#include "Window.h"

const int Width = 1280;
const int Height = 720;
GLFWwindow* Window = nullptr;
const char* WindowTitle = "My Fantasy Engine";

int windowGetHeight() { return Height; }
int windowGetWidth() { return Width; }

int windowInit()
{
	if(!glfwInit()) {
		printf("Failed to setup glfw\n");
		return 0;
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	Window = glfwCreateWindow(Width, Height, WindowTitle, nullptr, nullptr);
	if(!Window) {
		printf("Failed to create window\n");
		glfwTerminate();
		return 0;
	}

#ifndef MFE_PLATFORM_WEB

	GLFWimage images[2];
	images[0].pixels = stbi_load("logo/icon.png", &images[0].width, &images[0].height, 0, 4);
	images[1].pixels = stbi_load("logo/icon-small.png", &images[1].width, &images[1].height, 0, 4);
	
	glfwSetWindowIcon(Window, 2, images);

	stbi_image_free(images[0].pixels);
	stbi_image_free(images[1].pixels);

	//Turn on vsync
	glfwSwapInterval(1);
#endif

	glfwMakeContextCurrent(Window);

	return 1;
}

void *windowGetHandle()
{
	void *handle = nullptr;
#ifdef MFE_PLATFORM_WIN
	handle = (HWND) glfwGetWin32Window(Window);
#endif
	
	return handle;
}


void windowClose()
{
	glfwTerminate();
}

void windowLoop()
{
	glfwPollEvents();
}

bool windowShouldClose()
{
	return glfwWindowShouldClose(Window);
}