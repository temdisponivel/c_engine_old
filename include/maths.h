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

namespace math {
    transform_t *create_transform();
    void free_transform(transform_t *transform);

    void prepare_transform_for_draw(transform_t *transform);
}

#endif //CYNICAL_ENGINE_CPP_MATHS_H
