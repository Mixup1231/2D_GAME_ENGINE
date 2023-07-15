#include <linmath.h>
#include <stdio.h>

#include "render.h"
#include "render_internal.h"
#include "../containers/map/map.h"
#include "../io/image_io.h"
#include "../util.h"

static SDL_Window* window;

static Map* texture_map;

static u32 default_shader;
static u32 default_texture;

static u32 vbo_quad;
static u32 ebo_quad;
static u32 vao_quad;

static u32 vbo_line;
static u32 vao_line;

SDL_Window* render_init(u32 width, u32 height, const char* window_name) {
    window = render_init_window(width, height, window_name);
    
    default_shader = render_init_shaders("shaders/shader_default.vert", "shaders/shader_default.frag");
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture_map = map_create(sizeof(u32), hash_string);
    
    return window;
}

void render_begin(void) {
    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(void) {
    SDL_GL_SwapWindow(window);
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

u32 render_load_texture(const char* path, bool flip) {
    if (map_contains(texture_map, path))
        return *(u32*)map_get(texture_map, path);

    ImageFile image = io_image_read(path, flip);
    if (!image.is_valid)
        ERROR_RETURN(0, "Failed to read image! %s\nReturning 0...\n", path);

    u32 texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

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
        glDeleteTextures(1, &texture_id);
        ERROR_RETURN(0, "Unsupported image channel count! %s\nReturning 0...\n", path);
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);
    io_image_free(image.image_data);

    map_insert(texture_map, path, &texture_id);

    return texture_id;
}