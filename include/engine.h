//
// Created by temdisponivel on 09/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_ENGINE_H
#define CYNICAL_ENGINE_CPP_ENGINE_H

#define memcalloc calloc
#define memalloc malloc
#define memfree free
#define memrealloc realloc
#define memcopy memcpy

#define STBI_MALLOC memalloc
#define STBI_FREE memfree
#define STBI_REALLOC memrealloc
#define STBI_ASSERT(EXPRESSION) ASSERT(!EXPRESSION)

#define null nullptr

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <cstdlib>
#include <log.h>
#include <stb_image.h>
#include <memory.h>

typedef int8_t byte;

typedef uint8_t ubyte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t ulong;

#define STRING_BUFF_HELPER_SIZE 2048
static char STRING_BUFF_HELPER[2048];


#endif //CYNICAL_ENGINE_CPP_ENGINE_H
