#pragma once

#include <map>
#include <string>
#include <functional>

class Mesh;
void inspect(Mesh* mesh);

// Async resource file loader
class ResourceManager 
{
public:
	typedef unsigned int RESOURCE_TYPE;
	typedef std::function<void* (void*, unsigned int)>  callback_func;
	
	ResourceManager();
	~ResourceManager();

	void addResourceType(RESOURCE_TYPE type_id, callback_func* callback);
	void addResource(const char* resource_name, const char* file_path, RESOURCE_TYPE resource_type);

	void startAsyncLoad();

	bool getResource(const char* resource_name, unsigned int& resource_handle);
	//bool getResource(const char* resource_name, void*& resource_handle);
	bool getResource(const char* resource_name, void** resource_handle);
	void* getResource(const char* resource_name);
	
	//void unloadResource(..);
	std::map<RESOURCE_TYPE, callback_func*> callbacks;
	//std::map<RESOURCE_TYPE, void*> callbacks;

	static int pending;

protected:

	struct Resource
	{
		std::string sourcePath;
		//std::string name;
		RESOURCE_TYPE resourceType;
		int loadingStatus = 0;
		void* handle = nullptr;
		//ResourceManager* rm;
		callback_func* f = nullptr;
	};

	std::map<std::string, Resource> resources;

	static void onLoad(void* arg, void* buffer, int size);
	static void onError(void* arg);

};
