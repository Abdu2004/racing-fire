#include "GameSpawner.h"

#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/object.hpp>

using namespace godot;

// Notice the underscore added here!
void GameSpawner::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_car_scene_path", "path"), &GameSpawner::set_car_scene_path);
	ClassDB::bind_method(D_METHOD("get_car_scene_path"), &GameSpawner::get_car_scene_path);
	ClassDB::bind_method(D_METHOD("set_floor_size", "size"), &GameSpawner::set_floor_size);
	ClassDB::bind_method(D_METHOD("get_floor_size"), &GameSpawner::get_floor_size);
	ClassDB::bind_method(D_METHOD("set_car_speed", "speed"), &GameSpawner::set_car_speed);
	ClassDB::bind_method(D_METHOD("get_car_speed"), &GameSpawner::get_car_speed);
	ClassDB::bind_method(D_METHOD("set_acceleration", "acceleration"), &GameSpawner::set_acceleration);
	ClassDB::bind_method(D_METHOD("get_acceleration"), &GameSpawner::get_acceleration);
	ClassDB::bind_method(D_METHOD("set_steering_speed", "speed"), &GameSpawner::set_steering_speed);
	ClassDB::bind_method(D_METHOD("get_steering_speed"), &GameSpawner::get_steering_speed);
	ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &GameSpawner::set_gravity);
	ClassDB::bind_method(D_METHOD("get_gravity"), &GameSpawner::get_gravity);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "car_scene_path", PROPERTY_HINT_FILE, "*.glb,*.tscn"), "set_car_scene_path", "get_car_scene_path");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "floor_size"), "set_floor_size", "get_floor_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "car_speed", PROPERTY_HINT_RANGE, "0.0,100.0,0.1"), "set_car_speed", "get_car_speed");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "acceleration", PROPERTY_HINT_RANGE, "0.0,100.0,0.1"), "set_acceleration", "get_acceleration");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "steering_speed", PROPERTY_HINT_RANGE, "0.0,10.0,0.1"), "set_steering_speed", "get_steering_speed");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity", PROPERTY_HINT_RANGE, "0.0,100.0,0.1"), "set_gravity", "get_gravity");
}

void GameSpawner::_ready() {
	build_floor();
	spawn_car();
}

void GameSpawner::build_floor() {
	Ref<BoxMesh> mesh;
	mesh.instantiate();
	mesh->set_size(floor_size);

	MeshInstance3D *floor_mesh = memnew(MeshInstance3D);
	floor_mesh->set_mesh(mesh);

	Ref<BoxShape3D> shape;
	shape.instantiate();
	shape->set_size(floor_size);

	CollisionShape3D *collision_shape = memnew(CollisionShape3D);
	collision_shape->set_shape(shape);

	StaticBody3D *floor_body = memnew(StaticBody3D);
	floor_body->set_name("RaceFloor");
	floor_body->add_child(floor_mesh);
	floor_body->add_child(collision_shape);
	floor_body->set_position(Vector3(0.0, -floor_size.y * 0.5, 0.0));
	add_child(floor_body);
}

void GameSpawner::spawn_car() {
	Ref<PackedScene> car_scene = ResourceLoader::get_singleton()->load(car_scene_path);
	if (car_scene.is_null()) {
		ERR_PRINT("GameSpawner could not load car scene: " + car_scene_path);
		return;
	}

	car_body = memnew(CharacterBody3D);
	car_body->set_name("PlayerCar");
	car_body->set_position(Vector3(0.0, 0.6, 0.0));

	// The GLB is visual-only; give its CharacterBody3D a simple physics shape.
	Ref<BoxShape3D> car_shape;
	car_shape.instantiate();
	car_shape->set_size(Vector3(1.8, 1.0, 3.8));
	CollisionShape3D *car_collision = memnew(CollisionShape3D);
	car_collision->set_shape(car_shape);
	car_body->add_child(car_collision);

	Node *car_visual = car_scene->instantiate();
	if (car_visual == nullptr) {
		ERR_PRINT("GameSpawner could not instantiate car scene: " + car_scene_path);
		memdelete(car_body);
		car_body = nullptr;
		return;
	}

	car_body->add_child(car_visual);
	add_child(car_body);
}

void GameSpawner::_physics_process(double p_delta) {
	if (car_body == nullptr) {
		return;
	}

	Input *input = Input::get_singleton();
	const float throttle = input->get_axis("ui_down", "ui_up");
	const float steering = input->get_axis("ui_left", "ui_right");
	const float delta = static_cast<float>(p_delta);

	Vector3 velocity = car_body->get_velocity();
	// Kenney vehicle scenes face -Z, which is Godot's conventional forward axis.
	const float target_speed = -throttle * car_speed;
	velocity.z = Math::move_toward(velocity.z, target_speed, acceleration * delta);
	
	// FIXED: Changed 0.0 to 0.0f to resolve compiler ambiguity
	velocity.x = Math::move_toward(velocity.x, 0.0f, acceleration * delta);
	velocity.y -= gravity * delta;

	car_body->set_velocity(velocity);
	car_body->rotate_y(-steering * steering_speed * delta);
	car_body->move_and_slide();
}

void GameSpawner::set_car_scene_path(const String &p_path) {
	car_scene_path = p_path;
}

String GameSpawner::get_car_scene_path() const {
	return car_scene_path;
}

void GameSpawner::set_floor_size(const Vector3 &p_size) {
	floor_size = Vector3(Math::max(p_size.x, 1.0f), Math::max(p_size.y, 0.01f), Math::max(p_size.z, 1.0f));
}

Vector3 GameSpawner::get_floor_size() const {
	return floor_size;
}

void GameSpawner::set_car_speed(float p_speed) {
	car_speed = Math::max(p_speed, 0.0f);
}

float GameSpawner::get_car_speed() const {
	return car_speed;
}

void GameSpawner::set_acceleration(float p_acceleration) {
	acceleration = Math::max(p_acceleration, 0.0f);
}

float GameSpawner::get_acceleration() const {
	return acceleration;
}

void GameSpawner::set_steering_speed(float p_speed) {
	steering_speed = Math::max(p_speed, 0.0f);
}

float GameSpawner::get_steering_speed() const {
	return steering_speed;
}

void GameSpawner::set_gravity(float p_gravity) {
	gravity = Math::max(p_gravity, 0.0f);
}

float GameSpawner::get_gravity() const {
	return gravity;
}