//
// Created by temdisponivel on 10/02/2018.
//

#include "log.h"

#if DEV

void MESSAGEF(const char *format, ...) {
    FORMAT_VARARGS(format);
    MESSAGE(STRING_BUFF_HELPER);
}

void WARNINGF(const char *format, ...) {
    FORMAT_VARARGS(format);
    WARNING(STRING_BUFF_HELPER);
}

void ERRORF(const char *format, ...) {
    FORMAT_VARARGS(format);
    ERROR(STRING_BUFF_HELPER);
}

#endif