#include <stdbool.h>
#include <linmath.h>
#include <stdio.h>

#include "engine/render/render.h"
#include "engine/ecs/ecs.h"
#include "engine/util.h"
#include "engine/physics/physics.h"
#include "engine/input/input.h"

/*
TODO:
    consider moving sprite, sprite_sheets, and animated_sprites to their own files
    add collision between dyanamic bodies
*/

int main(int argc, char* argv[])
{
    ecs_init();
    time_init(144);
    physics_init();
    input_init();
    u32 width = 1920;
    u32 height = 1080;
    SDL_Window* window = render_init(1920, 1080, "GAME");

    while (!input_get_quit_state()) {
        input_poll_events();
        time_update();
        render_begin();

        physics_update(time_get_delta());

        render_end();
        time_update_late();
    }
}