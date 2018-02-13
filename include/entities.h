//
// Created by temdisponivel on 13/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_ENTITIES_H
#define CYNICAL_ENGINE_CPP_ENTITIES_H

#include "maths.h"

enum BUILT_IN_ENTITIES {
    MESH_RENDERER,
    CAMERA,
};

typedef struct entity {
    transform_t *transform;
    uint entity_type;
    void *entity_ref;
} entity_t;

entity_t *create_entity(uint type, void *entity_ref);
void destroy_entity(entity_t *entity);

#endif //CYNICAL_ENGINE_CPP_ENTITIES_H
