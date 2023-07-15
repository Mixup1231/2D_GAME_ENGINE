#include <stdbool.h>
#include <linmath.h>
#include <stdio.h>

#include "engine/render/render.h"
#include "engine/ecs/ecs.h"
#include "engine/util.h"

int main(int argc, char* argv[])
{
    ecs_init();
    SDL_Window* window = render_init(1920, 1080, "GAME");

    u32 grass_block = render_load_texture("C:\\Users\\Mitchell\\Pictures\\sprites\\grass_block.png", false);

    bool should_close = false;
    while (!should_close) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                should_close = true;
                break;
            }
        }

        render_begin();

        render_quad((vec2){1920 / 2, 1080 / 2}, (vec2){50, 50}, WHITE, grass_block);
        render_line((vec2){ 0, 0 }, (vec2){ 1920 / 2, 1080 / 2 }, CYAN, 3);

        render_end();
    }
}