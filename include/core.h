//
// Created by temdisponivel on 13/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_CORE_H
#define CYNICAL_ENGINE_CPP_CORE_H

#include "engine.h"

typedef void (*engine_callback_func)();

typedef struct engine_state {
    GLFWwindow *window;
    bool break_game_loop;

    engine_callback_func update_callback;

    float delta_time;
} engine_state_t;

enum ENGINE_PREPARE_RESULT {
    ERROR_GLFW_INIT,
    ERROR_OPEN_WINDOW,
    SUCCESS
};

engine_state_t get_engine_state();

ENGINE_PREPARE_RESULT prepare(engine_callback_func update_callback);

void loop();

void simulate();

void draw();

void release();

void close_game();

float get_dt();

GLFWwindow *get_window();

#endif //CYNICAL_ENGINE_CPP_CORE_H
