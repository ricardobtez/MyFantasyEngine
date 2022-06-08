#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>
#include <filament/Camera.h>
#include <filament/Viewport.h>
#include <math/TVecHelpers.h>
#include <filament/TransformManager.h>

#include <utils/Panic.h>
#include <utils/Entity.h>
#include <utils/EntityManager.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>

using namespace filament;
using namespace utils;

void* getNativeWindow(SDL_Window* sdlWindow) {
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	ASSERT_POSTCONDITION(SDL_GetWindowWMInfo(sdlWindow, &wmi), "SDL version unsupported!");
	HWND win = (HWND) wmi.info.win.window;
	return (void*) win;
}

int main() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("My Fantasy Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	void* nativeWindow = getNativeWindow(window);

	// Create the Engine after the window in case this happens to be a single-threaded platform.
	// For single-threaded platforms, we need to ensure that Filament's OpenGL context is
	// current, rather than the one created by SDL.
	Engine* engine = Engine::create();

	SwapChain* swapChain = engine->createSwapChain(nativeWindow);

	Renderer* renderer = engine->createRenderer();

	SDL_GL_SetSwapInterval(1);

	Camera* camera = engine->createCamera(EntityManager::get().create());
	View* view = engine->createView();
	Scene* scene = engine->createScene();

	view->setScene(scene);
	view->setCamera(camera);
	view->setViewport({ 0, 0, uint32_t(1280), uint32_t(720) });

	Entity renderable = EntityManager::get().create();

	float vertices[] = {
			0.0f,1.0f,
			1.0f,-1.0f,
			-1.0f,-1.0f
	};

	uint16_t indices[] = {
			0,1,2
	};

	VertexBuffer* vbo =
			VertexBuffer::Builder()
					.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT2)
					.bufferCount(1)
					.vertexCount(3)
					.build(*engine);
	vbo->setBufferAt(*engine, 0, VertexBuffer::BufferDescriptor(vertices, sizeof(vertices)));

	IndexBuffer* ibo =
			IndexBuffer::Builder()
					.bufferType(IndexBuffer::IndexType::USHORT)
					.indexCount(3)
					.build(*engine);
	ibo->setBuffer(*engine, IndexBuffer::BufferDescriptor(indices, sizeof(indices)));


	RenderableManager::Builder(1)
			.boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
			.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vbo, ibo, 0, 3)
			.culling(false)
			.build(*engine, renderable);


	scene->addEntity(renderable);

	bool done = false;
	while (!done) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				default:
					break;
				case SDL_QUIT:
					done = true;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE: {
							SDL_Event user_event;
							user_event.type = SDL_QUIT;
							SDL_PushEvent(&user_event);
							break;
						}
					}
					break;
			}
		}
		if (renderer->beginFrame(swapChain)) {
			renderer->render(view);
			renderer->endFrame();
		}
		SDL_Delay(16);
	}

	SDL_Quit();
	return 0;
}