//
// Created by temdisponivel on 13/02/2018.
//

#include "entities.h"

entity_t *create_entity(uint type, void *entity_ref) {
    ENSURE(entity_ref != null);
    entity_t *entity = (entity_t *) memalloc(sizeof(entity_t));
    entity->transform = create_transform();
    entity->entity_type = type;
    entity->entity_ref = entity_ref;
    return entity;
}

void destroy_entity(entity_t *entity) {
    destroy_transform(entity->transform);
    memfree(entity);
}