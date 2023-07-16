#pragma once
#include <stdbool.h>

#include "../../types.h"

typedef struct arrayList {
	usize length;
	usize capacity;
	usize item_size;
	void* items;
} ArrayList;

typedef bool array_list_predicate(void* a, void* b);

ArrayList array_list_create(usize length, usize item_size);
void array_list_push_back(ArrayList* list, void* item);
void array_list_remove(ArrayList* list, usize index);
void* array_list_get(ArrayList* list, usize index);
void array_list_insert_sorted(ArrayList* list, void* item, array_list_predicate* callback);