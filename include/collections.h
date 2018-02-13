//
// Created by temdisponivel on 09/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_COLLECTIONS_H
#define CYNICAL_ENGINE_CPP_COLLECTIONS_H

#include "engine.h"

#define LIST_TEMPLATE template<typename T>

LIST_TEMPLATE
struct list {
    T *items;
    uint length;
    uint capacity;
};

LIST_TEMPLATE
list<T> *create_list(uint capacity);

LIST_TEMPLATE
void destroy_list(list<T> *list);

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

LIST_TEMPLATE
bool null_or_empty(list<T> *list);

// IMPLEMENTATION

LIST_TEMPLATE
list<T> *create_list(uint capacity) {
    list<T> *list = (struct list<T> *) memalloc(sizeof(struct list<T>));
    list->items = (T *) memalloc(sizeof(T) * capacity);
    list->capacity = capacity;
    list->length = 0;
    return list;
}

LIST_TEMPLATE
void destroy_list(list<T> *list) {
    memfree(list->items);
    memfree(list);
}

LIST_TEMPLATE
void expand(list <T> *list) {
    uint new_capacity = list->capacity * 2;
    T *items = (T *) memalloc(sizeof(T) * new_capacity);
    memcopy(items, list->items, list->length * sizeof(T));
    memfree(list->items);
    list->items = items;
    list->capacity = new_capacity;
}

LIST_TEMPLATE
void add(list <T> *list, T item) {
    if (list->length >= list->capacity - 1)
        expand(list);
    list->items[list->length++] = item;
}

LIST_TEMPLATE
void remove_at(list <T> *list, uint index) {
    if (index >= list->length)
        return;

    for (int i = index; i < list->length - 1; ++i) {
        list->items[i] = list->items[i + 1];
    }

    list->length--;
}

LIST_TEMPLATE
int index_of(list <T> *list, T item) {
    for (int i = 0; i < list->length; ++i) {
        T list_item = list->items[i];
        if (list_item == item)
            return i;
    }
    return -1;
}

LIST_TEMPLATE
void remove(list <T> *list, T item) {
    int index = index_of(list, item);
    if (index >= 0)
        remove_at(list, index);
}

LIST_TEMPLATE
bool null_or_empty(list <T> *list) {
    return list == null || list->length == 0;
}

#endif //CYNICAL_ENGINE_CPP_COLLECTIONS_H
