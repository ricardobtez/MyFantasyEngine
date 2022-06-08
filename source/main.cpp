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

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

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
	MaterialInstance* matInstance;
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
	static_assert(sizeof(Vertex) == 12, "Strange vertex size.");

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

	delete buffer;

	app.matInstance = app.mat->createInstance();

	app.matInstance->setParameter("baseColor", filament::RgbType::sRGB, {0.8, 0.0, 0.0});
	app.matInstance->setParameter("roughness", 0.2f);
	app.matInstance->setParameter("metallic", 1.0f);

	app.renderable = EntityManager::get().create();
	RenderableManager::Builder(1)
		.boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
		.material(0, app.matInstance)
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
	glfwPollEvents();

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
		filament::math::mat4f::rotation(glfwGetTime(), filament::math::float3{ 0, 0, 1 }));

	if (app.renderer->beginFrame(app.swapChain)) {
		app.renderer->render(app.view);
		app.renderer->endFrame();
	}

#ifdef __EMSCRIPTEN__
	app.engine->execute()
#else
	std::this_thread::sleep_for(std::chrono::milliseconds(8));
#endif

}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	int win_w = 1280;
	int win_h = 720;

	GLFWwindow* window = glfwCreateWindow(win_w, win_h, "My Fantasy Engine", 0, 0);
	glfwMakeContextCurrent(window);

	app.engine = Engine::create();

#ifdef __EMSCRIPTEN__
	app.swapChain = app.engine->createSwapChain(nullptr);
#else
	HWND winHandle = glfwGetWin32Window(window);
	app.swapChain = app.engine->createSwapChain(winHandle);
#endif

	app.renderer = app.engine->createRenderer();
	app.scene = app.engine->createScene();
	app.view = app.engine->createView();

	app.view->setCamera(app.cam);
	app.view->setScene(app.scene);
	app.view->setViewport({ 0, 0, uint32_t(win_w), uint32_t(win_h) });

	/*glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);
	glfwSetFramebufferSizeCallback(window, resizeCallback);*/

	setup();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(mainLoop, 0, true);
#else
	while (!glfwWindowShouldClose(window)) {
		mainLoop();
	}
#endif

	cleanup();
	Engine::destroy(&app.engine);

	glfwTerminate();

	return 0;
}