#include <stdbool.h>
#include <linmath.h>
#include <stdio.h>

#include "engine/render/render.h"
#include "engine/ecs/ecs.h"
#include "engine/util.h"
#include "engine/physics/physics.h"

Bitset32 render_bodies_signature;

void render_bodies(void) {
    System* system = ecs_get_entities(render_bodies_signature);
    for (usize i = 0; i < system->length; i++) {
        Sprite* sprite = ecs_get_component(Sprite, system->entities[i]);
        AABB* aabb = ecs_get_component(AABB, system->entities[i]);
        static vec2 size;
        size[0] = sprite->scale[0] * (aabb->half_size[0] * 2);
        size[1] = sprite->scale[1] * (aabb->half_size[1] * 2);
        render_quad(aabb->position, size, sprite->colour, sprite->texture_id);
    }
}

int main(int argc, char* argv[])
{
    ecs_init();
    time_init(144);
    physics_init();
    u32 width = 1920;
    u32 height = 1080;
    SDL_Window* window = render_init(1920, 1080, "GAME");

    render_bodies_signature = bitset_create_32();
    bitset_set_32(&render_bodies_signature, ecs_get_component_index(AABB));
    bitset_set_32(&render_bodies_signature, ecs_get_component_index(Sprite));
    ecs_insert_system(render_bodies_signature);

    u32 grass_block = render_load_texture("C:\\Users\\Mitchell\\Pictures\\sprites\\grass_block.png", false);

    usize block = ecs_create_entity();
    physics_insert_dynamic_body(block, (vec2) { width / 2, height / 2 }, (vec2) { 50, 50 }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(block, grass_block, (vec2) { 1, 1 }, WHITE);

    usize floor = ecs_create_entity();
    physics_insert_static_body(floor, (vec2) { width / 2, height - 50 }, (vec2) { width, 100 }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(floor, 0, (vec2) { 1, 1 }, WHITE);

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
        render_bodies();
        
        physics_update(time_get_delta());

        render_end();

        time_update_late();
    }
}