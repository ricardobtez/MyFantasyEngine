#define USE_GL

#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "window.h"
#include "filament.h"
#include "resourceManager.h"

// Some javascript is used to set the current GL context
#ifdef __EMSCRIPTEN__
void registerGlContext()
{
	EM_ASM_INT({
		const handle = GL.registerContext(myengine.glContext, myengine.glOptions);
		GL.makeContextCurrent(handle);
	});
}
#endif


ResourceManager ResourceManagerMaster;
bool MeshLoaded = false;
void onLoadFinal(ResourceManager& rm);

void loop()
{
	windowLoop();
	filamentLoop();

	// Final step to async loading  
	if (MeshLoaded == false && ResourceManagerMaster.pending == 0)
	{
		MeshLoaded = true;
		onLoadFinal(ResourceManagerMaster);
	}
}


#ifdef __EMSCRIPTEN__
EM_BOOL em_mouse_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
	static bool down = false;
	static int x = 0;
	static int y = 0;

	if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
	{
		down = true;
		x = mouseEvent->clientX;
		y = mouseEvent->clientY;
	}

	if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
	{
		down = false;
	}

	if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
	{
		if (down)
		{
		int dx = x - mouseEvent->clientX;
		int dy = y - mouseEvent->clientY;
		x = mouseEvent->clientX;
		y = mouseEvent->clientY;
		windowMouseEvent(dx, dy);
		}
	}

	return 0;
}
#endif


int main(int argc, char* argv[]) {

	enum 
	{
		FILAMESH = 0,
		FILAMAT,
		TEXTURE_KTX,
	};
	std::function x = &onMeshLoad;
	std::function y = &onMaterialLoad;
	std::function z = &onTextureLoad;
	ResourceManagerMaster.addResourceType(FILAMESH, &x);
	ResourceManagerMaster.addResourceType(FILAMAT, &y);
	ResourceManagerMaster.addResourceType(TEXTURE_KTX, &z);

	ResourceManagerMaster.addResource("sky",				"assets/scenes/dungeon/default_env_skybox.ktx",	TEXTURE_KTX);
	ResourceManagerMaster.addResource("ibl",				"assets/scenes/dungeon/default_env_ibl.ktx",	TEXTURE_KTX);
	ResourceManagerMaster.addResource("Heart ao",			"assets/models/ao_etc.ktx",						TEXTURE_KTX);
	ResourceManagerMaster.addResource("Default material",	"assets/materials/plastic.filamat",				FILAMAT);
	ResourceManagerMaster.addResource("Heart mesh",			"assets/models/heart.filamesh",					FILAMESH);

	windowInit();
#ifdef __EMSCRIPTEN__
	registerGlContext();
#endif
	filamentInit();

#ifdef __EMSCRIPTEN__
	emscripten_set_mousedown_callback("canvas", nullptr, true, em_mouse_callback);
	emscripten_set_mouseup_callback("canvas", nullptr, true, em_mouse_callback);
	emscripten_set_mousemove_callback("canvas", nullptr, true, em_mouse_callback);
#endif

	ResourceManagerMaster.startAsyncLoad();
	
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, 1);
#else
	while (!windowShouldClose()) {
		loop();
	}
#endif

	filamentStop();
	windowClose();

  return 0;
}
