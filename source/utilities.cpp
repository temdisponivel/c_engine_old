//
// Created by temdisponivel on 10/02/2018.
//

#include "utilities.h"

char *read_file_text(const char *file_path) {
    long len;
    return read_file_content(file_path, &len, true);
}

char *read_file_content(const char *file_path, long *length, bool is_string) {
    FILE *file = fopen(file_path, "rb");

    ENSURE(file != null);

    fseek(file, 0, SEEK_END);
    *length = ftell(file);

    long total_length = *length;
    if (is_string)
        total_length += 1;

    char *buffer = (char *) memalloc(sizeof(char) * (total_length + 1));
    fseek(file, 0, SEEK_SET);
    fread(buffer, (uint) length, 1, file);

    if (is_string)
        buffer[*length] = '\0';

    fclose(file);

    return buffer;
}

void free_file_content(char *content) {
    memfree(content);
}

int hash(char *str) {
    int hash = 0;
    char c;
    while ((c = *str++) != '\0') {
        hash += c;
    }
    return hash;
}