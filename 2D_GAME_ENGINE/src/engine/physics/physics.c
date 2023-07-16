#include "physics.h"
#include "../ecs/ecs.h"
#include "../containers/bitset/bitset.h"
#include "../containers/array_list/array_list.h"
#include "../util.h"
#include "../types.h"

static f32 accumulator;
static f32 tick_rate;
static const u32 max_iterations = 10;
static u32 current_iterations;

static Bitset32 dynamic_body_signature;
static Bitset32 static_body_signature;

vec2 physics_gravity = { 0, 10 };
vec2 physics_terminal_velocity = { 1000, 1000 };

static ArrayList collided;

static bool physics_array_list_callback(void* a, void* b) {
	return ((Hit*)a)->time < ((Hit*)b)->time;
}

void physics_init(void) {
	accumulator = 0;
	tick_rate = 0.001;
	current_iterations = 0;
	static_body_signature = bitset_create_32();
	dynamic_body_signature = bitset_create_32();

	ecs_register_component(AABB);
	ecs_register_component(StaticBody);

	bitset_set_32(&static_body_signature, ecs_get_component_index(AABB));
	bitset_set_32(&static_body_signature, ecs_get_component_index(StaticBody));
	ecs_insert_system(static_body_signature);

	ecs_register_component(DynamicBody);

	bitset_set_32(&dynamic_body_signature, ecs_get_component_index(AABB));
	bitset_set_32(&dynamic_body_signature, ecs_get_component_index(DynamicBody));
	ecs_insert_system(dynamic_body_signature);

	collided = array_list_create(0, sizeof(Hit));
}

static void aabb_min_max(vec2 min, vec2 max, AABB aabb) {
	vec2_sub(min, aabb.position, aabb.half_size);
	vec2_add(max, aabb.position, aabb.half_size);
}

bool physics_aabb_point_intersect(vec2 point, AABB aabb) {
	vec2 min, max;
	aabb_min_max(min, max, aabb);
	return point[0] >= min[0] && point[0] <= max[0] && point[1] >= min[1] && point[1] <= max[1];
}

bool physics_aabb_rectangle_intersect(AABB a, AABB b) {
	AABB sum = {
		.position = {b.position[0], b.position[1]},
		.half_size = {
			a.half_size[0] + b.half_size[0],
			a.half_size[1] + b.half_size[1]
		}
	};

	return physics_aabb_point_intersect(a.position, sum);
}

static void physics_update_dynamic_bodies(void) {
	System* dynamic_bodies = ecs_get_entities(dynamic_body_signature);
	System* static_bodies = ecs_get_entities(static_body_signature);

	for (usize i = 0; i < dynamic_bodies->length; i++) {
		DynamicBody* body = ecs_get_component(DynamicBody, dynamic_bodies->entities[i]);
		AABB* body_aabb = ecs_get_component(AABB, dynamic_bodies->entities[i]);
		
		vec2 terminal_velocity_cache = {
			tick_rate * physics_terminal_velocity[0],
			tick_rate * physics_terminal_velocity[1]
		};

		vec2_add(body->acceleration, body->acceleration, physics_gravity);
		vec2_scale(body->acceleration, body->acceleration, tick_rate);
		vec2_add(body->velocity, body->velocity, body->acceleration);

		if (fabsf(body->velocity[0]) > terminal_velocity_cache[0]) {
			if (body->velocity[0] > 0)
				body->velocity[0] = terminal_velocity_cache[0];
			else
				body->velocity[0] = -terminal_velocity_cache[0];
		}

		if (fabsf(body->velocity[1]) > terminal_velocity_cache[1]) {
			if (body->velocity[1] > 0)
				body->velocity[1] = terminal_velocity_cache[1];
			else
				body->velocity[1] = -terminal_velocity_cache[1];
		}

		AABB body_filter = {
			.position = {body_aabb->position[0], body_aabb->position[1]},
			.half_size = {
				body_aabb->half_size[0] * 2 + fabsf(body->velocity[0]),
				body_aabb->half_size[1] * 2 + fabsf(body->velocity[1])
			}
		};

		for (usize j = 0; j < static_bodies->length; j++) {
			StaticBody* static_body = ecs_get_component(StaticBody, static_bodies->entities[j]);
			AABB static_aabb = *ecs_get_component(AABB, static_bodies->entities[j]);

			if ((static_body->layer & body->layer) == static_body->layer &&
				physics_aabb_rectangle_intersect(body_filter, static_aabb)) {

				static_aabb.half_size[0] += body_aabb->half_size[0];
				static_aabb.half_size[1] += body_aabb->half_size[1];

				Hit hit = physics_aabb_ray_intersect(body_aabb->position, body->velocity, static_aabb);
				if (hit.is_hit)
					array_list_insert_sorted(&collided, &hit, physics_array_list_callback);
			}
		}

		for (usize j = 0; j < collided.length; j++) {
			Hit hit = *(Hit*)array_list_get(&collided, i);
			body->velocity[0] += hit.normal[0] * fabsf(body->velocity[0]) * (1 - hit.time);
			body->velocity[1] += hit.normal[1] * fabsf(body->velocity[1]) * (1 - hit.time);
			body->last_normal[0] = hit.normal[0];
			body->last_normal[1] = hit.normal[1];
		}

		if (!collided.length) {
			body->last_normal[0] = 0;
			body->last_normal[1] = 0;
		}

		usize length = collided.length;
		for (usize j = 0; j < length; j++)
			array_list_remove(&collided, 0);

		body_aabb->position[0] += body->velocity[0];
		body_aabb->position[1] += body->velocity[1];
	}
}

