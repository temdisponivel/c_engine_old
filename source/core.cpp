//
// Created by temdisponivel on 13/02/2018.
//

#include <graphics.h>
#include "core.h"

static engine_state_t *engine_state;

engine_state_t get_engine_state() {
    return *engine_state;
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

ENGINE_PREPARE_RESULT prepare(engine_callback_func update_callback) {
    engine_state = (engine_state_t *) memalloc(sizeof(engine_state_t));

    memset(engine_state, 0, sizeof(engine_state_t));

    // TODO: receive these configs through parameters

    GLFWwindow *window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return ENGINE_PREPARE_RESULT::ERROR_GLFW_INIT;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Cynical Engine", NULL, NULL);

    if (window == null) {
        glfwTerminate();
        return ENGINE_PREPARE_RESULT::ERROR_OPEN_WINDOW;
    }

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    engine_state->window = window;

    ENSURE(update_callback != null);

    engine_state->update_callback = update_callback;

    prepare_graphics();

    return ENGINE_PREPARE_RESULT::SUCCESS;
}

void loop() {
    float start = 0;
    float end = 0;

    while (!engine_state->break_game_loop) {
        engine_state->delta_time = end - start;
        start = (float) glfwGetTime();
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

    if (glfwWindowShouldClose(engine_state->window)) {
        engine_state->break_game_loop = true;
    }
}

void draw_scene() {
    draw_all_renderers();
}

void draw() {
    // NOTE: We might want to draw multiple times to different buffers!
    draw_scene();

    glfwSwapBuffers(engine_state->window);
    glfwPollEvents();
}

void release() {
    release_graphics();
    glfwDestroyWindow(engine_state->window);
    memfree(engine_state);
    glfwTerminate();
}

void close_game() {
    engine_state->break_game_loop = true;
}

float get_dt() {
    return engine_state->delta_time;
}

GLFWwindow *get_window() {
    return engine_state->window;
}