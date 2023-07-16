#pragma once
#include "..\types.h"

void time_init(f32 frame_rate);
void time_update(void);
void time_update_late(void);

f32 time_get_delta(void);
f32 time_get_frame_time(void);
f32 time_get_frame_rate(void);