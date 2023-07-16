#pragma once

#define ERROR_LOG(x, ...) { printf(x, __VA_ARGS__); }
#define ERROR_EXIT(x, ...) { ERROR_LOG(x, __VA_ARGS__) exit(1); }
#define ERROR_RETURN(ret, x, ...) { printf(x, __VA_ARGS__); return ret; }

#define SWAP(i, j, type) {\
	type tmp;\
	tmp = *i;\
	*i = *j;\
	*j = tmp;\
}

#define RED    (vec4){1, 0, 0, 1}
#define GREEN  (vec4){0, 1, 0, 1}
#define BLUE   (vec4){0, 0, 1, 1}
#define PURPLE (vec4){0.7, 0, 1, 1}
#define ORANGE (vec4){1, 0.55, 0, 1}
#define YELLOW (vec4){1, 1, 0, 1}
#define CYAN   (vec4){0, 1, 1, 1}
#define WHITE  (vec4){1, 1, 1, 1}
#define BLACK  (vec4){0, 0, 0, 1}