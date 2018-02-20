//
// Created by temdisponivel on 19/02/2018.
//

#include "editor.h"

#define ZOOM_VELOCITY 60
#define CAM_MOVEMENT_VELOCITY .6f

static editor_state_t *state;

void setup_editor(camera_t *editor_camera) {
    state = (editor_state_t *) memalloc(sizeof(editor_state_t));
    state->camera_state.editor_camera = editor_camera;
    state->camera_state.last_mouse_pos = get_mouse_screen_pos();
}

void update_editor_camera() {
    camera_t *cam = state->camera_state.editor_camera;
    update_transform_matrix(cam->entity->transform);

    glm::vec2 mouse_pos = get_mouse_screen_pos();
    glm::vec2 mouse_delta = mouse_pos - state->camera_state.last_mouse_pos;
    state->camera_state.last_mouse_pos = mouse_pos;

    glm::vec3 cam_forward = get_forward(cam->entity->transform);
    cam->entity->transform->position += cam_forward * get_mouse_delta_scroll().y * (get_dt() * ZOOM_VELOCITY);

    glm::vec2 mouse_movement = glm::vec3(mouse_delta.x, mouse_delta.y, 0) * CAM_MOVEMENT_VELOCITY;
    if (is_mouse_button_down(MOUSE_BUTTON_MIDDLE)) {

        MESSAGEF("%f %f", mouse_movement.x, mouse_movement.y);

        glm::vec3 right = -get_right(cam->entity->transform) * mouse_movement.x;
        glm::vec3 up = get_up(cam->entity->transform) * mouse_movement.y;

        glm::vec3 movement = right + up;

        glm::vec3 current_pos = cam->entity->transform->position;
        glm::vec3 desired_pos = current_pos + movement;

        cam->entity->transform->position = glm::mix(current_pos, desired_pos, get_dt() * 10);
    }

    if (is_mouse_button_down(MOUSE_BUTTON_RIGHT)) {
        mouse_movement = -mouse_movement * get_dt();

        glm::quat current_rot = cam->entity->transform->rotation;

        glm::quat desired_rotation = glm::quat(glm::vec3(mouse_movement.y, mouse_movement.y, 0));

        cam->entity->transform->rotation = current_rot * desired_rotation;//, get_dt() * 10);
    }
}