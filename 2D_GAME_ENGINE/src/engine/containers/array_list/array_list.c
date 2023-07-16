#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array_list.h"
#include "../../util.h"

#define ARRAY_LIST_ERROR_MESSAGE_ALLOCATION "\nFailed to allocate array list...returning NULL.\n"
#define ARRAY_LIST_ERROR_MESSAGE_REALLOCATION "\nFailed to expand array list...exiting.\n"

ArrayList array_list_create(usize length, usize item_size) {
	ArrayList list = {
		.capacity = !length ? 100 : 2 * length,
		.length = length,
		.item_size = item_size,
	};

	list.items = malloc(list.capacity * item_size);
	if (!list.items) {
		ERROR_RETURN(list, ARRAY_LIST_ERROR_MESSAGE_ALLOCATION);
	}

	memset(list.items, 0, list.length * list.item_size);

	return list;
}

void array_list_push_back(ArrayList* list, void* item) {
	assert(list && item);

	if (list->capacity == list->length) {
		list->capacity *= 2;

		void* success = realloc(list->items, list->capacity * list->item_size);

		if (!list->items) {
			ERROR_EXIT(ARRAY_LIST_ERROR_MESSAGE_REALLOCATION);
		}

		list->items = success;
	}

	void* last = (u8*)list->items + list->length * list->item_size;
	if (!last) {
		ERROR_EXIT(ARRAY_LIST_ERROR_MESSAGE_ALLOCATION);
	}

	memcpy(last, item, list->item_size);
	list->length++;
}

void array_list_remove(ArrayList* list, usize index) {
	assert(list && 0 <= index && index < list->length);

	memcpy(array_list_get(list, index), array_list_get(list, list->length - 1), list->item_size);
	list->length--;
}

void* array_list_get(ArrayList* list, usize index) {
	assert(list && 0 <= index && index < list->length);

	return (u8*)list->items + index * list->item_size;
}

void array_list_insert_sorted(ArrayList* list, void* item, array_list_predicate* callback) {
	assert(list && callback && item);

	if (!list->length) {
		array_list_push_back(list, item);
		return;
	}

	if (list->capacity == list->length) {
		list->capacity *= 2;

		void* success = realloc(list->items, list->capacity * list->item_size);

		if (!list->items)
			ERROR_EXIT(ARRAY_LIST_ERROR_MESSAGE_REALLOCATION);

		list->items = success;
	}

	bool inserted = false;
	for (usize i = 0; i < list->length; i++) {
		if (callback(item, array_list_get(list, i))) {
			for (usize j = list->length; j >= i + 1; j--) {
				if (!((u8*)list->items + j * list->item_size) || !((u8*)list->items + (j - 1) * list->item_size))
					ERROR_EXIT("\nNULL pointer at index %zu or %zu of array list\n", j, j - 1);

				memcpy(
					(u8*)list->items + j * list->item_size,
					(u8*)list->items + (j - 1) * list->item_size,
					list->item_size
				);
			}

			if (!((u8*)list->items + i * list->item_size))
				ERROR_EXIT("\nNULL pointer at index %zu of array list\n", i);

			memcpy((u8*)list->items + i * list->item_size, item, list->item_size);
			inserted = true;

			list->length++;

			return;
		}
	}

	if (!inserted) {
		array_list_push_back(list, item);
		return;
	}
}