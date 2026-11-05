#ifndef RACING_FIRE_GAME_SPAWNER_H
#define RACING_FIRE_GAME_SPAWNER_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class GameSpawner : public Node3D {
	GDCLASS(GameSpawner, Node3D)

private:
	// Scene and movement settings are exposed so they can be changed per scene.
	String car_scene_path = "res://assets/kenney_car-kit/Models/GLB format/race.glb";
	Vector3 floor_size = Vector3(40.0, 0.2, 40.0);
	float car_speed = 12.0;
	float acceleration = 30.0;
	float steering_speed = 2.5;
	float gravity = 24.0;

	CharacterBody3D *car_body = nullptr;

	void build_floor();
	void spawn_car();

protected:
	// Godot requires the underscore here!
	static void _bind_methods();

public:
	GameSpawner() = default;
	~GameSpawner() override = default;

	// THESE MUST BE PUBLIC FOR GODOT TO CALL THEM!
	void _ready() override;
	void _physics_process(double p_delta) override;

	void set_car_scene_path(const String &p_path);
	String get_car_scene_path() const;

	void set_floor_size(const Vector3 &p_size);
	Vector3 get_floor_size() const;

	void set_car_speed(float p_speed);
	float get_car_speed() const;

	void set_acceleration(float p_acceleration);
	float get_acceleration() const;

	void set_steering_speed(float p_speed);
	float get_steering_speed() const;

	void set_gravity(float p_gravity);
	float get_gravity() const;
};

} // namespace godot

#endif // RACING_FIRE_GAME_SPAWNER_H