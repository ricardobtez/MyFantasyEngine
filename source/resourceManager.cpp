#include <stdio.h>
#include <fstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "resourceManager.h"

int ResourceManager::pending = 0;


void ResourceManager::onLoad(void* arg, void* buffer, int size)
{
	--pending;
	Resource* r = (Resource*)arg;
	printf("Resource type %i loaded\n", r->resourceType);
	r->loadingStatus = 50;
	r->handle = (*r->f)(buffer, size);
	r->loadingStatus = 100;
}


void ResourceManager::onError(void* arg)
{
	--pending;
	Resource* r = (Resource*)arg;

	printf("Resource %s could not be loaded\n", r->sourcePath.c_str());
}


ResourceManager::ResourceManager()
{

}


ResourceManager::~ResourceManager()
{

}


void ResourceManager::addResourceType(RESOURCE_TYPE type_id, callback_func* callback)
{
	printf("Adding resource type %i\n", type_id);
	printf("Callback address %i\n", (int)callback);
	//callbacks[type_id] = callback;
	callbacks.insert(std::pair<RESOURCE_TYPE, callback_func*>(type_id, callback));
}


void ResourceManager::addResource(const char* resource_name, const char* file_path, RESOURCE_TYPE resource_type)
{
	// Check if exitsts
	if (resources.count(resource_name) > 0) {
		printf("A resource with that name already exists: %s\n", resource_name);
		return;
	}

	// Create an empty, unloaded resource
	ResourceManager::Resource resourceNew;
	resourceNew.resourceType = resource_type;
	resourceNew.sourcePath = file_path;
	resourceNew.f = callbacks[resource_type]; // !
	resources[resource_name] = resourceNew;
}


void ResourceManager::startAsyncLoad()
{
	printf("Begin async resource loading\n");

	for (const auto& i: resources) {
		++pending;

		//const Resource& r = i.second;
		Resource* r = &resources[i.first];

#ifdef __EMSCRIPTEN__
		// Set up the async load for each resource
		emscripten_async_wget_data(
			r->source_path.c_str(),
			(void**)r,
			&onLoad,
			&onError
		);
#else
		std::ifstream asset (r->sourcePath, std::ifstream::binary);
		if (!asset) {
			onError(r);
		}
		// get length of file:
		asset.seekg(0, asset.end);
		int size = asset.tellg();
		asset.seekg(0, asset.beg);

		// allocate memory:
		char* buffer = new char[size];

		asset.read(buffer, size);

		onLoad(r, buffer, size);

		asset.close();
#endif

	}
	printf("End async resource loading\n");
}


bool ResourceManager::getResource(const char* resource_name, unsigned int& resource_handle)
{
	// //printf("getting \"%s\" (%i)\n", resource_name, resources[resource_name].loading_status);
	// auto i = resources.find(resource_name);
	// if (i == resources.end()) return false;

	// resource_handle = i->second.handle;

	// return true;
	return false;
}


// bool ResourceManager::getResource(const char* resource_name, void*& resource_handle)
// {
	// resource_handle = (void*)resources[resource_name].handle;

	// return false;
// }


bool ResourceManager::getResource(const char* resource_name, void** resourceHandle)
{
	// //printf("getting \"%s\" (%i)\n", resource_name, resources[resource_name].loading_status);
	// auto i = resources.find(resource_name);
	// if (i == resources.end()) return false;

	// if (i->second.loading_status == 100)
	//   *resource_handle = (void*)i->second.handle;

	// return true;
	return false;
}


void* ResourceManager::getResource(const char* resourceName)
{
	auto i = resources.find(resourceName);
	if (i == resources.end()) return nullptr;

	if (i->second.loadingStatus == 100)
		return (void*)i->second.handle;

	return (void*)1;
}
