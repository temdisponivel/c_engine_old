//
// Created by temdisponivel on 09/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_GRAPHICS_H
#define CYNICAL_ENGINE_CPP_GRAPHICS_H

#include <engine.h>
#include "math_helper.h"
#include "collections.h"

#define VERTEX_POSITION_ATTRIBUTE_INDEX 0
#define VERTEX_COLOR_ATTRIBUTE_INDEX 1
#define VERTEX_NORMAL_ATTRIBUTE_INDEX 2
#define VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX 3

#define VERTEX_POSITION_ATTRIBUTE_NAME "vertex_position"
#define VERTEX_COLOR_ATTRIBUTE_NAME "vertex_color"
#define VERTEX_NORMAL_ATTRIBUTE_NAME "vertex_normal"
#define VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME "vertex_texture_coord"

#define TEXTURE_0 TEXTURE_0
#define TEXTURE_1 TEXTURE_1
#define TEXTURE_2 TEXTURE_2
#define TEXTURE_3 TEXTURE_3
#define TEXTURE_4 TEXTURE_4
#define TEXTURE_5 TEXTURE_5
#define TEXTURE_6 TEXTURE_6
#define TEXTURE_7 TEXTURE_7
#define TEXTURE_8 TEXTURE_8
#define TEXTURE_9 TEXTURE_9



typedef struct mesh {
    uint vao_handle;

    uint vertex_position_handle;
    uint vertex_color_handle;
    uint vertex_tex_coord_handle;
    uint vertex_normal_handle;

    uint indices_handle;
} mesh_t;

typedef struct model {
    list<float> *vertices;
    list<float> *colors;
    list<float> *tex_coords;
    list<float> *normals;
} model_t;

typedef struct image {
    void *data;
    glm::vec2 size;
    uint channels;
} image_t;

typedef struct texture_config {
    uint depth_stencil_texture_mode;
    uint texture_base_level;
    uint texture_compare_func;
    uint texture_compare_mode;
    uint texture_lod_bias;
    uint texture_min_filter;
    uint texture_mag_filter;
    uint texture_min_lod;
    uint texture_max_lod;
    uint texture_max_level;
    uint texture_swizzle_r;
    uint texture_swizzle_g;
    uint texture_swizzle_b;
    uint texture_swizzle_a;
    uint texture_wrap_s;
    uint texture_wrap_t;
    uint texture_wrap_r;
} texture_config_t;

typedef struct texture {
    uint handle;
    image_t *image;
    texture_config_t config;
} texture_t;

typedef struct texture_material_property {
    texture_t *texture;
    uint texture_target_index;
} texture_material_propery_t;

enum UNIFORM_TYPE {
    UNIFORM_BOOLEAN,
    UNIFORM_BYTE,
    UNIFORM_UBYTE,
    UNIFORM_SHORT,
    UNIFORM_USHORT,
    UNIFORM_INT,
    UNIFORM_UINT,
    UNIFORM_LONG,
    UNIFORM_FLOAT,
    UNIFORM_DOUBLE,

    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,

    UNIFORM_MAT4,

    UNIFORM_TEXTURE2D,
};

typedef union uniform_values_union {
    bool bool_value;
    byte byte_value;
    ubyte ubyte_value;
    short short_value;
    ushort ushort_value;
    int int_value;
    uint uint_value;
    long long_value;
    float float_value;
    double double_value;
    glm::vec2 vector2_value;
    glm::vec3 vector3_value;
    glm::vec4 vector4_value;
    glm::mat4 matrix_value;
    texture_material_propery_t texture_value;
} uniform_values_union_t;

typedef struct uniform_definition {
    std::string *name;
    UNIFORM_TYPE type;
    uniform_values_union_t default_value;
} uniform_definition_t;

typedef struct uniform {
    uint handle;
    std::string *name;
    UNIFORM_TYPE type;
    uniform_values_union_t current_value;
} uniform_t;

enum SHADER_TYPE {
    VERTEX_SHADER,
    FRAGMENT_SHADER,
};

typedef struct shader {
    uint handle;
    SHADER_TYPE type;
} shader_t;

typedef struct shader_program {
    uint handle;

    shader_t vertex_shader;
    shader_t fragment_shader;
} shader_program_t;

typedef struct material_definition {
    list<uniform_definition_t> *uniforms_definitions;
} material_defition_t;

typedef struct material_instance {
    shader_program_t *shader;
    list<uniform_t> *uniforms;
} material_instance_t;

#endif //CYNICAL_ENGINE_CPP_GRAPHICS_H
