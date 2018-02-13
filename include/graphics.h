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

#define HANDLE_NONE 0

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

enum TEXTURE_DEPTH_STENCIAL_VALUES {
    DEPTH_STENCIL_DEFAULT,
    DEPTH_STENCIL_DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    DEPTH_STENCIL_STENCIL_COMPONENT = GL_STENCIL_COMPONENTS,
};

enum COMPARE_FUNCTIONS {
    COMPARE_DISABLED,
    COMPARE_DEFAULT = COMPARE_DISABLED,
    COMPARE_LESS = GL_LESS,
    COMPARE_LESS_OR_EQUAL = GL_LEQUAL,
    COMPARE_EQUAL = GL_EQUAL,
    COMPARE_GREATER = GL_GREATER,
    COMPARE_GREATER_OR_EQUAL = GL_GEQUAL,
    COMPARE_DIFFERENT = GL_NOTEQUAL,
    COMPARE_ALWAYS = GL_ALWAYS,
    COMPARE_NEVER = GL_NEVER,
};

enum TEXTURE_COMPARE_MODE {
    TEX_COMPARE_DEFAULT,
    TEX_COMPARE_COMPARE_REF_TO_TEXTURE = GL_COMPARE_REF_TO_TEXTURE,
    TEX_COMPARE_NONE = GL_NONE
};

enum TEXTURE_MIN_FILTER_VALUES {
    TEX_MIN_FILTER_DEFAULT,
    TEX_MIN_FILTER_NEAREST = GL_NEAREST,
    TEX_MIN_FILTER_LINEAR = GL_LINEAR,
    TEX_MIN_FILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    TEX_MIN_FILTER_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    TEX_MIN_FILTER_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    TEX_MIN_FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

enum TEXTURE_MAG_FILTER_VALUES {
    TEX_MAG_FILTER_DEFAULT,
    TEX_MAG_FILTER_NEAREST = GL_NEAREST,
    TEX_MAG_FILTER_LINEAR = GL_LINEAR,
};

enum TEXTURE_SWIZZLE_VALUES {
    TEX_SWIZZLE_DEFAULT,
    TEX_SWIZZLE_RED = GL_RED,
    TEX_SWIZZLE_GREEN = GL_GREEN,
    TEX_SWIZZLE_BLUE = GL_BLUE,
    TEX_SWIZZLE_ALPHA = GL_ALPHA,
    TEX_SWIZZLE_ZERO = GL_ZERO,
    TEX_SWIZZLE_ONE = GL_ONE
};

enum TEXTURE_WRAP_VALUES {
    TEX_WRAP_DEFAULT,
    TEX_WRAP_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    TEX_WRAP_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
    TEX_WRAP_MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    TEX_WRAP_REPEAT = GL_REPEAT,
    TEX_WRAP_MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE,
};

typedef struct texture_config {
    TEXTURE_DEPTH_STENCIAL_VALUES depth_stencil_texture_mode;
    uint texture_base_level;
    glm::vec4 texture_border_color;
    COMPARE_FUNCTIONS texture_compare_func;
    TEXTURE_COMPARE_MODE texture_compare_mode;
    int texture_lod_bias;
    TEXTURE_MIN_FILTER_VALUES texture_min_filter;
    TEXTURE_MAG_FILTER_VALUES texture_mag_filter;
    float texture_min_lod;
    float texture_max_lod;
    int texture_max_level;
    TEXTURE_SWIZZLE_VALUES texture_swizzle_r;
    TEXTURE_SWIZZLE_VALUES texture_swizzle_g;
    TEXTURE_SWIZZLE_VALUES texture_swizzle_b;
    TEXTURE_SWIZZLE_VALUES texture_swizzle_a;
    TEXTURE_SWIZZLE_VALUES texture_swizzle_rgba;
    TEXTURE_WRAP_VALUES texture_wrap_s;
    TEXTURE_WRAP_VALUES texture_wrap_t;
    TEXTURE_WRAP_VALUES texture_wrap_r;
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

    COMPARE_FUNCTIONS depth_func;
} material_t;

typedef struct graphics_state {
    COMPARE_FUNCTIONS current_depth_func;
    uint current_shader_program;
} graphics_state_t;

#define DEFAULT_COMPARE_FUNC GL_LESS

namespace gl {
    graphics_state_t get_graphics_state();
    void set_depth_func(COMPARE_FUNCTIONS func);

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
            COMPARE_FUNCTIONS depth_func,
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
