//
// Created by temdisponivel on 13/02/2018.
//

#include "maths.h"

glm::vec3 forward() {
    return glm::vec3(0, 0, -1);
}

glm::vec3 backwards() {
    return -forward();
}

glm::vec3 up() {
    return glm::vec3(0, 1, 0);
}

glm::vec3 down() {
    return -up();
}

glm::vec3 right() {
    return glm::vec3(1, 0, 0);
}

glm::vec3 left() {
    return -right();
}

transform_t *create_transform() {
    transform_t *transform = (transform_t *) memalloc(sizeof(transform_t));
    transform->position = glm::vec3();
    transform->rotation = glm::quat();
    transform->scale = glm::vec3(1, 1, 1);
    transform->_matrix = glm::mat4();
    return transform;
}
void destroy_transform(transform_t *transform) {
    memfree(transform);
}

void update_transform_matrix(transform_t *transform) {
    transform->_matrix = glm::mat4();
    transform->_matrix = glm::translate(transform->_matrix, transform->position);
    transform->_matrix = transform->_matrix * glm::mat4_cast(transform->rotation); // TODO: maybe optimize this so that it doesn't use a matrix
    transform->_matrix = glm::scale(transform->_matrix, transform->scale);
}

glm::vec3 get_forward(transform_t *transform) {
    return transform->rotation * forward();
}

void set_forward(transform_t *transform, glm::vec3 forward) {
    // TODO: optimize this to not use matrix to quat conversion!!
    transform->rotation = glm::quat_cast(glm::lookAt(transform->position, forward, get_up(transform)));
}

glm::vec3 get_up(transform_t *transform) {
    return transform->rotation * up();
}