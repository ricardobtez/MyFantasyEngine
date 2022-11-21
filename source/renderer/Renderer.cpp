#include <stdio.h>

#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Material.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/Skybox.h>
#include <filament/Viewport.h>
#include <filament/SwapChain.h>
#include <filament/Camera.h>
#include <filament/TransformManager.h>
#include <filament/LightManager.h>
#include <filament/TextureSampler.h>
#include <filament/IndirectLight.h>

#include <math/mat3.h>
#include <math/mat4.h>

#include <utils/EntityManager.h>
#include <utils/Path.h>
#include <utils/NameComponentManager.h>

#include <image/Ktx1Bundle.h>
#include <ktxreader/Ktx1Reader.h>
#include <filameshio/MeshReader.h>

#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>
#include <gltfio/FilamentAsset.h>
#include <gltfio/AssetLoader.h>
#include <gltfio/FilamentAsset.h>

#include "Renderer.h"
#include "input/BackgroundLoader.h"

filament::Engine* Engine;
filament::SwapChain* SwapChain;
filament::View* View;
filament::Renderer* Renderer;
filament::Camera* Camera;
filament::Scene* Scene;
utils::Entity CameraEntity;

filament::gltfio::AssetLoader* AssetLoader;
utils::NameComponentManager* Names;
filament::gltfio::MaterialProvider* Materials;

filament::gltfio::ResourceLoader* resourceLoader = nullptr;
filament::gltfio::TextureProvider* stbDecoder = nullptr;
filament::gltfio::TextureProvider* ktxDecoder = nullptr;

void loadResources( const std::string& path, filament::gltfio::FilamentAsset* asset) {
	// Load external textures and buffers.
	filament::gltfio::ResourceConfiguration configuration = {};
	configuration.engine = Engine;
	configuration.gltfPath = path.c_str();
	configuration.normalizeSkinningWeights = true;

	if (!resourceLoader) {
		resourceLoader = new filament::gltfio::ResourceLoader(configuration);
		stbDecoder = filament::gltfio::createStbProvider(Engine);
		ktxDecoder = filament::gltfio::createKtx2Provider(Engine);
		resourceLoader->addTextureProvider("image/png", stbDecoder);
		resourceLoader->addTextureProvider("image/jpeg", stbDecoder);
		resourceLoader->addTextureProvider("image/ktx2", ktxDecoder);
	}
	resourceLoader->loadResources(asset);
	asset->releaseSourceData();

};

void loadScene(AssetManager& assetManager)
{
	// Material instance
	// filament::Material* material = (filament::Material*) assetManager.getAsset("Default material"); 
	// filament::MaterialInstance* materialInst = material->createInstance();

	// materialInst->setParameter("baseColor", filament::RgbType::sRGB, {0.8, 0.0, 0.0});
	// materialInst->setParameter("roughness", 0.2f);
	// materialInst->setParameter("clearCoat", 1.0f);
	// materialInst->setParameter("clearCoatRoughness", 0.3f);

	// // Textures
	// filament::TextureSampler sampler(
	// 				filament::TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
	// 				filament::TextureSampler::MagFilter::LINEAR
	// 				);

	// // Mesh
	// const void* meshBuffer = (void*) assetManager.getAsset("Heart mesh");

	// filamesh::MeshReader::MaterialRegistry materialRegistry;
	// materialRegistry.registerMaterialInstance(utils::CString("DefaultMaterial"), materialInst);

	// filamesh::MeshReader::Mesh mesh;
	// mesh = filamesh::MeshReader::loadMeshFromBuffer(
	// 	Engine,
	// 	meshBuffer,
	// 	[](void* buffer, size_t size, void* user) {},
	// 	nullptr,
	// 	materialRegistry
	// 	);

	// Scene->addEntity(mesh.renderable);

	auto mesh = (filament::gltfio::FilamentAsset*) assetManager.getAsset("Lantern mesh");

	loadResources(".", mesh);

	Scene->addEntities(mesh->getRenderableEntities(), mesh->getRenderableEntityCount());

	// Sky / IBL
	filament::Texture* skytex = (filament::Texture*) assetManager.getAsset("sky"); 
	filament::Texture* ibltex = (filament::Texture*) assetManager.getAsset("ibl"); 

	filament::IndirectLight * ibl = filament::IndirectLight::Builder()
			.reflections(ibltex)
			.build(*Engine);

	Scene->setIndirectLight(ibl);
	ibl->setIntensity(21);
	
	filament::Skybox* sky = filament::Skybox::Builder().environment(skytex).build(*Engine);
	Scene->setSkybox(sky);

}

