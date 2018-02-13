//
// Created by temdisponivel on 13/02/2018.
//

#include "maths.h"

transform_t *math::create_transform() {
    transform_t *transform = (transform_t *) memalloc(sizeof(transform_t));
    transform->position = glm::vec3();
    transform->rotation = glm::quat();
    transform->scale = glm::vec3(1, 1, 1);
    transform->_matrix = glm::mat4();
    return transform;
}
void math::free_transform(transform_t *transform) {
    memfree(transform);
}

void math::prepare_transform_for_draw(transform_t *transform) {
    transform->_matrix = glm::mat4();
    transform->_matrix = glm::translate(transform->_matrix, transform->position);
    transform->_matrix = transform->_matrix * glm::mat4_cast(transform->rotation); // TODO: maybe optimize this so that it doesn't use a matrix
    transform->_matrix = glm::scale(transform->_matrix, transform->scale);
}