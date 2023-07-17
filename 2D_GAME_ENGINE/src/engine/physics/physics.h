#pragma once
#include <linmath.h>
#include <stdbool.h>

#include "../time/time.h"

#define MAX_DYNAMIC_BODY_OVERLAPS 5

typedef struct {
	vec2 position;
	vec2 normal;
	float time;
	usize static_entity;
	bool is_hit;
} Hit;

typedef void collision_callback(Hit hit, usize entity);

typedef struct {
	usize entities[MAX_DYNAMIC_BODY_OVERLAPS];
	usize length;
} DynamicBodyOverlaps;

typedef enum {
	COLLISION_LAYER_TERRAIN,
	COLLISION_LAYER_ONE,
	COLLISION_LAYER_TWO
} CollisionLayer;

typedef struct {
	vec2 position;
	vec2 half_size;
} AABB;

typedef struct {
	CollisionLayer layer;
} StaticBody;

typedef struct {
	vec2 acceleration;
	vec2 velocity;
	CollisionLayer layer;
	vec2 last_normal;
	collision_callback* callback;
} DynamicBody;

void physics_init(void);
bool physics_aabb_point_intersect(vec2 point, AABB aabb);
bool physics_aabb_rectangle_intersect(AABB a, AABB b);
void physics_update(f32 dt);
DynamicBodyOverlaps physics_dynamic_body_overlap(usize entity);
Hit physics_aabb_ray_intersect(vec2 position, vec2 direction, AABB target);
void physics_insert_dynamic_body(usize entity, vec2 position, vec2 size, CollisionLayer layer, collision_callback* callback);
void physics_insert_static_body(usize entity, vec2 position, vec2 size, CollisionLayer layer);