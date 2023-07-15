#pragma once
#include <stdbool.h>

#include "../containers/stack/stack.h"
#include "../containers/map/map.h"
#include "../types.h"

#define MAX_ENTITIES 1000
#define MAX_COMPONENTS 32
static SIGNATURE[MAX_COMPONENTS];

typedef struct {
	void* items;
	usize item_size;
	usize length;
	usize entity_index[MAX_ENTITIES];
	usize index_entity[MAX_ENTITIES];
} ComponentArray;

void component_array_init(ComponentArray* arr, usize item_size);
void* component_array_insert(ComponentArray* arr, usize entity);
void component_array_remove(ComponentArray* arr, usize entity);
void* component_array_get(ComponentArray* arr, usize entity);

typedef struct {
	Stack entities;
	Map* signatures;

	ComponentArray components[MAX_COMPONENTS];
	usize length;
	Map* component_index;
} ECS;

extern ECS* ecs;

void ecs_init(void);
usize ecs_create_entity(void);
void ecs_delete_entity(usize entity);

void ecs_register_component(const char* name, usize component_size);
void* ecs_insert_component(const char* name, usize entity);
void ecs_remove_component(const char* name, usize entity);
void* ecs_get_component(const char* name, usize entity);
