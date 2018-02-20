//
// Created by temdisponivel on 19/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_EDITOR_H
#define CYNICAL_ENGINE_CPP_EDITOR_H

#include "engine.h"
#include "graphics.h"
#include "input.h"

typedef struct editor_camera_state {
    camera_t *editor_camera;
    glm::vec2 last_mouse_pos;
    glm::vec2 acumulated_mouse_rotation;
} editor_camera_state_t;

typedef struct editor_state {
    editor_camera_state_t camera_state;
} editor_state_t;

void setup_editor(camera_t *editor_camera);

void update_editor_camera();

#endif //CYNICAL_ENGINE_CPP_EDITOR_CAMERA_CONTROLLER_H