void setCameraAngle(float a, float b)
{
  float r = 8.f;
  float x = r * sinf(a) * cosf(b);
  float y = r * sinf(b);
  float z = r * cosf(a) * cosf(b);
  Camera->lookAt({x, y, z}, {0, 0, 0}, {0, 1, 0});
}


int rendererInit(void* windowHandle, const uint32_t windowWidth, const uint32_t windowHeight)
{
	// Create engine (Set current GL context beforehand)
	Engine = filament::Engine::create();
	if (Engine == nullptr) 
	{
		printf("Failled to create the filament engine\n");
		return 0;
	}

	SwapChain = Engine->createSwapChain(windowHandle);

	if (SwapChain == nullptr) 
	{
		printf("Failed to create the SwapChain\n");
		return 0;
	}

	Renderer = Engine->createRenderer();
	Scene = Engine->createScene();
	View = Engine->createView();
	CameraEntity = utils::EntityManager::get().create();
	Camera = Engine->createCamera(CameraEntity);

	Names = new utils::NameComponentManager(utils::EntityManager::get());
	Materials = filament::gltfio::createJitShaderProvider(Engine);
	AssetLoader = filament::gltfio::AssetLoader::create({Engine, Materials});

	Camera->setExposure(16.0f, 1 / 125.0f, 100.0f);
	Camera->setExposure(100.0f);
	Camera->setProjection(45.0f, float(windowWidth)/windowHeight, 0.1f, 100.0f);
	setCameraAngle(0.f, 0.f);

	View->setViewport({0, 0, windowWidth, windowHeight});
	View->setScene(Scene);
	View->setCamera(Camera); 
	View->setName("main view");

	Renderer->setClearOptions({
		.clearColor = { 0.0f, 0.13f, 0.0f, 1.0f },
		.clear = true
	});



	return 1;
}


void rendererStop()
{
	Engine->destroyCameraComponent(CameraEntity);
	utils::EntityManager::get().destroy(CameraEntity);
	Engine->destroy(View);
	Engine->destroy(Scene);	
	Engine->destroy(Renderer);
	Engine->destroy(SwapChain);
	filament::Engine::destroy(&Engine);
}


void rendererLoop()
{
	if (Renderer->beginFrame(SwapChain)) {
		Renderer->render(View);
		Renderer->endFrame();
	}

#ifdef MFE_PLATFORM_WEB
	Engine->execute();
#endif

}


void* meshLoad(void* buffer, unsigned int size)
{
	filament::gltfio::FilamentAsset* mesh;
	mesh = AssetLoader->createAsset((uint8_t*) buffer, size);

	return (void*) mesh;
}


void* materialLoad(void* buffer, unsigned int size)
{
	filament::Material* material = filament::Material::Builder()
		.package(buffer, size)
		.build(*Engine);

	return (void*) material;
}


void* textureLoad(void* buffer, unsigned int size)
{
	image::Ktx1Bundle* bundle = new image::Ktx1Bundle( (const uint8_t*)buffer, size);

	filament::Texture* texture = ktxreader:: Ktx1Reader::createTexture(Engine, bundle, !bundle->isCubemap());
	
	return (void*) texture;
}

void* backgroundLoad(void* buffer, unsigned int size)
{
	std::vector<uint8_t> pixels;
	std::vector<uint8_t> icc_profile;
	size_t width = 0, height = 0;
	decodeJpegXlOneShot((uint8_t*) buffer, size, &pixels, &width, &height, &icc_profile))

	filament::Texture* texture = filament::Texture::Builder()
		.width(width)
		.height(height)
		.levels(1)
		.format(filament::Texture::InternalFormat::RGB8)
		.sampler(filament::Texture::Sampler::SAMPLER_2D)
		.build(*Engine);
	filament::Texture::PixelBufferDescriptor textureBuffer(pixels.data(), 4, filament::Texture::Format::RGB, filament::Texture::Type::UBYTE);
	texture->setImage(*Engine, 0, std::move(textureBuffer));
	
	return (void*) texture;
}