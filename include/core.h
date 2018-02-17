//
// Created by temdisponivel on 13/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_CORE_H
#define CYNICAL_ENGINE_CPP_CORE_H

#include "engine.h"
#include "time.h"

typedef void (*engine_callback_func)();

typedef struct window {
    GLFWwindow *glfw_window;
    glm::ivec2 size;
    char *title;
} window_t;

typedef struct engine_state {
    window_t *window;
    bool break_game_loop;

    engine_callback_func update_callback;
} engine_state_t;

enum ENGINE_PREPARE_RESULT {
    ERROR_INIT_DEPS,
    ERROR_OPEN_WINDOW,
    SUCCESS
};

typedef struct engine_params {
    char *window_title;
    glm::ivec2 window_size;
    uint gl_major_version;
    uint gl_minor_version;
    engine_callback_func update_callback;
} engine_params_t;

engine_state_t get_engine_state();

ENGINE_PREPARE_RESULT prepare(engine_params_t params);

void loop();

void simulate();

void draw();

void release();

void close_game();

float get_dt();

window_t *get_window();

glm::ivec2 get_screen_size();

float get_window_ratio();

#endif //CYNICAL_ENGINE_CPP_CORE_H
