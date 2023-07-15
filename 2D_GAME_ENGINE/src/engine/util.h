#pragma once
#include <stdio.h>

#define ERROR_LOG(x, ...) { printf(x, __VA_ARGS__); }
#define ERROR_EXIT(x, ...) { ERROR_LOG(x, __VA_ARGS__) exit(1); }
#define ERROR_RETURN(ret, x, ...) { printf(x, __VA_ARGS__); return ret; }
