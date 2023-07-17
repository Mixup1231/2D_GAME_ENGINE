#include <stdbool.h>
#include <linmath.h>
#include <stdio.h>

#include "engine/ecs/ecs.h"
#include "engine/render/render.h"
#include "engine/physics/physics.h"
#include "engine/input/input.h"
#include "engine/util.h"

Bitset32 update_characters_signature;

void render_characters(void) {
    System* entities = ecs_get_entities(update_characters_signature);
    for (usize i = 0; i < entities->length; i++) {
        AnimatedSprite* animation = ecs_get_component(AnimatedSprite, entities->entities[i]);
        AABB* body = ecs_get_component(AABB, entities->entities[i]);
        render_update_animation(animation, time_get_delta());
        render_sprite_sheet(&animation->sheets[animation->current_sheet], animation->row, animation->column, body->position, (vec2) { body->half_size[0] * 2, body->half_size[1] * 2 });
    }
}

/*
TODO:
    add hold times for individual sprite sheets
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

    update_characters_signature = bitset_create_32();
    bitset_set_32(&update_characters_signature, ecs_get_component_index(AnimatedSprite));
    bitset_set_32(&update_characters_signature, ecs_get_component_index(DynamicBody));
    ecs_insert_system(update_characters_signature);

    usize player = ecs_create_entity();
    AnimatedSprite* animation = render_insert_animated_sprite(player, 0.1);
    render_push_animated_sheet(animation, "assets/Player/idle.png", 128, 128, (vec2) { 1.5, 1.5 }, WHITE);
    render_push_animated_sheet(animation, "assets/Player/run.png", 128, 128, (vec2) { 1.5, 1.5 }, WHITE);
    render_push_animated_sheet(animation, "assets/Player/jump.png", 128, 128, (vec2) { 1.5, 1.5 }, WHITE);
    animation->current_sheet = 0;
    vec2 size;
    size[0] = animation->sheets[0].cell_width  * animation->sheets[0].scale[0];
    size[1] = animation->sheets[0].cell_height * animation->sheets[0].scale[1];
    physics_insert_dynamic_body(player, (vec2) { width / 2, height / 2 }, size, COLLISION_LAYER_TERRAIN, NULL);
    DynamicBody* player_body = ecs_get_component(DynamicBody, player);
    AABB* player_aabb = ecs_get_component(AABB, player);

    usize floor = ecs_create_entity();
    physics_insert_static_body(floor, (vec2) { width / 2, height - 50 }, (vec2) { width, 100 }, COLLISION_LAYER_TERRAIN);
    AABB* floor_aabb = ecs_get_component(AABB, floor);

    bool on_ground = true;
    while (!input_get_quit_state()) {
        input_poll_events();
        time_update();
        render_begin();

        if (player_body->velocity[0] >= 0) {
            animation->sheets[animation->current_sheet].scale[0] = 1.5;
        }
        if (player_body->velocity[0] <= 0) {
            animation->sheets[animation->current_sheet].scale[0] = -1.5;
        }
        if (input_get_key_state(SDL_SCANCODE_D) == INPUT_PRESSED && player_body->last_normal[1])
            render_change_animated_sheet(animation, 1);
        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_PRESSED && player_body->last_normal[1])
            render_change_animated_sheet(animation, 1);
        if (input_get_key_state(SDL_SCANCODE_D) == INPUT_HELD && input_get_key_state(SDL_SCANCODE_A) != INPUT_HELD)
            player_body->acceleration[0] += 48;
        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_HELD && input_get_key_state(SDL_SCANCODE_D) != INPUT_HELD)
            player_body->acceleration[0] -= 48;
        if (input_get_key_state(SDL_SCANCODE_A) == INPUT_RELEASED || input_get_key_state(SDL_SCANCODE_D) == INPUT_RELEASED) {
            player_body->velocity[0] = 0;
            render_change_animated_sheet(animation, 0);
        }
        if (input_get_key_state(SDL_SCANCODE_SPACE) == INPUT_PRESSED && player_body->last_normal[1]) {
            player_body->acceleration[1] -= 3000;
            render_change_animated_sheet(animation, 2);
            on_ground = false;
            player_body->last_normal[1] = 0;
        }
        if (player_body->last_normal[1] && !on_ground) {
            render_change_animated_sheet(animation, 0);
            on_ground = true;
        }

        render_characters();
        physics_update(time_get_delta());

        render_end();
        time_update_late();
    }
}