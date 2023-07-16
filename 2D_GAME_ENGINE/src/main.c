#include <stdbool.h>
#include <linmath.h>
#include <stdio.h>

#include "engine/render/render.h"
#include "engine/ecs/ecs.h"
#include "engine/util.h"
#include "engine/physics/physics.h"
#include "engine/input/input.h"

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
    input_init();
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
    DynamicBody* block_body = ecs_get_component(DynamicBody, block);

    usize floor = ecs_create_entity();
    physics_insert_static_body(floor, (vec2) { width / 2, height - 50 }, (vec2) { width, 100 }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(floor, 0, (vec2) { 1, 1 }, WHITE);

    usize floor2 = ecs_create_entity();
    physics_insert_static_body(floor2, (vec2) { width / 3, height - 200 }, (vec2) { 200, 100 }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(floor2, 0, (vec2) { 1, 1 }, WHITE);

    usize ceiling = ecs_create_entity();
    physics_insert_static_body(ceiling, (vec2) { width / 2, 50 }, (vec2) { width, 100 }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(ceiling, 0, (vec2) { 1, 1 }, WHITE);

    usize wall = ecs_create_entity();
    physics_insert_static_body(wall, (vec2) { 50, height / 2 }, (vec2) { 100, height }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(wall, 0, (vec2) { 1, 1 }, WHITE);

    usize wall2 = ecs_create_entity();
    physics_insert_static_body(wall2, (vec2) { width - 50, height / 2 }, (vec2) { 100, height }, COLLISION_LAYER_TERRAIN);
    render_insert_sprite(wall2, 0, (vec2) { 1, 1 }, WHITE);

    while (!input_get_quit_state()) {
        input_poll_events();
        time_update();
        render_begin();

        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_HELD && input_get_key_state(SDL_SCANCODE_D) != INPUT_HELD)
            block_body->acceleration[0] -= 100;
        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_RELEASED) {
            block_body->velocity[0] = 0;
            block_body->acceleration[0] = 0;
        }
        if (input_get_key_state(SDL_SCANCODE_D) == INPUT_HELD && input_get_key_state(SDL_SCANCODE_A) != INPUT_HELD)
            block_body->acceleration[0] += 100;
        if (input_get_key_state(SDL_SCANCODE_D) == INPUT_RELEASED) {
            block_body->velocity[0] = 0;
            block_body->acceleration[0] = 0;
        }
        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_HELD && input_get_key_state(SDL_SCANCODE_D) == INPUT_HELD) {
            block_body->velocity[0] = 0;
            block_body->acceleration[0] = 0;
        }
        if (input_get_key_state(SDL_SCANCODE_SPACE) & (INPUT_PRESSED | INPUT_HELD) == input_get_key_state(SDL_SCANCODE_SPACE) && block_body->last_normal[1])
            block_body->acceleration[1] = -2000;

        render_bodies();

        physics_update(time_get_delta());

        render_end();
        time_update_late();
    }
}