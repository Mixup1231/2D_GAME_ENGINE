#pragma once
#include <linmath.h>
#include <stdbool.h>

#include "../time/time.h"

typedef enum {
	COLLISION_LAYER_TERRAIN
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
} DyanamicBody;

typedef struct {
	vec2 position;
	vec2 normal;
	float time;
	bool is_hit;
} Hit;

void physics_init(void);
bool physics_aabb_point_intersect(vec2 point, AABB aabb);
bool physics_aabb_rectangle_intersect(AABB a, AABB b);
void physics_update(f32 dt);
Hit physics_aabb_ray_intersect(vec2 position, vec2 direction, AABB target);