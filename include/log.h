//
// Created by Morpheus on 06-Oct-17.
//

#ifndef CYNICAL_ENGINE_CYNICAL_LOG_H
#define CYNICAL_ENGINE_CYNICAL_LOG_H

#include <cstdio>
#include <csignal>
#include <cstdarg>
#include "engine.h"

#if unix
#define ERROR_COLOR   "\x1B[31m"
#define WARNING_COLOR  "\x1B[33m"
#define NORMAL_COLOR "\x1B[0m"
#elif _WIN32
#define ERROR_COLOR
#define WARNING_COLOR
#define NORMAL_COLOR
#endif

#define INT_TO_STR_2(X) #X
#define INT_TO_STR(X) INT_TO_STR_2(X)

#define LOG_HEADER(COLOR, TYPE) COLOR "[" TYPE "]: "
#define LOG_FOOTER "\n-> " __FILE__ ":" INT_TO_STR(__LINE__) "\n\n"; NORMAL_COLOR

#define LOG_MESSAGE 0
#define LOG_WARNING 1
#define LOG_ERROR 2

void MESSAGEF(const char *format, ...);

void WARNINGF(const char *format, ...);

void ERRORF(const char *format, ...);

#define FORMAT_VARARGS(format) {  \
    va_list args; \
    va_start(args, format); \
    vsnprintf(STRING_BUFF_HELPER, STRING_BUFF_HELPER_SIZE, format, args); \
    va_end(args); \
} \


#ifdef DEV

#define MESSAGE(MSG) {\
    char header[] = LOG_HEADER(NORMAL_COLOR,"MESSAGE");\
    char footer[] = LOG_FOOTER;\
    printf("%s%s%s",header, MSG, footer);\
}

#define WARNING(WARN_MESSAGE) {\
    char header[] = LOG_HEADER(WARNING_COLOR,"WARNING");\
    char footer[] = LOG_FOOTER;\
    char message[sizeof(header) + sizeof((WARN_MESSAGE)) + sizeof(footer)];\
    sprintf(message, "%s%s%s",header, (WARN_MESSAGE), footer);\
    printf(message);\
}

#define ERROR(ERR_MESSAGE) {\
    char header[] = LOG_HEADER(ERROR_COLOR,"ERROR");\
    char footer[] = LOG_FOOTER;\
    char message[sizeof(header) + sizeof((ERR_MESSAGE)) + sizeof(footer)];\
    sprintf(message, "%s%s%s",header, (ERR_MESSAGE), footer);\
    fprintf(stderr,message);\
}

#define ASSERT(RESULT) {\
    if (!(RESULT)) {\
        ERROR("Assertion failed!");\
    }\
}

#define ASSERT_BREAK(RESULT) {\
    if (!(RESULT)) {\
        ERROR("Assertion failed! Breaking into debugger!");\
        BREAK();\
    }\
}

#define ASSERT_EXIT(RESULT, EXIT_CODE) {\
    if (!(RESULT)) {\
        ERROR("Assertion failed! Exiting program!");\
        exit(EXIT_CODE);\
    }\
}

#define BREAK() __asm__("int $3")

#define LOG(MSG, TYPE) {\
    switch (TYPE) {\
        case LOG_WARNING:\
            WARNING((MSG)); \
            break;\
        case LOG_ERROR:\
            ERROR((MSG)); \
            break;\
        default:\
            MESSAGE((MSG)); \
            break;\
    }\
}

#define PRINT_GL_ERROR(MSG, ERROR)\
if (ERROR != GL_NO_ERROR){\
    switch (ERROR) {\
        case GL_INVALID_ENUM:\
            MESSAGE(MSG "GL_INVALID_ENUM");\
            break;\
        case GL_INVALID_OPERATION:\
            MESSAGE(MSG "GL_INVALID_OPERATION");\
            break;\
        case GL_INVALID_VALUE:\
            MESSAGE(MSG "GL_INVALID_VALUE");\
            break;\
    }\
}\


#define PRINT_SHADER_ERROR(SHADER) {\
    if (glIsShader(SHADER)) {\
        int info_log_length = 0;\
        int max_length = info_log_length;\
\
        glGetShaderiv(SHADER, GL_INFO_LOG_LENGTH, &max_length);\
\
        char *info_log = (char *) memcalloc((size_t) max_length, sizeof(char));\
\
        glGetShaderInfoLog(SHADER, max_length, &info_log_length, info_log);\
        if (info_log_length > 0) {\
            MESSAGE(info_log);\
        }\
\
        memfree(info_log);\
    } else {\
        MESSAGE("Name %d is not a shader\n" #SHADER);\
    }\
}\

#define PRINT_PROGRAM_ERROR(PROGRAM) {\
    if (glIsProgram(PROGRAM)) {\
        int info_log_length = 0;\
        int max_length = info_log_length;\
\
        glGetProgramiv(PROGRAM, GL_INFO_LOG_LENGTH, &max_length);\
\
        char* info_log = (char *) memcalloc((size_t) max_length, sizeof(char));\
\
        glGetProgramInfoLog(PROGRAM, max_length, &info_log_length, info_log);\
        if (info_log_length > 0) {\
            MESSAGE(info_log);\
        }\
\
        memfree(info_log);\
    } else {\
        MESSAGE("Name %d is not a program\n"#PROGRAM);\
    }\
}\

#define CHECK_SHADER_COMPILE_STATUS(SHADER, SOURCE_CODE) {\
    GLint compile_status = GL_FALSE;\
    glGetShaderiv(SHADER, GL_COMPILE_STATUS, &compile_status);\
    if (compile_status != GL_TRUE) {\
        PRINT_SHADER_ERROR((SHADER));\
        MESSAGE("Unable to compile shader!\nShader source:\n");\
        MESSAGE(#SOURCE_CODE);\
    }\
}\

#define CHECK_SHADER_LINK_STATUS(PROG_HANDLE) {\
    glLinkProgram(PROG_HANDLE);\
    GLint program_link_status = GL_TRUE;\
    glGetProgramiv(PROG_HANDLE, GL_LINK_STATUS, &program_link_status);\
    if (program_link_status != GL_TRUE) {\
        PRINT_PROGRAM_ERROR((PROG_HANDLE));\
        MESSAGE("Unable to link program!");\
    }\
}\

#define CHECK_GL_ERROR() {\
    GLenum error = glGetError();\
    if (error != GL_NO_ERROR) {\
        PRINT_GL_ERROR("Gl error: ", error);\
    }\
}\

#else

void str_format(char *buffer, uint buffer_size, const char *format, ...);
#define MESSAGE(MSG)
#define WARNING(WARN_MESSAGE)
#define ERROR(ERR_MESSAGE)
#define ASSERT(RESULT)
#define LOG(MESSAGE,TYPE)
#define ASSERT_BREAK(RESULT)
#define BREAK()
#define ASSERT_EXIT(RESULT,EXIT_CODE)
#define PRING_GL_ERROR(MSG,ERROR)
#define PRINT_SHADER_ERROR(SHADER)
#define PRINT_PROGRAM_ERROR(PROGRAM)
#define CHECK_SHADER_COMPILE_STATUS(SHADER,SOURCE_CODE)
#define CHECK_SHADER_LINK_STATUS(PROG_HANDLE)
#define CHECK_GL_ERROR()

#endif

#endif //CYNICAL_ENGINE_CYNICAL_LOG_H