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
#include "entities.h"
#include "core.h"

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

typedef glm::vec4 color_rgba_t;
// TODO: maybe we need a rgba 255 color type?

enum DEPTH_BUFFER_STATUS {
    DEPTH_BUFFER_ENABLED = GL_TRUE,
    DEPTH_BUFFER_DISABLED = GL_FALSE,
    DEPTH_TEST_DEFAULT = DEPTH_BUFFER_ENABLED,
};

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
    COMPARE_LESS = GL_LESS,
    COMPARE_LESS_OR_EQUAL = GL_LEQUAL,
    COMPARE_EQUAL = GL_EQUAL,
    COMPARE_GREATER = GL_GREATER,
    COMPARE_GREATER_OR_EQUAL = GL_GEQUAL,
    COMPARE_DIFFERENT = GL_NOTEQUAL,
    COMPARE_ALWAYS_TRUE = GL_ALWAYS,
    COMPARE_NEVER_TRUE = GL_NEVER,
    COMPARE_DEFAULT = COMPARE_DISABLED,
};

enum CULL_FUNCTIONS {
    CULL_DISABLED,
    CULL_DEFAULT = CULL_DISABLED,
    CULL_FRONT = GL_FRONT,
    CULL_BACK = GL_BACK,
    CULL_FRONT_AND_BACK = GL_FRONT_AND_BACK,
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

    UNIFORM_MATRIX,

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
    char *name;
    UNIFORM_TYPE type;
    uniform_values_union_t current_value;
} uniform_t;

enum SHADER_TYPE {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
};

typedef struct shader {
    uint handle;
    SHADER_TYPE type;
} shader_t;

typedef struct shader_program {
    uint handle;

    shader_t vertex_shader;
    shader_t fragment_shader;
    shader_t geomtry_shader;
} shader_program_t;

typedef struct material {
    shader_program_t *shader;
    list<uniform_t *> *uniforms;

    COMPARE_FUNCTIONS depth_func;
    CULL_FUNCTIONS cull_func;
} material_t;

typedef struct mesh_renderer {
    entity_t *entity;
    material_t *material;
    mesh_t *mesh;
    bool should_be_drawn;

    uint layer_mask; //TODO: improve this
} mesh_renderer_t;

typedef struct color_mask {
    bool red;
    bool green;
    bool blue;
    bool alpha;
} color_mask_t;

enum STENCIL_OP_ACTION {
    STENCIL_OP_KEEP = GL_KEEP,
    STENCIL_OP_ZERO = GL_ZERO,
    STENCIL_OP_REPLACE = GL_REPLACE,
    STENCIL_OP_INCR = GL_INCR,
    STENCIL_OP_INCR_WRAP = GL_INCR_WRAP,
    STENCIL_OP_DECR = GL_DECR,
    STENCIL_OP_DECR_WRAP = GL_DECR_WRAP,
    STENCIL_OP_INVERT = GL_INVERT,
};

typedef struct stencil_settings {
    COMPARE_FUNCTIONS compare_func;
    int reference_value;
    uint stencil_func_mask;

    STENCIL_OP_ACTION stencil_fail;
    STENCIL_OP_ACTION stencil_pass_depth_fail;
    STENCIL_OP_ACTION stencil_depth_pass;

    uint stencil_mask;
} stencil_settings_t;

typedef struct depth_settings {
    COMPARE_FUNCTIONS compare_func;
    DEPTH_BUFFER_STATUS mask;
} depth_settings_t;

enum CLEAR_MASK {
    CLEAR_COLOR = GL_COLOR_BUFFER_BIT,
    CLEAR_DEPTH = GL_DEPTH_BUFFER_BIT,
    CLEAR_STENCIL = GL_STENCIL_BUFFER_BIT,
    CLEAR_COLOR_AND_DEPTH = CLEAR_COLOR | CLEAR_DEPTH,
    CLEAR_ALL = CLEAR_COLOR_AND_DEPTH | CLEAR_STENCIL,
};

enum CAMERA_TYPE {
    ORTHO,
    PERSPECTIVE,
};

typedef struct perspective_camera {
    float field_of_view;
    float aspect_ratio;
    float near_plane;
    float far_plane;
} perspective_camera_t;

typedef struct orthogonal_camera {
    float left;
    float right;
    float bottom;
    float top;
    float near_plane;
    float far_plane;
} orthogonal_camera_t;

typedef struct view_port {
    glm::ivec2 size;
    glm::ivec2 position;
} view_port_t;

enum CAMERA_CLEAR_MODE {
    CAMERA_CLEAR_COLOR,
    CAMERA_CLEAR_DEPTH,
    CAMERA_CLEAR_STENCIL,
    CAMERA_CLEAR_COLOR_DEPTH,
    CAMERA_CLEAR_ALL, // NOTE: maybe this should be the default?!
    CAMERA_CLEAR_NONE,
    CAMERA_CLEAR_DEFAULT = CAMERA_CLEAR_COLOR_DEPTH,
};

