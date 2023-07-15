#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "ecs.h"

static ECS* ecs = NULL;

void component_array_init(ComponentArray* arr, usize item_size) {
	arr->items = malloc(item_size * MAX_ENTITIES);
	assert(arr->items);

	arr->item_size = item_size;
	arr->length = 0;

	memset(&arr->entity_index, 0xff, sizeof(usize) * MAX_ENTITIES);
	memset(&arr->index_entity, 0xff, sizeof(usize) * MAX_ENTITIES);
}

void* component_array_insert(ComponentArray* arr, usize entity) {
	assert(0 <= entity && entity < MAX_ENTITIES);
	assert(arr->entity_index[entity] == SIZE_MAX);

	arr->entity_index[entity] = arr->length;
	arr->index_entity[arr->length] = entity;
	arr->length++;

	return component_array_get(arr, entity);
}

void component_array_remove(ComponentArray* arr, usize entity) {
	assert(0 <= entity && entity < MAX_ENTITIES);
	assert(arr->entity_index[entity] != SIZE_MAX);

	void* location = component_array_get(arr, entity);
	void* end_location = component_array_get(arr, arr->length - 1);

	memcpy(location, end_location, arr->item_size);

	usize last_entity = arr->index_entity[arr->length - 1];
	usize deleted_index = arr->entity_index[entity];
	arr->index_entity[deleted_index] = last_entity;
	arr->entity_index[last_entity] = deleted_index;
}

void* component_array_get(ComponentArray* arr, usize entity) {
	assert(0 <= entity && entity < MAX_ENTITIES);
	assert(arr->entity_index[entity] != SIZE_MAX);

	return (u8*)arr->items + arr->entity_index[entity] * arr->item_size;
}

void ecs_init(void) {
	assert(!ecs);

	ecs = malloc(sizeof(ECS));
	assert(ecs);

	ecs->entities = stack_create(MAX_ENTITIES, sizeof(usize));
	for (usize i = 0; i < MAX_ENTITIES; i++)
		stack_push(&ecs->entities, &i);

	memset(SIGNATURE, 0, MAX_COMPONENTS);
	ecs->signatures = map_create(MAX_COMPONENTS * sizeof(bool), hash_uint);
	ecs->component_index = map_create(sizeof(usize), hash_string);

	ecs->length = 0;
}

usize ecs_create_entity(void) {
	usize entity = *(usize*)stack_pop(&ecs->entities);
	map_insert(ecs->signatures, &entity, SIGNATURE);
	return entity;
}

void ecs_delete_entity(usize entity) {
	assert(0 <= entity && entity < MAX_ENTITIES);
	assert(map_contains(ecs->signatures, &entity));

	bool signature[MAX_COMPONENTS];
	memcpy(&signature, map_get(ecs->signatures, &entity), MAX_COMPONENTS);

	for (usize i = 0; i < ecs->length; i++)
		if (signature[i])
			component_array_remove(&ecs->components[i], entity);

	stack_push(&ecs->entities, &entity);
	map_remove(ecs->signatures, &entity);
}

void ecs_register_component(const char* name, usize component_size) {
	assert(ecs->length < MAX_COMPONENTS);
	assert(!map_contains(ecs->component_index, name));

	map_insert(ecs->component_index, name, &ecs->length);
	component_array_init(&ecs->components[ecs->length], sizeof(component_size));
	ecs->length++;
}

void* ecs_insert_component(const char* name, usize entity) {
	assert(map_contains(ecs->component_index, name));
	assert(map_contains(ecs->signatures, &entity));

	bool* signature = map_get(ecs->signatures, &entity);
	usize comp_index = *(usize*)map_get(ecs->component_index, name);
	assert(!signature[comp_index]);

	signature[comp_index] = true;
	
	return component_array_insert(&ecs->components[comp_index], entity);
}

void ecs_remove_component(const char* name, usize entity) {
	assert(map_contains(ecs->component_index, name));
	assert(map_contains(ecs->signatures, &entity));

	bool* signature = map_get(ecs->signatures, &entity);
	usize comp_index = *(usize*)map_get(ecs->component_index, name);
	assert(signature[comp_index]);

	signature[comp_index] = false;
	component_array_remove(&ecs->components[comp_index], entity);
}

void* ecs_get_component(const char* name, usize entity) {
	assert(map_contains(ecs->component_index, name));
	assert(map_contains(ecs->signatures, &entity));

	bool* signature = map_get(ecs->signatures, &entity);
	usize comp_index = *(usize*)map_get(ecs->component_index, name);
	assert(signature[comp_index]);

	return component_array_get(&ecs->components[comp_index], entity);
}