//
// Created by temdisponivel on 13/02/2018.
//

#include <graphics.h>
#include <input.h>
#include "core.h"

static engine_state_t *engine_state;

engine_state_t get_engine_state() {
    return *engine_state;
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void on_window_resize(GLFWwindow *window, int width, int heigh) {
    engine_state->window->size = glm::ivec2(width, heigh);
    update_cameras_view_port_to_screen_size();
}

ENGINE_PREPARE_RESULT prepare(engine_params_t params) {
    engine_state = (engine_state_t *) memalloc(sizeof(engine_state_t));

    memset(engine_state, 0, sizeof(engine_state_t));

    // TODO: receive these configs through parameters

    GLFWwindow *glfw_window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return ENGINE_PREPARE_RESULT::ERROR_INIT_DEPS;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, params.gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, params.gl_major_version);

    glfw_window = glfwCreateWindow(params.window_size.x, params.window_size.y, params.window_title, NULL, NULL);

    if (glfw_window == null) {
        glfwTerminate();
        return ENGINE_PREPARE_RESULT::ERROR_OPEN_WINDOW;
    }

    glfwMakeContextCurrent(glfw_window);
    CHECK_GL_ERROR();

    glewInit();
    glfwSwapInterval(1);

    glfwSetWindowSizeCallback(glfw_window, &on_window_resize);

    ENSURE(params.update_callback != null);

    engine_state->update_callback = params.update_callback;

    window_t *window = (window_t *) memalloc(sizeof(window_t));

    window->size = params.window_size;
    window->glfw_window = glfw_window;
    window->title = (char *) memalloc((strlen(params.window_title) + 1) * sizeof(char));
    strcpy(window->title, params.window_title);

    engine_state->window = window;

    prepare_graphics();
    prepare_input(glfw_window);

    return ENGINE_PREPARE_RESULT::SUCCESS;
}

void loop() {
    float start = 0;
    float end = 0;

    while (!engine_state->break_game_loop) {
        engine_state->delta_time = end - start;
        start = (float) glfwGetTime();

        update_input();

        simulate();

        engine_state->update_callback();

        draw();

        end = (float) glfwGetTime();
    }
}

void simulate() {
    // TODO: handle input in a more appropriate place!! Maybe create a input file or somethign
    // TODO: handle input in a more appropriate place!! Maybe create a input file or somethign
    // TODO: handle input in a more appropriate place!! Maybe create a input file or somethign

    if (glfwWindowShouldClose(engine_state->window->glfw_window)) {
        engine_state->break_game_loop = true;
    }
}

void draw() {

    // NOTE: We might want to draw multiple times to different buffers!
    draw_scene();

    glfwSwapBuffers(engine_state->window->glfw_window);

    view_port_t screen_vp = get_screen_view_port();
    color_rgba_t clear_color = white();

    set_clear_color(clear_color);
    clear_view_port(screen_vp, CLEAR_COLOR_AND_DEPTH);
}

void release() {
    release_graphics();
    release_input();
    glfwDestroyWindow(engine_state->window->glfw_window);
    memfree(engine_state->window->title);
    memfree(engine_state);
    glfwTerminate();
}

void close_game() {
    engine_state->break_game_loop = true;
}

float get_dt() {
    return engine_state->delta_time;
}

window_t *get_window() {
    return engine_state->window;
}

glm::ivec2 get_screen_size() {
    return engine_state->window->size;
}

float get_window_ratio() {
    glm::ivec2 size = engine_state->window->size;
    return size.x / (float) size.y;
}