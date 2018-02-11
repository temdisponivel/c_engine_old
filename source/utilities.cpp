//
// Created by temdisponivel on 10/02/2018.
//

#include "utilities.h"

char *read_file_text(const char *file_path) {
    FILE *file = fopen(file_path, "rb");

    ENSURE(file != null);

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    char *buffer = (char *) memalloc(sizeof(char) * length);
    fseek(file, 0, SEEK_SET);
    fread(buffer, (uint) length, 1, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

void free_file_text(char *text) {
    memfree(text);
}

int hash(char *str) {
    int hash = 0;
    char c;
    while ((c = *str++) != '\0') {
        hash += c;
    }
    return hash;
}