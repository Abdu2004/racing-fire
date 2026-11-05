#include "register_types.h"

#include "GameSpawner.h"

#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_racing_game_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<GameSpawner>();
}

void uninitialize_racing_game_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {

GDExtensionBool GDE_EXPORT racing_game_library_init(
		GDExtensionInterfaceGetProcAddress p_get_proc_address,
		GDExtensionClassLibraryPtr p_library,
		GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_object(
			p_get_proc_address,
			p_library,
			r_initialization);

	init_object.register_initializer(initialize_racing_game_module);
	init_object.register_terminator(uninitialize_racing_game_module);
	init_object.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_object.init();
}

} // extern "C"
