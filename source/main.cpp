/*
	+-------------------------------------------------------------+
	|                   My Fantasy Engine v0.1                    |
	|            A pre-rendered background game engine            |
	+-------------------------------------------------------------+
*/

#include <stdio.h>

#ifdef MFE_PLATFORM_WEB
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "window/Window.h"
#include "renderer/Renderer.h"

void loop()
{
	windowLoop();
	rendererLoop();
}

int main() {

	if(!windowInit()){
		return 1;
	};
	
	if(!rendererInit(windowGetHandle(), uint32_t(windowGetWidth()), uint32_t(windowGetHeight()))) {
		windowClose();
		return 1;
	};
	
#ifdef MFE_PLATFORM_WEB
	emscripten_set_main_loop(loop, 0, 1);
#else
	while (!windowShouldClose()) {
		loop();
	}
#endif

	rendererStop();
	windowClose();

	return 0;
}
