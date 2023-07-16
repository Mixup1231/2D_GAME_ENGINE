#pragma once
#include <SDL2/SDL.h>

#include "time.h"

static f32 delta;
static f32 now;
static f32 last;

static f32 frame_last;
static f32 frame_delay;
static f32 frame_time;

static f32 frame_rate;
static f32 frame_count;

void time_init(f32 frame_rate) {
	frame_rate = frame_rate;
	frame_delay = 1000.f / frame_rate;
}

void time_update(void) {
	now = (f32)SDL_GetTicks();
	delta = (now - last) / 1000.f;
	last = now;
	frame_count++;

	if (now - frame_last >= 1000.f) {
		frame_rate = frame_count;
		frame_count = 0;
		frame_last = now;
	}
}

void time_update_late(void) {
	frame_time = (f32)SDL_GetTicks() - now;

	if (frame_delay > frame_time) {
		SDL_Delay(frame_delay - frame_time);
	}
}

f32 time_get_delta(void) {
	return delta;
}

f32 time_get_frame_time(void) {
	return frame_time;
}

f32 time_get_frame_rate(void) {
	return frame_rate;
}