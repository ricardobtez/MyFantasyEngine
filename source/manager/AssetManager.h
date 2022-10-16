#pragma once

#include <map>
#include <string>
#include <functional>

class Mesh;
void inspect(Mesh* mesh);

// Async resource file loader
class AssetManager 
{
public:
	typedef unsigned int type;
	typedef std::function<void* (void*, unsigned int)>  callbackFunc;
	
	AssetManager();
	~AssetManager();

	void addType(type type_id, callbackFunc* callback);
	void addAsset(const std::string& assetName, const std::string& path, type assetType);

	void startAsyncLoad();

	bool getAsset(const std::string& assetName, unsigned int& handle);
	bool getAsset(const std::string& assetName, void** handle);
	void* getAsset(const std::string& assetName);
	
	//void unloadResource(..);
	std::map<type, callbackFunc*> callbacks;

	static int pending;

protected:

	struct Asset
	{
		bool loaded = false;
		std::string sourcePath;
		//std::string name;
		type assetType;
		void* handle = nullptr;
		//AssetManager* rm;
		callbackFunc* f = nullptr;
	};

	std::map<std::string, Asset> assets;

	static void onLoaded(unsigned int handle, void* userData, void* buffer, unsigned int size);
	static void onFailed(unsigned int handle, void* userData, int errorCode, const char *statusDescription);

};
