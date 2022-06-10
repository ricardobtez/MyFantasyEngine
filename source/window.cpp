#include <stdio.h>
#include <thread>
#include <chrono>
#include <GLFW/glfw3.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "window.h"

const int Width = 1280;
const int Height = 720;
GLFWwindow* Window = nullptr;
const char* WindowTitle = "My Fantasy Engine";

int windowGetHeight() { return Height; }
int windowGetWidth() { return Width; }

void windowInit()
{
	if(!glfwInit()) {
		printf("Failed to setup glfw\n");
		return;
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#if !USE_GL
	printf("Not using a GL context\n");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
	
	Window = glfwCreateWindow(Width, Height, WindowTitle, nullptr, nullptr);
	if(!Window) {
		printf("Failed to create window\n");
		glfwTerminate();
		return;
	}

#ifdef WIN32
	// Turn on vertical screen sync under Windows.
	// (I.e. it uses the WGL_EXT_swap_control extension)
	//typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
	//PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
	//wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	//if(wglSwapIntervalEXT)
	//	wglSwapIntervalEXT(1);
#endif

	glfwMakeContextCurrent(Window);
}

void *windowGetHandle()
{
	void *handle = nullptr;
#ifdef WIN32
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
	std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

bool windowShouldClose()
{
	return glfwWindowShouldClose(Window);
}


void windowMouseEvent(int x, int y)
{
static float s_a = 0;
static float s_b = 0;
s_a +=  0.002f * (float)x;
s_b += -0.002f * (float)y;
if (s_b > 1.0f) s_b = 1.0f;
if (s_b < -1.0f) s_b = -1.0f;
//set_camera_angle(s_a, s_b);
}