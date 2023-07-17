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
    add animation
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

    SpriteSheet sheet;
    render_load_sprite_sheet(&sheet, "C:\\Users\\Mitchell\\Pictures\\sprites\\grass_block.png", 8, 8, (vec2) { 1, 1 }, WHITE);

    u32 x = 0;
    u32 y = 0;
    while (!input_get_quit_state()) {
        input_poll_events();
        time_update();
        render_begin();

        if (input_get_key_state(SDL_SCANCODE_D) == INPUT_PRESSED)
            x = (x + 1) % 4;
        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_PRESSED)
            x = (x - 1) % 4;
        if (input_get_key_state(SDL_SCANCODE_W) == INPUT_PRESSED)
            y = (y - 1) % 4;
        if (input_get_key_state(SDL_SCANCODE_S) == INPUT_PRESSED)
            y = (y + 1) % 4;

        render_sprite_sheet(&sheet, y, x, (vec2) { width / 2, height / 2 }, (vec2) { 100, 100 });

        physics_update(time_get_delta());

        render_end();
        time_update_late();
    }
}