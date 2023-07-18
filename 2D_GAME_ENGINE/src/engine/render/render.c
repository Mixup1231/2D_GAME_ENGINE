#include <linmath.h>
#include <stdio.h>

#include "render.h"
#include "render_internal.h"
#include "../containers/map/map.h"
#include "../io/image_io.h"
#include "../util.h"
#include "../ecs/ecs.h"

static SDL_Window* window;
static u32 window_width;
static u32 window_height;

static Map* texture_map;

static u32 default_shader;
static u32 default_shader_sheet;
static u32 default_texture;

static u32 vbo_quad;
static u32 ebo_quad;
static u32 vao_quad;

static u32 vbo_line;
static u32 vao_line;

void render_load_sprite(Sprite* sprite, const char* path, vec2 scale, vec4 colour) {
    sprite->texture = render_load_texture(path, false, NULL, NULL);
    memcpy(sprite->scale, scale, sizeof(vec2));
    memcpy(sprite->colour, colour, sizeof(vec4));
    return sprite;
}

void render_load_sprite_sheet(SpriteSheet* sheet, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour) {
    sheet->texture = render_load_texture(path, false);
    sheet->cell_width = cell_width;
    sheet->cell_height = cell_height;
    memcpy_s(sheet->scale, sizeof(vec2), scale, sizeof(vec2));
    memcpy_s(sheet->colour, sizeof(vec4), colour, sizeof(vec4));
}

AnimatedSprite render_create_animated_sprite(void) {
    AnimatedSprite animation = {
        .column = 0,
        .columns = 0,
        .row = 0,
        .rows = 0,
        .current_sheet = 0,
        .sheets_length = 0,
        .current_time = 0
    };

    memset(animation.hold_times, 0, sizeof(f32) * MAX_ANIMATION_SPRITE_SHEETS);

    return animation;
}

void render_push_animated_sheet(AnimatedSprite* animation, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour, f32 hold_time) {
    assert(animation && animation->sheets_length < MAX_ANIMATION_SPRITE_SHEETS);

    render_load_sprite_sheet(&animation->sheets[animation->sheets_length], path, cell_width, cell_height, scale, colour);
    animation->hold_times[animation->sheets_length] = hold_time;
    animation->sheets_length++;
    render_change_animated_sheet(animation, animation->sheets_length - 1);
}

void render_change_animated_sheet(AnimatedSprite* animation, u32 sheet) {
    assert(animation && 0 <= sheet && sheet < animation->sheets_length);

    animation->previous_sheet = animation->current_sheet;
    animation->current_sheet = sheet;
    animation->column = 0;
    animation->row = 0;
    animation->columns = animation->sheets[animation->current_sheet].texture.width / animation->sheets[animation->current_sheet].cell_width;
    animation->rows = animation->sheets[animation->current_sheet].texture.height / animation->sheets[animation->current_sheet].cell_height;
}

void render_update_animation(AnimatedSprite* animation, f32 dt) {
    assert(animation);

    animation->current_time += dt;
    while (animation->current_time >= animation->hold_times[animation->current_sheet]) {
        animation->column = (animation->column + 1) % animation->columns;
        animation->current_time -= animation->hold_times[animation->current_sheet];
    }
}

