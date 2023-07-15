#include "render.h"
#include "render_internal.h"
#include "../util.h"

static SDL_Window* window;

static vbo_quad;
static ebo_quad;
static vao_quad;

SDL_Window* render_init(u32 width, u32 height, const char* window_name) {
    window = render_init_window(width, height, window_name);
    
    render_init_quad(&vbo_quad, &ebo_quad, &vao_quad);
    
    return window;
}

void render_begin(void) {
    glClearColor(0, 0, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(void) {
    SDL_GL_SwapWindow(window);
}

void render_quad(void) {
    glBindVertexArray(vao_quad);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void render_line(void) {

}