void physics_update(f32 dt) {
	accumulator += dt;
	while (accumulator >= tick_rate && current_iterations < max_iterations) {
		physics_update_dynamic_bodies();
		accumulator -= tick_rate;
	}
	current_iterations = 0;
}

Hit physics_aabb_ray_intersect(vec2 position, vec2 direction, AABB target) {
	Hit hit = { .is_hit = false };

	vec2 cache_direction = {
		1 / direction[0],
		1 / direction[1]
	};

	vec2 t_near, t_far;
	for (int i = 0; i < 2; i++) {
		t_near[i] = ((target.position[i] - target.half_size[i]) - position[i]) * cache_direction[i];
		t_far[i] = ((target.position[i] + target.half_size[i]) - position[i]) * cache_direction[i];
	}

	if (isnan(t_near[0]) || isnan(t_near[1]))
		return hit;
	if (isnan(t_far[0])  || isnan(t_far[1]))
		return hit;

	for (int i = 0; i < 2; i++)
		if (t_near[i] > t_far[i])
			SWAP(&t_near[i], &t_far[i], f32);

	if (t_near[0] > t_far[1] || t_near[1] > t_far[0])
		return hit;

	f32 t_hit_near = fmaxf(t_near[0], t_near[1]);
	f32 t_hit_far  = fmaxf(t_far[0], t_far[1]);

	if (t_hit_near < 0 || t_hit_far < 0 || t_hit_near > 1)
		return hit;

	hit.position[0] = position[0] + direction[0] * t_hit_near;
	hit.position[1] = position[1] + direction[1] * t_hit_near;

	if (t_near[0] > t_near[1]) {
		if (direction[0] > 0) {
			hit.normal[0] = -1;
			hit.normal[1] = 0;
		} else {
			hit.normal[0] = 1;
			hit.normal[1] = 0;
		}
	} else {
		if (direction[1] > 0) {
			hit.normal[0] = 0;
			hit.normal[1] = -1;
		} else {
			hit.normal[0] = 0;
			hit.normal[1] = 1;
		}
	}

	hit.time = t_hit_near;
	hit.is_hit = true;

	return hit;
}

void physics_insert_dynamic_body(usize entity, vec2 position, vec2 size, CollisionLayer layer) {
	AABB* aabb = ecs_insert_component(AABB, entity);
	memcpy(&aabb->position, position, sizeof(vec2));
	aabb->half_size[0] = size[0] * 0.5;
	aabb->half_size[1] = size[1] * 0.5;

	DynamicBody* body = ecs_insert_component(DynamicBody, entity);
	vec2_scale(body->velocity, body->velocity, 0);
	vec2_scale(body->acceleration, body->acceleration, 0);
	body->layer = layer;
}

void physics_insert_static_body(usize entity, vec2 position, vec2 size, CollisionLayer layer) {
	AABB* aabb = ecs_insert_component(AABB, entity);
	memcpy(&aabb->position, position, sizeof(vec2));
	aabb->half_size[0] = size[0] * 0.5;
	aabb->half_size[1] = size[1] * 0.5;

	StaticBody* body = ecs_insert_component(StaticBody, entity);
	body->layer = layer;
}