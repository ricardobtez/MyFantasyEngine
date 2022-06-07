#include <nlohmann/json.hpp>
#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <SDL.h>
#include <stdio.h>


using namespace filament;


int main(int argc, char** argv)
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	} else {
		printf("we good to go\n");
	}

	Engine* engine = Engine::create();
	engine->destroy(&engine);

	nlohmann::json data;

	return 0;
}