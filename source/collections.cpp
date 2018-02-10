//
// Created by temdisponivel on 09/02/2018.
//

#include <collections.h>

LIST_TEMPLATE
list<T> *lists::create(uint capacity) {
    list<T> *list = (struct list<T>*) malloc(sizeof(struct list<T>));
    list->items = (T *) malloc(sizeof(T) * capacity);
    list->capacity = capacity;
    list->length = 0;
    return list;
}

LIST_TEMPLATE
void lists::destroy(list<T> *list) {
    free(list->items);
    free(list);
}

LIST_TEMPLATE
void lists::expand(list<T> *list) {
    uint new_capacity = list->capacity * 2;
    T *items = malloc(sizeof(T) * new_capacity);
    memcpy(list->items, items, list->length * sizeof(T));
    free(list->items);
    list->items = items;
    list->capacity = new_capacity;
}

LIST_TEMPLATE
void lists::add(list<T> *list, T item) {
    if (list->length >= list->capacity - 1)
        expand(list);
    list->items[list->length++] = item;
}

LIST_TEMPLATE
void lists::remove_at(list<T> *list, uint index) {
    if (index >= list->length)
        return;

    for (int i = index; i < list->length - 1; ++i) {
        list->items[i] = list->items[i + 1];
    }

    list->length--;
}

LIST_TEMPLATE
int lists::index_of(list<T> *list, T item) {
    for (int i = 0; i < list->length; ++i) {
        T list_item = list->items[i];
        if (list_item == item)
            return i;
    }
    return -1;
}

LIST_TEMPLATE
void lists::remove(list<T> *list, T item) {
    int index_of = index_of(list, item);
    if (index_of >= 0)
        remove_at(list, index_of);
}

