#include <stdbool.h>
#include <linmath.h>
#include <stdio.h>

#include "engine/render/render.h"
#include "engine/ecs/ecs.h"
#include "engine/util.h"
#include "engine/physics/physics.h"

int main(int argc, char* argv[])
{
    ecs_init();
    time_init(144);
    physics_init();
    u32 width = 1920;
    u32 height = 1080;
    SDL_Window* window = render_init(1920, 1080, "GAME");

    u32 grass_block = render_load_texture("C:\\Users\\Mitchell\\Pictures\\sprites\\grass_block.png", false);

    usize entity = ecs_create_entity();

    AABB* aabb = ecs_insert_component(AABB, entity);
    aabb->position[0] = width / 2;
    aabb->position[1] = height / 2;
    aabb->half_size[0] = 50;
    aabb->half_size[1] = 50;

    DyanamicBody* body = ecs_insert_component(DyanamicBody, entity);
    body->acceleration[0] = 0;
    body->acceleration[1] = 0;
    body->velocity[0] = 0;
    body->velocity[1] = 0;
    body->layer = COLLISION_LAYER_TERRAIN;

    usize floor = ecs_create_entity();

    AABB* faabb = ecs_insert_component(AABB, floor);
    faabb->position[0] = width / 2;
    faabb->position[1] = height - 50;
    faabb->half_size[0] = width / 2;
    faabb->half_size[1] = 100;

    StaticBody* fbody = ecs_insert_component(StaticBody, floor);
    fbody->layer = COLLISION_LAYER_TERRAIN;

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

        time_update();

        render_begin();

        render_quad(aabb->position, (vec2) { aabb->half_size[0] * 2, aabb->half_size[1] * 2 }, WHITE, grass_block);
        render_quad(faabb->position, (vec2) { faabb->half_size[0] * 2, faabb->half_size[1] * 2 }, WHITE, 0);
        physics_update(time_get_delta());

        render_end();

        time_update_late();
    }
}