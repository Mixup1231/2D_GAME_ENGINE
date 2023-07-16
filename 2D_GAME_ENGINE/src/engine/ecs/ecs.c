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

System system_create(Bitset32 mask) {
	System system = {
		.length = 0,
		.mask = mask
	};

	return system;
}

void system_insert_entity(System* system, usize entity) {
	assert(system->length < MAX_SYSTEMS);
	assert(0 <= entity && entity < MAX_ENTITIES);

	system->entities[system->length++] = entity;
}

void system_remove_entity(System* system, usize entity) {
	assert(system->length > 0);
	assert(0 <= entity && entity < MAX_ENTITIES);

	for (usize i = 0; i < system->length; i++)
		if (system->entities[i] == entity)
			system->entities[i] = system->entities[system->length--];
}

void ecs_init(void) {
	assert(!ecs);

	ecs = malloc(sizeof(ECS));
	assert(ecs);

	ecs->entities = stack_create(MAX_ENTITIES, sizeof(usize));
	for (usize i = 0; i < MAX_ENTITIES; i++)
		stack_push(&ecs->entities, &i);

	SIGNATURE = bitset_create_32();
	ecs->signatures = map_create(sizeof(Bitset32), hash_size);
	ecs->component_index = map_create(sizeof(usize), hash_string);
	ecs->mask_system = map_create(sizeof(usize), hash_uint);

	ecs->length = 0;
	ecs->systems_inserted = 0;
}

usize ecs_create_entity(void) {
	usize entity = *(usize*)stack_pop(&ecs->entities);
	map_insert(ecs->signatures, &entity, &SIGNATURE);
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

void ecs_register_component_impl(const char* name, usize component_size) {
	assert(ecs->length < MAX_COMPONENTS);
	assert(!map_contains(ecs->component_index, name));

	map_insert(ecs->component_index, name, &ecs->length);
	component_array_init(&ecs->components[ecs->length], sizeof(component_size));
	ecs->length++;
}

void* ecs_insert_component_impl(const char* name, usize entity) {
	assert(map_contains(ecs->component_index, name));
	assert(map_contains(ecs->signatures, &entity));

	Bitset32* signature = map_get(ecs->signatures, &entity);
	usize comp_index = *(usize*)map_get(ecs->component_index, name);
	assert(!bitset_test_32(signature, comp_index));

	bitset_set_32(signature, comp_index);
	if (map_contains(ecs->mask_system, &signature->bits))
		system_insert_entity(&ecs->systems[*(usize*)map_get(ecs->mask_system, &signature->bits)], entity);
	
	return component_array_insert(&ecs->components[comp_index], entity);
}

void ecs_remove_component_impl(const char* name, usize entity) {
	assert(map_contains(ecs->component_index, name));
	assert(map_contains(ecs->signatures, &entity));

	Bitset32* signature = map_get(ecs->signatures, &entity);
	usize comp_index = *(usize*)map_get(ecs->component_index, name);
	assert(bitset_test_32(signature, comp_index));

	if (map_contains(ecs->systems, &signature->bits))
		system_remove_entity(&ecs->systems[*(usize*)map_get(ecs->mask_system, &signature->bits)], entity);
	bitset_zero_32(signature, comp_index);
	component_array_remove(&ecs->components[comp_index], entity);
}

void* ecs_get_component_impl(const char* name, usize entity) {
	assert(map_contains(ecs->component_index, name));
	assert(map_contains(ecs->signatures, &entity));

	Bitset32* signature = map_get(ecs->signatures, &entity);
	usize comp_index = *(usize*)map_get(ecs->component_index, name);
	assert(bitset_test_32(signature, comp_index));

	return component_array_get(&ecs->components[comp_index], entity);
}

usize ecs_get_component_index_impl(const char* name) {
	assert(map_contains(ecs->component_index, name));

	return *(usize*)map_get(ecs->component_index, name);
}

void ecs_insert_system(Bitset32 mask) {
	for (usize i = 0; i < ecs->systems_inserted; i++)
		if (ecs->systems->mask.bits == mask.bits)
			return;
	
	ecs->systems[ecs->systems_inserted] = system_create(mask);
	map_insert(ecs->mask_system, &mask.bits, &ecs->systems_inserted);
	ecs->systems_inserted++;
}

System ecs_get_entities(Bitset32 mask) {
	assert(map_contains(ecs->mask_system, &mask.bits));

	return ecs->systems[*(usize*)map_get(ecs->mask_system, &mask.bits)];
}