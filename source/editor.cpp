//
// Created by temdisponivel on 19/02/2018.
//

#include "editor.h"

#define ZOOM_VELOCITY 40
#define CAM_MOVEMENT_VELOCITY .6f

static editor_state_t *state;

void setup_editor(camera_t *editor_camera) {
    state = (editor_state_t *) memalloc(sizeof(editor_state_t));
    state->camera_state.editor_camera = editor_camera;
    state->camera_state.last_mouse_pos = get_mouse_screen_pos();
}

void update_editor_camera() {
    camera_t *cam = state->camera_state.editor_camera;
    glm::vec2 mouse_pos = get_mouse_screen_pos();
    glm::vec2 mouse_delta = mouse_pos - state->camera_state.last_mouse_pos;
    state->camera_state.last_mouse_pos = mouse_pos;

    glm::vec3 cam_forward = get_forward(cam->entity->transform);
    cam->entity->transform->position += cam_forward * get_mouse_delta_scroll().y * (get_dt() * ZOOM_VELOCITY);

    glm::vec2 mouse_movement = glm::vec3(mouse_delta.x, mouse_delta.y, 0) * CAM_MOVEMENT_VELOCITY;
    if (is_mouse_button_down(MOUSE_BUTTON_MIDDLE)) {
        cam->entity->transform->position -= get_right(cam->entity->transform) * mouse_movement.x;
        cam->entity->transform->position += get_up(cam->entity->transform) * mouse_movement.y;
    }

    if (is_mouse_button_down(MOUSE_BUTTON_RIGHT)) {
        glm::quat right_quat = glm::angleAxis(-mouse_movement.x * get_dt(), get_up(cam->entity->transform));// world_up());// get_right(cam->entity->transform));
        glm::quat up_quat = glm::angleAxis(-mouse_movement.y * get_dt(), get_right(cam->entity->transform));// world_right());//get_up(cam->entity->transform));
        cam->entity->transform->rotation *= right_quat;
        cam->entity->transform->rotation *= up_quat;
    }
}