#pragma once
#include <stdbool.h>

#include "../containers/stack/stack.h"
#include "../containers/map/map.h"
#include "../containers/bitset/bitset.h"
#include "../types.h"

#define ecs_register_component(type)\
	ecs_register_component_impl(#type, sizeof(type))

#define ecs_insert_component(type, entity)\
	(type*)ecs_insert_component_impl(#type, entity)

#define ecs_remove_comonent(type, entity)\
	ecs_remove_component_impl(#type, entity)

#define ecs_get_component(type, entity)\
	(type*)ecs_get_component_impl(#type, entity)

#define ecs_get_component_index(type)\
	ecs_get_component_index_impl(#type)

#define MAX_ENTITIES 1000
#define MAX_COMPONENTS 32
#define MAX_SYSTEMS 32

static Bitset32 SIGNATURE;

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
	usize entities[MAX_ENTITIES];
	usize length;
	Bitset32 mask;
} System;

System system_create(Bitset32 mask);
void system_insert_entity(System* system, usize entity);
void system_remove_entity(System* system, usize entity);

typedef struct {
	Stack entities;
	Map* signatures;

	ComponentArray components[MAX_COMPONENTS];
	usize length;
	Map* component_index;

	System systems[MAX_SYSTEMS];
	usize systems_inserted;
	Map* mask_system;
} ECS;

extern ECS* ecs;

void ecs_init(void);
usize ecs_create_entity(void);
void ecs_delete_entity(usize entity);
void ecs_register_component_impl(const char* name, usize component_size);
void* ecs_insert_component_impl(const char* name, usize entity);
void ecs_remove_component_impl(const char* name, usize entity);
void* ecs_get_component_impl(const char* name, usize entity);
usize ecs_get_component_index_impl(const char* name);
void ecs_insert_system(Bitset32 mask);
const System* ecs_get_entities(Bitset32 mask);