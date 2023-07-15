#pragma once

#include "../types.h"

bool io_serialise_struct(const char* filepath, void* buffer, usize size, usize count);
bool io_deserialise_struct(const char* filepath, void* buffer, usize size, usize count);
