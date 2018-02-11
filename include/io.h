//
// Created by temdisponivel on 10/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_IO_H
#define CYNICAL_ENGINE_CPP_IO_H

#include "engine.h"
#include <cstdio>

char *read_file_text(const char *file_path);
void free_file_text(char *text);

#endif //CYNICAL_ENGINE_CPP_IO_H