typedef struct camera {
    entity_t *entity;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 _matrix;

    CAMERA_TYPE type;
    CAMERA_CLEAR_MODE clear_mode;

    DEPTH_BUFFER_STATUS depth_buffer_status;
    color_mask_t color_mask;
    stencil_settings_t stencil_settings;

    view_port_t view_port;
    bool full_screen;

    color_rgba_t clear_color;

    union {
        orthogonal_camera_t ortho;
        perspective_camera_t perspective;
    };

    bool enabled;

    uint culling_mask;
} camera_t;

typedef struct graphics_state {
    CULL_FUNCTIONS current_cull_func;

    depth_settings_t current_depth_settings;
    color_mask_t color_mask;
    stencil_settings_t current_stencil_config;

    uint current_shader_program;
    camera_t *current_camera;
    list<mesh_renderer_t *> *rendereres;
    list<camera_t *> *cameras;

    color_rgba_t clear_color;

    view_port_t current_view_port;
} graphics_state_t;

#define DEFAULT_COMPARE_FUNC GL_LESS

color_rgba_t black();
color_rgba_t white();
color_rgba_t red();
color_rgba_t green();
color_rgba_t blue();
color_rgba_t transparent();

void prepare_graphics();

void release_graphics();

graphics_state_t get_graphics_state();

void set_cull_func(CULL_FUNCTIONS func);

void set_vbo_enable_state(mesh_t *mesh, bool state);

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
        shader_t geomtry_shader,
        const char *vertex_position_name,
        const char *vertex_color_name,
        const char *vertex_tex_coord_name,
        const char *vertex_normal_name
);

void destroy_shader_program(shader_program_t *shader);

void create_and_add_uniform(
        material_t *material,
        uniform_definition_t uniform_def
);

material_t *create_material(
        shader_program_t *shader,
        list<uniform_definition_t> *uniform_definitions
);

void destroy_material(material_t *material);

void change_shader(material_t *material, shader_program_t *shader);

void set_uniform_bool(material_t *material, const char *name, bool value);

void set_uniform_byte(material_t *material, const char *name, byte value);

void set_uniform_ubyte(material_t *material, const char *name, ubyte value);

void set_uniform_short(material_t *material, const char *name, short value);

void set_uniform_ushort(material_t *material, const char *name, ushort value);

void set_uniform_int(material_t *material, const char *name, int value);

void set_uniform_uint(material_t *material, const char *name, uint value);

void set_uniform_long(material_t *material, const char *name, long value);

void set_uniform_float(material_t *material, const char *name, float value);

void set_uniform_double(material_t *material, const char *name, double value);

void set_uniform_vec2(material_t *material, const char *name, glm::vec2 value);

void set_uniform_vec3(material_t *material, const char *name, glm::vec3 value);

void set_uniform_vec4(material_t *material, const char *name, glm::vec4 value);

void set_uniform_matrix(material_t *material, const char *name, glm::mat4 value);

void set_uniform_texture(material_t *material, const char *name, texture_t *value);

void set_uniform_texture_property(material_t *material, const char *name, texture_material_propery_t value);

uniform_t *find_uniform_by_name(const char *name, material_t *material);

void buff_uniform(uniform_t *uniform);

void buff_uniforms(list<uniform_t *> *uniforms);

mesh_renderer_t *create_mesh_renderer(material_t *material, mesh_t *mesh);

void destroy_mesh_renderer(mesh_renderer_t *renderer);

void prepare_to_draw(mesh_renderer_t *renderer);

void use_material(material_t *material);

void draw_renderer(mesh_renderer_t *renderer);

void draw_scene();

void draw_all_renderers();

void draw_renderers(list<mesh_renderer_t *> *renderers);

camera_t *create_perspective_camera(
        float field_of_view,
        float ratio,
        float near_plane,
        float far_plane
);

camera_t *create_ortho_camera(
        float left,
        float right,
        float bottom,
        float top,
        float near_plane,
        float far_plane
);

void destroy_camera(camera_t *camera);

void update_camera_matrix(camera_t *camera);

void use_camera(camera_t *camera);

void update_cameras_view_port_to_screen_size();

view_port_t get_screen_view_port();

void set_view_port(view_port_t view_port);

void clear_current_view_port(CLEAR_MASK clear_mask);

void clear_view_port(view_port_t view_port, CLEAR_MASK clear_mask);

void set_clear_color(color_rgba_t color);

void set_depth_test_status(DEPTH_BUFFER_STATUS status);

void set_depth_func(COMPARE_FUNCTIONS functions);

void set_depth_settings(depth_settings_t settings);

void set_color_mask(color_mask_t mask);

color_mask_t get_color_mask_disabled();

void set_stencil_func(COMPARE_FUNCTIONS func, int ref_value, uint func_mask);

void set_stencil_op(STENCIL_OP_ACTION fail, STENCIL_OP_ACTION depth_fail_stencil_pass, STENCIL_OP_ACTION all_pass);

void set_stencil_mask(uint stencil_mask);

void set_stencil_settings(stencil_settings_t settings);

#endif //CYNICAL_ENGINE_CPP_GRAPHICS_H
