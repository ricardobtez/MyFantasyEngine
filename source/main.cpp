#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/TransformManager.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>
#include <filament/Engine.h>
#include <filament/IndirectLight.h>
#include <filament/LightManager.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/TransformManager.h>
#include <filament/View.h>
#include <filament/Viewport.h>

#include <utils/EntityManager.h>
#include <utils/Panic.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>

#include <thread>
#include <fstream>
#include <chrono>
#include<iostream>

using namespace filament;
using namespace utils;
using namespace math;
using namespace std;

void* getNativeWindow(SDL_Window* sdlWindow) {
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	ASSERT_POSTCONDITION(SDL_GetWindowWMInfo(sdlWindow, &wmi), "SDL version unsupported!");
	HWND win = (HWND) wmi.info.win.window;
	return (void*) win;
}

bool done = false;

using namespace filament;
using utils::Entity;
using utils::EntityManager;

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

struct App {
	Engine* engine;
	SwapChain* swapChain;
	Renderer* renderer;
	View* view;
	Scene* scene;
	Camera* cam;

	VertexBuffer* vb;
	IndexBuffer* ib;
	Material* mat;
	Skybox* skybox;

	Entity camera;
	Entity renderable;
};

App app;

struct Vertex {
	filament::math::float2 position;
	uint32_t color;
};

static const Vertex TRIANGLE_VERTICES[3] = {
	{{1, 0}, 0xffff0000u},
	{{cos(M_PI * 2 / 3), sin(M_PI * 2 / 3)}, 0xff00ff00u},
	{{cos(M_PI * 4 / 3), sin(M_PI * 4 / 3)}, 0xff0000ffu},
};

static constexpr uint16_t TRIANGLE_INDICES[3] = { 0, 1, 2 };

void setup() {
	Engine* engine = app.engine;
	Scene* scene = app.scene;
	View* view = app.view;

	app.skybox = Skybox::Builder().color({ 0.1, 0.125, 0.25, 1.0 }).build(*engine);
	scene->setSkybox(app.skybox);
	view->setPostProcessingEnabled(false);

	app.vb = VertexBuffer::Builder()
		.vertexCount(3)
		.bufferCount(1)
		.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT2, 0, 12)
		.attribute(VertexAttribute::COLOR, 0, VertexBuffer::AttributeType::UBYTE4, 8, 12)
		.normalized(VertexAttribute::COLOR)
		.build(*engine);

	app.vb->setBufferAt(*engine, 0,
		VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, 36, nullptr));

	app.ib = IndexBuffer::Builder()
		.indexCount(3)
		.bufferType(IndexBuffer::IndexType::USHORT)
		.build(*engine);

	app.ib->setBuffer(*engine,
		IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, 6, nullptr));

	std::ifstream matBinary ("aiDefaultMat.inc", std::ifstream::binary);
	if (!matBinary) {
		std::cout << "Error opening material" << std::endl;
	}
	// get length of file:
	matBinary.seekg (0, matBinary.end);
	int length = matBinary.tellg();
	matBinary.seekg (0, matBinary.beg);

	// allocate memory:
	char * buffer = new char [length];

	// read data as a block:
	matBinary.read (buffer, length);

	matBinary.close();

	app.mat = Material::Builder()
		.package(buffer, length)
		.build(*engine);

	app.renderable = EntityManager::get().create();
	RenderableManager::Builder(1)
		.boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
		.material(0, app.mat->getDefaultInstance())
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, app.vb, app.ib, 0, 3)
		.culling(false)
		.receiveShadows(false)
		.castShadows(false)
		.build(*engine, app.renderable);

	scene->addEntity(app.renderable);
	app.camera = utils::EntityManager::get().create();
	app.cam = engine->createCamera(app.camera);
	view->setCamera(app.cam);
}

void cleanup() {
	app.engine->destroy(app.skybox);
	app.engine->destroy(app.renderable);
	app.engine->destroy(app.mat);
	app.engine->destroy(app.vb);
	app.engine->destroy(app.ib);
	app.engine->destroyCameraComponent(app.camera);
	utils::EntityManager::get().destroy(app.camera);
}

void mainLoop() {
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				done = false;
				break;

			case SDL_KEYDOWN: {
				switch ((int) event.key.keysym.sym) {
					case SDLK_ESCAPE: {
						done = false;
						break;
					}
				}
				break;
			}
		}
	}

	constexpr double ZOOM = 1.5f;
	const uint32_t w = app.view->getViewport().width;
	const uint32_t h = app.view->getViewport().height;
	const double aspect = (double)w / h;

	app.cam->setProjection(Camera::Projection::ORTHO,
		-aspect * ZOOM, aspect * ZOOM,
		-ZOOM, ZOOM, 0, 1
	);

	auto& tcm = app.engine->getTransformManager();
	tcm.setTransform(tcm.getInstance(app.renderable),
		filament::math::mat4f::rotation(SDL_GetTicks() / 1000.0, filament::math::float3{ 0, 0, 1 }));

	if (app.renderer->beginFrame(app.swapChain)) {
		app.renderer->render(app.view);
		app.renderer->endFrame();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("My Fantasy Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

	app.engine = Engine::create();
	app.swapChain = app.engine->createSwapChain(getNativeWindow(window));

	app.renderer = app.engine->createRenderer();
	app.scene = app.engine->createScene();
	//app.cam = app.engine->createCamera();
	app.view = app.engine->createView();

	app.view->setCamera(app.cam);
	app.view->setScene(app.scene);
	app.view->setViewport({ 0, 0, 1280, 720});

	setup();


	while (!done) {
		mainLoop();
	}

	cleanup();
	Engine::destroy(&app.engine);

	SDL_Quit();

	return 0;
}