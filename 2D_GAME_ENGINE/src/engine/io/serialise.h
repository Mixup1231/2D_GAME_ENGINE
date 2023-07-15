#pragma once

#include <stddef.h>

bool io_serialise_struct(const char* filepath, void* buffer, size_t size, size_t count);
bool io_deserialise_struct(const char* filepath, void* buffer, size_t size, size_t count);
