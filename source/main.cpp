#include <nlohmann/json.hpp>

#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <iostream>


using namespace filament;


int main(int argc, char** argv)
{

	Engine* engine = Engine::create();
	engine->destroy(&engine);

	nlohmann::json data;
	data["message"] = "This is a message";
	data["message2"] = "This is another message";

	std::cout << data["message"] << std::endl;
	std::cout << data["message2"] << std::endl;
	//std::cin.get();
	return 0;
}