SDL_Window* render_init(u32 width, u32 height, const char* window_name) {
    window = render_init_window(width, height, window_name);
    window_width = width;
    window_height = height;
    
    default_shader = render_init_shaders("shaders/shader_default.vert", "shaders/shader_default.frag");
    default_shader_sheet = render_init_shaders("shaders/shader_default_sheet.vert", "shaders/shader_default.frag");
    render_init_texture_default(&default_texture);
    render_init_quad(&vbo_quad, &ebo_quad, &vao_quad);
    render_init_line(&vbo_line, &vao_line);

    glUseProgram(default_shader);
    mat4x4 projection;
    mat4x4_ortho(projection, 0, width, height, 0, -1, 1);
    glUniformMatrix4fv(
        glGetUniformLocation(default_shader, "projection"),
        1,
        GL_FALSE,
        projection
    );
    glUseProgram(0);

    glUseProgram(default_shader_sheet);
    glUniformMatrix4fv(
        glGetUniformLocation(default_shader_sheet, "projection"),
        1,
        GL_FALSE,
        projection
    );
    glUseProgram(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture_map = map_create(sizeof(Texture), hash_string);

    ecs_register_component(Sprite);
    ecs_register_component(SpriteSheet);
    ecs_register_component(AnimatedSprite);
    
    return window;
}

void render_begin(void) {
    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(void) {
    SDL_GL_SwapWindow(window);
}

u32 render_get_window_width(void) {
    return window_width;
}

u32 render_get_window_height(void) {
    return window_height;
}

void render_quad(vec2 position, vec2 size, vec4 colour, u32 texture) {
    glUseProgram(default_shader);

    if (!texture)
        glBindTexture(GL_TEXTURE_2D, default_texture);
    else
        glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(vao_quad);

    mat4x4 model;
    mat4x4_translate(model, position[0], position[1], 0.f);
    mat4x4_scale_aniso(model, model, size[0], size[1], 1.f);
    glUniformMatrix4fv(
        glGetUniformLocation(default_shader, "model"),
        1,
        GL_FALSE,
        model
    );

    glUniform4f(
        glGetUniformLocation(default_shader, "colour"),
        colour[0],
        colour[1],
        colour[2],
        colour[3]
    );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void render_sprite(Sprite* sprite, vec2 position, vec2 size) {
    static vec2 scaled_size;
    scaled_size[0] = size[0] * sprite->scale[0];
    scaled_size[1] = size[1] * sprite->scale[1];
    render_quad(position, scaled_size, sprite->colour, sprite->texture.id);
}

static void calculate_sprite_sheet_uvs_shift(vec4 shift, f32 row, f32 column, f32 texture_width, f32 texture_height, f32 cell_width, f32 cell_height) {
    f32 w = 1.0 / (texture_width / cell_width);
    f32 h = 1.0 / (texture_height / cell_height);
    shift[0] = column * w;
    shift[1] = row * h;
    shift[2] = shift[0] + w;
    shift[3] = shift[1] + h;
}

void render_sprite_sheet(SpriteSheet* sheet, u32 row, u32 column, vec2 position, vec2 size) {
    static vec4 shift;
    calculate_sprite_sheet_uvs_shift(shift, row, column, sheet->texture.width, sheet->texture.height, sheet->cell_width, sheet->cell_height);

    glUseProgram(default_shader_sheet);

    if (!sheet->texture.id)
        glBindTexture(GL_TEXTURE_2D, default_texture);
    else
        glBindTexture(GL_TEXTURE_2D, sheet->texture.id);

    glBindVertexArray(vao_quad);

    static vec2 scaled_size;
    scaled_size[0] = size[0] * sheet->scale[0];
    scaled_size[1] = size[1] * sheet->scale[1];

    mat4x4 model;
    mat4x4_translate(model, position[0], position[1], 0.f);
    mat4x4_scale_aniso(model, model, scaled_size[0], scaled_size[1], 1.f);
    glUniformMatrix4fv(
        glGetUniformLocation(default_shader_sheet, "model"),
        1,
        GL_FALSE,
        model
    );

    glUniform4f(
        glGetUniformLocation(default_shader_sheet, "colour"),
        sheet->colour[0],
        sheet->colour[1],
        sheet->colour[2],
        sheet->colour[3]
    );

    glUniform4f(
        glGetUniformLocation(default_shader_sheet, "uvs_shift"),
        shift[0],
        shift[1],
        shift[2],
        shift[3]
    );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void render_line(vec2 start, vec2 end, vec4 colour, u32 line_width) {
    glUseProgram(default_shader);
    glLineWidth(line_width);

    f32 x = end[0] - start[0];
    f32 y = end[1] - start[1];
    f32 line[6] = { 0, 0, 0, x, y, 0 };

    mat4x4 model;
    mat4x4_translate(model, start[0], start[1], 0);

    glUniformMatrix4fv(
        glGetUniformLocation(default_shader, "model"),
        1,
        GL_FALSE,
        model
    );

    glUniform4fv(
        glGetUniformLocation(default_shader, "colour"),
        1,
        colour
    );

    glBindTexture(GL_TEXTURE_2D, default_texture);
    glBindVertexArray(vao_line);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line), line);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture render_load_texture(const char* path, bool flip) {
    Texture texture = { .id = 0, .width = 0, .height = 0 };

    if (map_contains(texture_map, path))
        return *(Texture*)map_get(texture_map, path);

    ImageFile image = io_image_read(path, flip);
    if (!image.is_valid)
        ERROR_RETURN(texture, "Failed to read image! %s\nReturning 0...\n", path);

    
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    switch (image.channels) {
    case 2:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, image.width, image.height, 0, GL_RG, GL_UNSIGNED_BYTE, image.image_data);
        break;
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.image_data);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image_data);
        break;
    default:
        io_image_free(image.image_data);
        glDeleteTextures(1, &texture.id);
        ERROR_RETURN(texture, "Unsupported image channel count! %s\nReturning 0...\n", path);
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);

    io_image_free(image.image_data);

    texture.width = image.width;
    texture.height = image.height;
    map_insert(texture_map, path, &texture);

    return texture;
}

Sprite* render_insert_sprite(usize entity, const char* path, vec2 scale, vec4 colour) {
    Sprite* sprite = ecs_insert_component(Sprite, entity);
    render_load_sprite(sprite, path, scale, colour);

    return sprite;
}

SpriteSheet* render_insert_sprite_sheet(usize entity, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour) {
    SpriteSheet* sheet = ecs_insert_component(SpriteSheet, entity);
    render_load_sprite_sheet(sheet, path, cell_width, cell_height, scale, colour);

    return sheet;
}

AnimatedSprite* render_insert_animated_sprite(usize entity) {
    AnimatedSprite* animation = ecs_insert_component(AnimatedSprite, entity);
    *animation = render_create_animated_sprite();

    return animation;
}