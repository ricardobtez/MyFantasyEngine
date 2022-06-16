#include <stdio.h>
#include <fstream>

#ifdef MFE_PLATFORM_WEB
#include <emscripten.h>
#endif

#include "AssetManager.h"

int AssetManager::pending = 0;

void AssetManager::onLoaded(unsigned int handle, void* userData, void* buffer, unsigned int size)
{
	--pending;
	Asset* r = (Asset*)userData;
	printf("%s loaded\n", r->sourcePath.c_str());
	r->handle = (*r->f)(buffer, size);
}


void AssetManager::onFailed(unsigned int handle, void* userData, int errorCode, const char* statusDescription)
{
	--pending;
	Asset* asset = (Asset*)userData;

	printf("%s could not be loaded: %s\n", asset->sourcePath.c_str(), statusDescription);
}


AssetManager::AssetManager()
{

}


AssetManager::~AssetManager()
{

}


void AssetManager::addType(type type_id, callbackFunc* callback)
{
	printf("Adding asset type %i\n", type_id);
	printf("Callback address %i\n", (int)callback);
	callbacks.insert(std::pair<type, callbackFunc*>(type_id, callback));
}


void AssetManager::addAsset(const std::string& assetName, const std::string& path, type assetType)
{
	// Check if exitsts
	if (assets.count(assetName) > 0) {
		printf("An asset with that name already exists: %s\n", assetName.c_str());
		return;
	}

	// Create an empty, unloaded asset
	AssetManager::Asset assetNew;
	assetNew.assetType = assetType;
	assetNew.sourcePath = path;
	assetNew.f = callbacks[assetType]; // !
	assets[assetName] = assetNew;
}


void AssetManager::startAsyncLoad()
{
	printf("Begin async asset loading\n");

	for (const auto& i: assets) {
		++pending;

		Asset* asset = &assets[i.first];

#ifdef MFE_PLATFORM_WEB
		// Set up the async load for each resource
		emscripten_async_wget2_data(asset->sourcePath.c_str(), "GET", nullptr, (void**) asset, true,  AssetManager::onLoaded,  AssetManager::onFailed, nullptr);
		//emscripten_async_wget_data(
		//	r->sourcePath.c_str(),
		//	(void**)r,
		//	&onLoad,
		//	&onError
		//);
#else
		std::ifstream file(asset->sourcePath, std::ios::binary);

		if (!file){
			onFailed(0, (void**) asset, 0, "failed to open file");
			file.close();
			return;
		}

		//Get length of file:
		file.seekg(0, file.end);
		int size =file.tellg();
		file.seekg(0, file.beg);

		//Allocate memory:
		char* buffer = new char[size];
		file.read(buffer, size);

		onLoaded(0, asset, buffer, size);

		file.close();
#endif

	}
	printf("End async asset loading\n");
}
