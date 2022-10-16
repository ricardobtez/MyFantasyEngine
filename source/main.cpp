/*
	+-------------------------------------------------------------+
	|                   My Fantasy Engine v0.1                    |
	|            A pre-rendered background game engine            |
	+-------------------------------------------------------------+
*/

#include <stdio.h>
#include <thread>
#include <chrono>

#ifdef MFE_PLATFORM_WEB
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "window/Window.h"
#include "renderer/Renderer.h"
#include "manager/AssetManager.h"

void loop()
{
	windowLoop();
	rendererLoop();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main() {

	enum 
	{
		FILAMESH = 0,
		FILAMAT,
		TEXTURE_KTX,
	};

	auto assetManager = AssetManager();

	std::function x = &meshLoad;
	std::function y = &materialLoad;
	std::function z = &textureLoad;

	assetManager.addType(FILAMESH, &x);
	assetManager.addType(FILAMAT, &y);
	assetManager.addType(TEXTURE_KTX, &z);

	assetManager.addAsset("sky",				"assets/scenes/dungeon/default_env_skybox.ktx",	TEXTURE_KTX);
	assetManager.addAsset("ibl",				"assets/scenes/dungeon/default_env_ibl.ktx",	TEXTURE_KTX);
	assetManager.addAsset("Default material",	"assets/materials/plastic.filamat",				FILAMAT);
	assetManager.addAsset("Heart mesh",			"assets/models/heart.filamesh",					FILAMESH);

	if(!windowInit()){
		return 1;
	};
	
	if(!rendererInit(windowGetHandle(), uint32_t(windowGetWidth()), uint32_t(windowGetHeight()))) {
		windowClose();
		return 1;
	};

	assetManager.startAsyncLoad();

	while(assetManager.pending > 0);
	
	loadScene(assetManager);
	
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
