//
// Created by temdisponivel on 13/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_MATHS_H
#define CYNICAL_ENGINE_CPP_MATHS_H

#include "engine.h"

typedef struct transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 _matrix;
} transform_t;

glm::vec3 forward();

glm::vec3 backwards();

glm::vec3 up();

glm::vec3 down();

glm::vec3 right();

glm::vec3 left();

transform_t *create_transform();

void destroy_transform(transform_t *transform);

void update_transform_matrix(transform_t *transform);

glm::vec3 get_forward(transform_t *transform);
void set_forward(transform_t *transform, glm::vec3 forward);

glm::vec3 get_up(transform_t *transform);

#endif //CYNICAL_ENGINE_CPP_MATHS_H
