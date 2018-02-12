//
// Created by temdisponivel on 09/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_GRAPHICS_H
#define CYNICAL_ENGINE_CPP_GRAPHICS_H

#include <engine.h>
#include <utilities.h>
#include "collections.h"
#include "stb_image.h"
#include <collections.h>

#define NONE 0

#define VERTEX_POSITION_ATTRIBUTE_INDEX 0
#define VERTEX_COLOR_ATTRIBUTE_INDEX 1
#define VERTEX_NORMAL_ATTRIBUTE_INDEX 2
#define VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX 3

#define VERTEX_POSITION_ATTRIBUTE_NAME "vertex_position"
#define VERTEX_COLOR_ATTRIBUTE_NAME "vertex_color"
#define VERTEX_NORMAL_ATTRIBUTE_NAME "vertex_normal"
#define VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME "vertex_texture_coord"

#define TEXTURE_0 GL_TEXTURE0
#define TEXTURE_1 GL_TEXTURE1
#define TEXTURE_2 GL_TEXTURE2
#define TEXTURE_3 GL_TEXTURE3
#define TEXTURE_4 GL_TEXTURE4
#define TEXTURE_5 GL_TEXTURE5
#define TEXTURE_6 GL_TEXTURE6
#define TEXTURE_7 GL_TEXTURE7
#define TEXTURE_9 GL_TEXTURE8
#define TEXTURE_8 GL_TEXTURE9

typedef struct model {
    list<int> *indices;
    list<float> *positions;
    list<float> *colors;
    list<float> *tex_coords;
    list<float> *normals;
} model_t;

typedef struct mesh {
    uint vao_handle;

    uint vertex_position_handle;
    uint vertex_color_handle;
    uint vertex_tex_coord_handle;
    uint vertex_normal_handle;

    uint indices_handle;

    model_t *model;
} mesh_t;

enum IMAGE_CHANNELS {
    RGB = 3,
    RGBA = 4,
};

typedef struct image {
    void *data;
    glm::vec2 size;
    IMAGE_CHANNELS channels;
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
    char *name;
    UNIFORM_TYPE type;
    uniform_values_union_t default_value;
} uniform_definition_t;

typedef struct uniform {
    int handle;
    int name_hash;
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

typedef struct material {
    shader_program_t *shader;
    list<uniform_t*> *uniforms;
} material_t;

namespace gl {
    image_t *create_image(const char *image_file_path);
    void destroy_image(image_t *image);

    texture_t *create_texture(image_t *image, texture_config_t config);
    void destroy_texture(texture_t *texture);
    texture_config_t get_default_texture_config();
    void buff_texture_config_to_gl(texture_t *texture);

    model_t *create_model(const char *model_file_path);
    void destroy_model(model_t *model);

    mesh_t *create_mesh(model_t *model);
    void destroy_mesh(mesh_t *mesh);

    shader_t *create_shader(const char *shader_code, SHADER_TYPE type);
    void destroy_shader(shader_t *shader);

    shader_program_t *create_shader_program(
            shader_t vertex_shader,
            shader_t fragment_shader,
            const char *vertex_position_name,
            const char *vertex_color_name,
            const char *vertex_tex_coord_name,
            const char *vertex_normal_name
    );

    void destroy_shader_program(shader_program_t *shader);

    material_t *create_material(
            shader_program_t *shader,
            list<uniform_definition_t> *uniform_definitions
    );
    void destroy_material(material_t *material);

    uniform_t *find_uniform_by_name(const char *name, material_t *material);

    void buff_uniform(uniform_t *uniform);
    void buff_uniforms(list<uniform_t*> *uniforms);

    void use_material(material_t *material);
    void draw_mesh(mesh_t *mesh);
}

#endif //CYNICAL_ENGINE_CPP_GRAPHICS_H
