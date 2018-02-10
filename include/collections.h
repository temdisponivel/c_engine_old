//
// Created by temdisponivel on 09/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_COLLECTIONS_H
#define CYNICAL_ENGINE_CPP_COLLECTIONS_H

#include <cstdlib>
#include <cstring>
#include "math_helper.h"

#define LIST_TEMPLATE template<typename T>

LIST_TEMPLATE
struct list {
    T *items;
    uint length;
    uint capacity;
};

namespace lists {

    LIST_TEMPLATE
    list<T> *create(uint capacity);

    LIST_TEMPLATE
    void destroy(list<T> *list);

    LIST_TEMPLATE
    void expand(list<T> *list);

    LIST_TEMPLATE
    void add(list<T> *list, T item);

    LIST_TEMPLATE
    void remove_at(list<T> *list, uint index);

    LIST_TEMPLATE
    int index_of(list<T> *list, T item);

    LIST_TEMPLATE
    void remove(list<T> *list, T item);
}

#endif //CYNICAL_ENGINE_CPP_COLLECTIONS_H
