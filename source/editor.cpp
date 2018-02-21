//
// Created by temdisponivel on 19/02/2018.
//

#include "editor.h"

#define ZOOM_VELOCITY 100
#define CAM_DRAG_VELOCITY 8.f
#define CAM_MOVEMENT_VELOCITY 50.f
#define CAM_ROTATION_VELOCITY 60.f

static editor_state_t *state;

void setup_editor(camera_t *editor_camera) {
    state = (editor_state_t *) memalloc(sizeof(editor_state_t));
    state->camera_state.editor_camera = editor_camera;
    state->camera_state.last_mouse_pos = get_mouse_screen_pos();
    state->camera_state.acumulated_mouse_rotation = glm::vec2(0, 0);

    shader_t vertex, fragment;
    bool loaded = create_shaders_from_file(
            "data/shaders/editor/editor_grid_vertex.glsl",
            "data/shaders/editor/editor_grid_frag.glsl",
            null,
            &vertex,
            &fragment,
            null
    );

    ENSURE(loaded);

    shader_program_t *program = create_shader_program(
            vertex,
            fragment,
            {}
    );

    model_t *quad = create_quad(glm::vec3(0, 0, 0), 500.f);
    mesh_t *quad_mesh = create_mesh(quad);

    list<uniform_definition_t> *empty = create_list<uniform_definition_t>(1);
    material_t *material = create_material(program, empty);
    material->cull_func = CULL_DISABLED;
    destroy_list(empty);

    mesh_renderer_t *renderer = create_mesh_renderer(material, quad_mesh);
    //look_at(renderer->entity->transform, world_up());

    state->scene_state.grid_quad = renderer;
}

void release_editor() {
    mesh_renderer_t *grid_renderer = state->scene_state.grid_quad;

    destroy_shader(grid_renderer->material->shader->vertex_shader);
    destroy_shader(grid_renderer->material->shader->fragment_shader);
    destroy_shader_program(grid_renderer->material->shader);

    destroy_material(grid_renderer->material);

    destroy_model(grid_renderer->mesh->model);
    destroy_mesh(grid_renderer->mesh);

    destroy_mesh_renderer(grid_renderer);

    memfree(state);
}

void validate_camera_set_commands(camera_t *camera) {
    if (is_key_down(KEY_LEFT_CONTROL)) {
        if (is_key_pressed(KEY_KP_2) || is_key_pressed(KEY_2)) {
            look_at(camera->entity->transform, world_backwards());
        } else if (is_key_pressed(KEY_KP_4) || is_key_pressed(KEY_4)) {
            look_at(camera->entity->transform, world_left());
        } else if (is_key_pressed(KEY_KP_8) || is_key_pressed(KEY_8)) {
            look_at(camera->entity->transform, world_forward());
        } else if (is_key_pressed(KEY_KP_6) || is_key_pressed(KEY_6)) {
            look_at(camera->entity->transform, world_right());
        } else if (is_key_pressed(KEY_KP_5) || is_key_pressed(KEY_5)) {
            look_at(camera->entity->transform, world_down());
        } else if (is_key_pressed(KEY_SLASH) || is_key_pressed(KEY_KP_DIVIDE)) {
            look_at(camera->entity->transform, world_up());
        }
    }
}

float get_lerp_delta() {
    return (float) fmax((get_dt() * 30.f), 1.f);
}

void move_editor_camera(transform_t *trans, glm::vec3 cam_forward, glm::vec3 cam_right) {
    glm::vec3 current_pos = trans->position;
    glm::vec3 desired_pos = current_pos;
    if (is_key_down(KEY_W)) {
        desired_pos += (cam_forward * CAM_MOVEMENT_VELOCITY * get_dt());
    }

    if (is_key_down(KEY_S)) {
        desired_pos -= (cam_forward * CAM_MOVEMENT_VELOCITY * get_dt());
    }

    if (is_key_down(KEY_D)) {
        desired_pos += (cam_right * CAM_MOVEMENT_VELOCITY * get_dt());
    }

    if (is_key_down(KEY_A)) {
        desired_pos -= (cam_right * CAM_MOVEMENT_VELOCITY * get_dt());
    }

    trans->position = glm::mix(current_pos, desired_pos, get_lerp_delta());
}

void update_editor_camera() {
    //TODO: CLEANUP

    camera_t *cam = state->camera_state.editor_camera;
    transform_t *trans = cam->entity->transform;

    validate_camera_set_commands(cam);

    glm::vec2 mouse_pos = get_mouse_screen_pos();
    glm::vec2 mouse_delta = mouse_pos - state->camera_state.last_mouse_pos;
    state->camera_state.last_mouse_pos = mouse_pos;

    glm::vec3 cam_forward = get_forward(trans);
    glm::vec3 cam_right = get_right(trans);
    trans->position += cam_forward * get_mouse_delta_scroll().y * (get_dt() * ZOOM_VELOCITY);

    if (is_mouse_button_down(MOUSE_BUTTON_RIGHT)) {
        if (fabsf(mouse_delta.x) > .1f || fabsf(mouse_delta.y) > .1f) {
            glm::vec2 rotation = state->camera_state.acumulated_mouse_rotation - (mouse_delta * get_dt() * .3f);

            glm::quat current_rot = trans->rotation;

            glm::vec3 rot_dir = glm::vec3(rotation.y, rotation.x, 0);
            //rot_dir = glm::normalize(rot_dir);

            // TODO: smoth this
            trans->rotation = glm::quat(rot_dir);

            state->camera_state.acumulated_mouse_rotation = rotation;
        }

        move_editor_camera(trans, cam_forward, cam_right);
    }

    if (is_mouse_button_down(MOUSE_BUTTON_MIDDLE)) {
        glm::vec2 mouse_movement = glm::vec3(mouse_delta.x, mouse_delta.y, 0) * CAM_DRAG_VELOCITY * get_dt();

        if (fabsf(mouse_movement.x) > .001f || fabsf(mouse_movement.y) > .001f) {
            glm::vec3 right = -get_right(trans) * mouse_movement.x;
            glm::vec3 up = get_up(trans) * mouse_movement.y;

            glm::vec3 movement = right + up;

            glm::vec3 current_pos = trans->position;
            glm::vec3 desired_pos = current_pos + movement;

            trans->position = glm::mix(current_pos, desired_pos, get_lerp_delta());
        }
    }
}

