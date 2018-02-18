//
// Created by temdisponivel on 10/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_UTILITIES_H
#define CYNICAL_ENGINE_CPP_UTILITIES_H

#include "engine.h"
#include <cstdio>

int hash(char *str);
char *read_file_text(const char *file_path);
char *read_file_content(const char *file_path, long *length, bool is_string);
void free_file_content(char *content);

#endif //CYNICAL_ENGINE_CPP_UTILITIES_H
