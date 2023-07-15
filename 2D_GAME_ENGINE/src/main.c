#include <stdbool.h>
#include <linmath.h>

#include "engine/render/render.h"

int main(int argc, char* argv[])
{
    SDL_Window* window = render_init(1920, 1080, "GAME");

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

        render_quad((vec2){1920 / 2, 1080 / 2}, (vec2){50, 50}, (vec4){1, 0, 0, 1});

        render_end();
    }
}