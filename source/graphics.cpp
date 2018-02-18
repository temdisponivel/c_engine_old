//
// Created by temdisponivel on 10/02/2018.
//

#include "graphics.h"
#include <log.h>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

bool has_gl_error() {
    GLenum error = glGetError();
    return error != GL_NO_ERROR;
}

void get_texture_gl_formats(image_t *image, uint *internal_format, uint *format, uint *pixel_type) {
    switch (image->format) {
        case TEXTURE_RGB:
            *internal_format = TEXTURE_RGB;
            *format = TEXTURE_RGB;
            *pixel_type = GL_UNSIGNED_BYTE;

            break;
        case TEXTURE_RGBA:
            *internal_format = TEXTURE_RGBA;
            *format = TEXTURE_RGBA;
            *pixel_type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_DEPTH:
            *internal_format = GL_DEPTH24_STENCIL8;
            *format = GL_DEPTH_STENCIL;
            *pixel_type = GL_UNSIGNED_INT_24_8;
            break;
    }
}

image_t *create_image_full(
        void *data,
        TEXTURE_FORMAT format,
        glm::vec2 resolution
) {
    image_t *image = (image_t *) memalloc(sizeof(image_t));
    image->data = data;
    image->format = format;
    image->resolution = resolution;
    return image;
}

image_t *create_image(const char *image_file_path) {
    int width, height, channels;
    stbi_uc *data = stbi_load(image_file_path, &width, &height, &channels, 4 /* TEXTURE_FORMAT::TEXTURE_RGBA */);

#if DEV
    if (data == null) {
        ERRORF("IMAGE '%s' COULDN'T BE LOADED!", image_file_path);
    }

    if (channels < 3 || channels > 4) {
        WARNINGF("THE IMAGE '%s' has %i format! We only support 3 and 4.", image_file_path, channels);
    }
#endif

    TEXTURE_FORMAT format = TEXTURE_FORMAT::TEXTURE_RGB;
    if (channels == 4)
        format = TEXTURE_FORMAT::TEXTURE_RGBA;

    image_t *image = create_image_full(data, format, glm::vec2(width, height));

    // NOTE: We could, in theory, release the image content right after uploading to opengl

    return image;
}

void destroy_image(image_t *image) {
    // Image sound_data is null when the image was created for a FBO
    if (image->data != null) {
        stbi_image_free(image->data);
    }

    memfree(image);
}

texture_t *create_texture(image_t *image, texture_config_t config) {
    uint texture_handle;

    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    CHECK_GL_ERROR();

    uint internal_format;
    uint format;
    uint pixel_type;
    get_texture_gl_formats(image, &internal_format, &format, &pixel_type);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal_format,
            (uint) image->resolution.x,
            (uint) image->resolution.y,
            0,
            format,
            pixel_type,
            image->data
    );
    CHECK_GL_ERROR();

    glBindTexture(GL_TEXTURE_2D, HANDLE_NONE);
    CHECK_GL_ERROR();

    texture_t *texture = (texture_t *) memalloc(sizeof(texture_t));

    texture->handle = texture_handle;
    texture->config = config;
    texture->image = image;

    buff_texture_config_to_gl(texture);

    return texture;
}

void destroy_texture(texture_t *texture) {
    glDeleteTextures(1, &texture->handle);
    memfree(texture);
}

texture_config_t get_default_texture_config() {
    texture_config_t default_config = {};

    default_config.texture_wrap_r = TEX_WRAP_MIRRORED_REPEAT;
    default_config.texture_wrap_s = TEX_WRAP_MIRRORED_REPEAT;
    default_config.texture_wrap_t = TEX_WRAP_MIRRORED_REPEAT;

    default_config.texture_min_filter = TEX_MIN_FILTER_NEAREST;
    default_config.texture_mag_filter = TEX_MAG_FILTER_NEAREST;

    return default_config;
}

void buff_texture_config_to_gl(texture_t *texture) {
    texture_config_t config = texture->config;

    glBindTexture(GL_TEXTURE_2D, texture->handle);

    if (config.depth_stencil_texture_mode != DEPTH_STENCIL_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, config.depth_stencil_texture_mode);
        CHECK_GL_ERROR();
    }

    if (config.texture_base_level > 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, config.texture_base_level);
        CHECK_GL_ERROR();
    }

    if (config.texture_border_color.length() > 0) {
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(config.texture_border_color));
        CHECK_GL_ERROR();
    }

    if (config.texture_compare_func != COMPARE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, config.texture_compare_func);
        CHECK_GL_ERROR();
    }

    if (config.texture_compare_mode != TEX_COMPARE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, config.texture_compare_mode);
        CHECK_GL_ERROR();
    }

    if (config.texture_min_filter != TEX_MIN_FILTER_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.texture_min_filter);
        CHECK_GL_ERROR();
    }

    if (config.texture_mag_filter != TEX_MAG_FILTER_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.texture_mag_filter);
        CHECK_GL_ERROR();
    }

    if (config.texture_lod_bias > 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, config.texture_lod_bias);
        CHECK_GL_ERROR();
    }

    if (config.texture_min_lod > 0) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, config.texture_min_lod);
        CHECK_GL_ERROR();
    }

    if (config.texture_max_lod > 0) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, config.texture_max_lod);
        CHECK_GL_ERROR();
    }

    if (config.texture_max_level > 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, config.texture_max_level);
        CHECK_GL_ERROR();
    }

    if (config.texture_swizzle_r != TEX_SWIZZLE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, config.texture_swizzle_r);
        CHECK_GL_ERROR();
    }

    if (config.texture_swizzle_g != TEX_SWIZZLE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, config.texture_swizzle_g);
        CHECK_GL_ERROR();
    }

    if (config.texture_swizzle_b != TEX_SWIZZLE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, config.texture_swizzle_b);
        CHECK_GL_ERROR();
    }

    if (config.texture_swizzle_a != TEX_SWIZZLE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, config.texture_swizzle_a);
        CHECK_GL_ERROR();
    }

    if (config.texture_swizzle_rgba != TEX_SWIZZLE_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, config.texture_swizzle_rgba);
        CHECK_GL_ERROR();
    }

    if (config.texture_wrap_s != TEX_WRAP_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, config.texture_wrap_s);
        CHECK_GL_ERROR();
    }

    if (config.texture_wrap_t != TEX_WRAP_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, config.texture_wrap_t);
        CHECK_GL_ERROR();
    }

    if (config.texture_wrap_r != TEX_WRAP_DEFAULT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, config.texture_wrap_r);
        CHECK_GL_ERROR();
    }

    glBindTexture(GL_TEXTURE_2D, HANDLE_NONE);
}

model_t *create_model(const char *model_file_path) {
    // TODO: optimize this!!!

    list<float> *positions = create_list<float>(256);
    list<float> *colors = create_list<float>(256);
    list<float> *tex_coords = create_list<float>(256);
    list<float> *normals = create_list<float>(256);
    list<int> *indices = create_list<int>(256);

    const char line_buffer[256] = {};

    FILE *file = fopen(model_file_path, "rb");
    ENSURE(file != null);

    while (fscanf(file, "%s", line_buffer) != EOF) {
        if (strcmp(line_buffer, "vp") == 0) {
            float x, y, z;
            if (fscanf(file, "%f %f %f\n", &x, &y, &z)) {
                add<float>(positions, x);
                add<float>(positions, y);
                add<float>(positions, z);
            }
        } else if (strcmp(line_buffer, "vc") == 0) {
            float r, g, b;
            if (fscanf(file, "%f %f %f\n", &r, &g, &b)) {
                add<float>(colors, r);
                add<float>(colors, g);
                add<float>(colors, b);
            }
        } else if (strcmp(line_buffer, "vt") == 0) {
            float x, y;
            if (fscanf(file, "%f %f\n", &x, &y)) {
                add<float>(tex_coords, x);
                add<float>(tex_coords, y);
            }
        } else if (strcmp(line_buffer, "vn") == 0) {
            float x, y, z;
            if (fscanf(file, "%f %f %f\n", &x, &y, &z)) {
                add<float>(normals, x);
                add<float>(normals, y);
                add<float>(normals, z);
            }
        } else if (strcmp(line_buffer, "vi") == 0) {
            int indice;
            if (fscanf(file, "%i\n", &indice)) {
                add<int>(indices, indice);
            }
        }
    }
    fclose(file);

    ENSURE(positions->length > 0);

    model_t *model = (model_t *) memalloc(sizeof(model_t));

    model->positions = positions;
    model->colors = colors;
    model->tex_coords = tex_coords;
    model->normals = normals;
    model->indices = indices;

    return model;
}

void destroy_model(model_t *mesh) {
    destroy_list(mesh->positions);
    destroy_list(mesh->colors);
    destroy_list(mesh->tex_coords);
    destroy_list(mesh->normals);
    destroy_list(mesh->indices);

    memfree(mesh);
}

void buff_vbo(
        uint handle,
        uint array_index,
        void *data,
        uint ptr_size,
        uint dimention,
        uint gl_data_type,
        GLboolean normalized,
        uint stride,
        void *offset,
        GLenum usage
) {
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    CHECK_GL_ERROR();

    glBufferData(
            GL_ARRAY_BUFFER,
            ptr_size,
            data,
            usage
    );
    CHECK_GL_ERROR();

    glVertexAttribPointer(
            array_index,
            dimention,
            gl_data_type,
            normalized,
            stride,
            offset
    );
    CHECK_GL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, HANDLE_NONE);
    CHECK_GL_ERROR();
}

mesh_t *create_mesh(model_t *model) {
    uint vao;

    uint indices_vbo = 0;
    uint position_vbo = 0;
    uint color_vbo = 0;
    uint tex_coord_vbo = 0;
    uint normal_vbo = 0;

    glGenVertexArrays(1, &vao);
    CHECK_GL_ERROR();

    glBindVertexArray(vao);
    CHECK_GL_ERROR();

    // TODO: move this fill vbo to a separate functions so we can update the mesh's data on-demand

    if (!null_or_empty(model->indices)) {
        glGenBuffers(1, &indices_vbo);
        CHECK_GL_ERROR();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
        CHECK_GL_ERROR();

        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                model->indices->length * sizeof(GLint),
                model->indices->items,
                GL_STATIC_DRAW
        );
        CHECK_GL_ERROR();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HANDLE_NONE);
        CHECK_GL_ERROR();
    }

    if (!null_or_empty(model->positions)) {
        glGenBuffers(1, &position_vbo);
        CHECK_GL_ERROR();

        buff_vbo(
                position_vbo,
                VERTEX_POSITION_ATTRIBUTE_INDEX,
                model->positions->items,
                model->positions->length * sizeof(GLfloat),
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                null,
                GL_STATIC_DRAW
        );
    }

    if (!null_or_empty(model->colors)) {
        glGenBuffers(1, &color_vbo);
        CHECK_GL_ERROR();

        buff_vbo(
                color_vbo,
                VERTEX_COLOR_ATTRIBUTE_INDEX,
                model->colors->items,
                model->colors->length * sizeof(GLfloat),
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                null,
                GL_STATIC_DRAW
        );
    }

    if (!null_or_empty(model->tex_coords)) {
        glGenBuffers(1, &tex_coord_vbo);
        CHECK_GL_ERROR();

        buff_vbo(
                tex_coord_vbo,
                VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX,
                model->tex_coords->items,
                model->tex_coords->length * sizeof(GLfloat),
                2,
                GL_FLOAT,
                GL_FALSE,
                0,
                null,
                GL_STATIC_DRAW
        );
    }

    if (!null_or_empty(model->normals)) {
        glGenBuffers(1, &normal_vbo);
        CHECK_GL_ERROR();

        buff_vbo(
                normal_vbo,
                VERTEX_NORMAL_ATTRIBUTE_INDEX,
                model->normals->items,
                model->normals->length * sizeof(GLfloat),
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                null,
                GL_STATIC_DRAW
        );
    }

    glBindVertexArray(HANDLE_NONE);

    mesh_t *mesh = (mesh_t *) memalloc(sizeof(mesh_t));

    mesh->model = model;

    mesh->vao_handle = vao;
    mesh->indices_handle = indices_vbo;
    mesh->vertex_position_handle = position_vbo;
    mesh->vertex_color_handle = color_vbo;
    mesh->vertex_tex_coord_handle = tex_coord_vbo;
    mesh->vertex_normal_handle = normal_vbo;

    return mesh;
}

void destroy_mesh(mesh_t *mesh) {
    if (mesh->indices_handle > 0) {
        glDeleteBuffers(1, &mesh->indices_handle);
        CHECK_GL_ERROR();
    }

    if (mesh->vertex_position_handle > 0) {
        glDeleteBuffers(1, &mesh->vertex_position_handle);
        CHECK_GL_ERROR();
    }

    if (mesh->vertex_color_handle > 0) {
        glDeleteBuffers(1, &mesh->vertex_color_handle);
        CHECK_GL_ERROR();
    }

    if (mesh->vertex_tex_coord_handle > 0) {
        glDeleteBuffers(1, &mesh->vertex_tex_coord_handle);
        CHECK_GL_ERROR();
    }

    if (mesh->vertex_normal_handle > 0) {
        glDeleteBuffers(1, &mesh->vertex_normal_handle);
        CHECK_GL_ERROR();
    }

    glDeleteVertexArrays(1, &mesh->vao_handle);
    CHECK_GL_ERROR();

    memfree(mesh);
}

shader_t *create_shader(
        const char *shader_code,
        SHADER_TYPE type
) {
    ENSURE(shader_code != null);
    ENSURE(strlen(shader_code) > 0);

    uint handle = glCreateShader(type);
    CHECK_GL_ERROR();

    glShaderSource(handle, 1, &shader_code, 0);
    CHECK_GL_ERROR();

    glCompileShader(handle);
    CHECK_GL_ERROR();

    CHECK_SHADER_COMPILE_STATUS(handle, shader_code);

    shader_t *shader = (shader_t *) memalloc(sizeof(shader_t));
    shader->handle = handle;
    shader->type = type;
    return shader;
}

void destroy_shader(shader_t *shader) {
    glDeleteShader(shader->handle);
    CHECK_GL_ERROR();

    memfree(shader);
}

shader_program_t *create_shader_program(
        shader_t vertex_shader,
        shader_t fragment_shader,
        shader_t geometry_shader,
        const char *vertex_position_name,
        const char *vertex_color_name,
        const char *vertex_tex_coord_name,
        const char *vertex_normal_name
) {
    uint handle = glCreateProgram();
    CHECK_GL_ERROR();

    glAttachShader(handle, vertex_shader.handle);
    CHECK_GL_ERROR();

    glAttachShader(handle, fragment_shader.handle);
    CHECK_GL_ERROR();

    // Shader programs can have no geometry shader objects
    if (geometry_shader.handle > 0) {
        glAttachShader(handle, geometry_shader.handle);
        CHECK_GL_ERROR();
    }

    if (vertex_position_name != null)
        glBindAttribLocation(handle, VERTEX_POSITION_ATTRIBUTE_INDEX, vertex_position_name);

    if (vertex_color_name != null)
        glBindAttribLocation(handle, VERTEX_COLOR_ATTRIBUTE_INDEX, vertex_color_name);

    if (vertex_tex_coord_name != null)
        glBindAttribLocation(handle, VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX, vertex_tex_coord_name);

    if (vertex_normal_name != null)
        glBindAttribLocation(handle, VERTEX_NORMAL_ATTRIBUTE_INDEX, vertex_normal_name);

    CHECK_GL_ERROR();

    glLinkProgram(handle);

    CHECK_SHADER_LINK_STATUS(handle);

    shader_program_t *program = (shader_program_t *) memalloc(sizeof(shader_program_t));

    program->handle = handle;
    program->vertex_shader = vertex_shader;
    program->fragment_shader = fragment_shader;
    program->geomtry_shader = geometry_shader;

    return program;
}

void destroy_shader_program(shader_program_t *shader) {
    glDeleteProgram(shader->handle);
    CHECK_GL_ERROR();

    memfree(shader);
}

int get_uniform_location(uint shader_program, const char *uniform_name) {
    ENSURE(uniform_name != null);

    int handle = glGetUniformLocation(shader_program, uniform_name);
    CHECK_GL_ERROR();

    return handle;
}

void create_and_add_uniform(
        material_t *material,
        uniform_definition_t uniform_def
) {
    int handle = get_uniform_location(material->shader->handle, uniform_def.name);

    if (handle < 0) {
        WARNINGF("UNIFORM '%s' NOT FOUND! IT IS EITHER UNDECLARED OR UNUSED!" FILE_LINE, uniform_def.name);
    } else {
        // Only create if it doesn't exists
        uniform_t *existing_uniform = find_uniform_by_name(uniform_def.name, material);
        if (existing_uniform == null) {
            uniform_t *uniform = (uniform_t *) memalloc(sizeof(uniform_t));

            uniform->handle = handle;
            uniform->name_hash = hash(uniform_def.name);
            uniform->type = uniform_def.type;
            uniform->current_value = uniform_def.default_value;

            char *name = (char *) memalloc(strlen(uniform_def.name) * sizeof(char));
            memcopy(name, uniform_def.name, strlen(uniform_def.name) * sizeof(char));

            uniform->name = name;

            add(material->uniforms, uniform);
        } else {
            if (existing_uniform->type != uniform_def.type) {
                MESSAGEF(
                        "Attempt to recreate uniform '%s'. The previous type were: '%s', the new type is: '%s'.",
                        uniform_def.name,
                        existing_uniform->type,
                        uniform_def.type
                );
            }
        }
    }
}

material_t *create_material(
        shader_program_t *shader,
        list<uniform_definition_t> *uniform_definitions
) {
    material_t *material = (material_t *) memalloc(sizeof(material_t));
    list<uniform_t *> *uniforms = create_list<uniform_t *>(uniform_definitions->length);

    material->shader = shader;
    material->depth_func = COMPARE_FUNCTIONS::COMPARE_DEFAULT;
    material->cull_func = CULL_FUNCTIONS::CULL_DEFAULT;
    material->shader = shader;
    material->uniforms = uniforms;

    for (int i = 0; i < uniform_definitions->length; ++i) {
        uniform_definition_t uniform_def = uniform_definitions->items[i];
        create_and_add_uniform(material, uniform_def);
    }

    // Try to create default uniforms for the uniforms that the engine supplies regardless of declaration

    uniform_definition_t matrices_defition = {};
    matrices_defition.type = UNIFORM_TYPE::UNIFORM_MATRIX;
    matrices_defition.default_value.matrix_value = glm::mat4();

    matrices_defition.name = (char *) "MVP";
    create_and_add_uniform(material, matrices_defition);

    matrices_defition.name = (char *) "PROJECTION";
    create_and_add_uniform(material, matrices_defition);

    matrices_defition.name = (char *) "VIEW";
    create_and_add_uniform(material, matrices_defition);

    matrices_defition.name = (char *) "VP";
    create_and_add_uniform(material, matrices_defition);

    matrices_defition.name = (char *) "MODEL";
    create_and_add_uniform(material, matrices_defition);

    return material;
}

void destroy_material(material_t *material) {
    for (int i = 0; i < material->uniforms->length; ++i) {
        uniform_t *uni = material->uniforms->items[i];
        memfree(uni->name);
        memfree(uni);
    }
    destroy_list(material->uniforms);
    memfree(material);
}

void change_shader(material_t *material, shader_program_t *shader) {
    material->shader = shader;
    for (int i = 0; i < material->uniforms->length; ++i) {
        uniform_t *uniform = material->uniforms->items[i];
        uniform->handle = get_uniform_location(shader->handle, uniform->name);
    }
}

void set_uniform_bool(material_t *material, const char *name, bool value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_BOOLEAN)
            WARNINGF("SETTING BOOL TO A UNIFORM ('%s') THAT IS NOT A BOOL!", name);
        uniform->current_value.bool_value = value;
    }
}

void set_uniform_byte(material_t *material, const char *name, byte value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_BYTE)
            WARNINGF("SETTING BYTE TO A UNIFORM ('%s') THAT IS NOT A BYTE!", name);
        uniform->current_value.byte_value = value;
    }
}

void set_uniform_ubyte(material_t *material, const char *name, ubyte value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_UBYTE)
            WARNINGF("SETTING UBYTE TO A UNIFORM ('%s') THAT IS NOT A UBYTE!", name);
        uniform->current_value.ubyte_value = value;
    }
}

void set_uniform_short(material_t *material, const char *name, short value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_SHORT)
            WARNINGF("SETTING SHORT TO A UNIFORM ('%s') THAT IS NOT A SHORT!", name);
        uniform->current_value.short_value = value;
    }
}

void set_uniform_ushort(material_t *material, const char *name, ushort value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_USHORT)
            WARNINGF("SETTING USHORT TO A UNIFORM ('%s') THAT IS NOT A USHORT!", name);
        uniform->current_value.ushort_value = value;
    }
}

void set_uniform_int(material_t *material, const char *name, int value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_INT)
            WARNINGF("SETTING INT TO A UNIFORM ('%s') THAT IS NOT A INT!", name);
        uniform->current_value.int_value = value;
    }
}

void set_uniform_uint(material_t *material, const char *name, uint value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_UINT)
            WARNINGF("SETTING UINT TO A UNIFORM ('%s') THAT IS NOT A UINT!", name);
        uniform->current_value.uint_value = value;
    }
}

void set_uniform_long(material_t *material, const char *name, long value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_LONG)
            WARNINGF("SETTING LONG TO A UNIFORM ('%s') THAT IS NOT A LONG!", name);
        uniform->current_value.long_value = value;
    }
}

void set_uniform_float(material_t *material, const char *name, float value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_FLOAT)
            WARNINGF("SETTING FLOAT TO A UNIFORM ('%s') THAT IS NOT A FLOAT!", name);
        uniform->current_value.float_value = value;
    }
}

void set_uniform_double(material_t *material, const char *name, double value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_DOUBLE)
            WARNINGF("SETTING DOUBLE TO A UNIFORM ('%s') THAT IS NOT A DOUBLE!", name);
        uniform->current_value.double_value = value;
    }
}

void set_uniform_vec2(material_t *material, const char *name, glm::vec2 value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_VEC2)
            WARNINGF("SETTING VEC2 TO A UNIFORM ('%s') THAT IS NOT A VEC2!", name);
        uniform->current_value.vector2_value = value;
    }
}

void set_uniform_vec3(material_t *material, const char *name, glm::vec3 value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_VEC3)
            WARNINGF("SETTING VEC3 TO A UNIFORM ('%s') THAT IS NOT A VEC3!", name);
        uniform->current_value.vector3_value = value;
    }
}

void set_uniform_vec4(material_t *material, const char *name, glm::vec4 value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_VEC4)
            WARNINGF("SETTING VEC4 TO A UNIFORM ('%s') THAT IS NOT A VEC4!", name);

        uniform->current_value.vector4_value = value;
    }
}

void set_uniform_matrix(material_t *material, const char *name, glm::mat4 value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_MATRIX)
            WARNINGF("SETTING MATRIX TO A UNIFORM ('%s') THAT IS NOT A MATRIX!", name);

        uniform->current_value.matrix_value = value;
    }
}

void set_uniform_texture(material_t *material, const char *name, texture_t *value) {
    ENSURE(material != null);

    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null) {
        if (uniform->type != UNIFORM_TYPE::UNIFORM_TEXTURE2D)
            WARNINGF("SETTING TEXTURE TO A UNIFORM ('%s') THAT IS NOT A TEXTURE!", name);

        uniform->current_value.texture_value.texture = value;
    }
}

void set_uniform_texture_property(material_t *material, const char *name, texture_material_propery_t value) {
    uniform_t *uniform = find_uniform_by_name(name, material);
    if (uniform != null)
        uniform->current_value.texture_value = value;
}

uniform_t *find_uniform_by_name(const char *name, material_t *material) {
    int name_hash = hash((char *) name);
    list<uniform_t *> *uniforms = material->uniforms;
    for (int i = 0; i < uniforms->length; ++i) {
        uniform_t *uniform = uniforms->items[i];
        if (uniform->handle < 0)
            continue;

        if (uniform->name_hash == name_hash) {
            return uniform;
        }
    }

    return null;
}

void buff_uniform(uniform_t *uniform) {

    // The shader doesn't have the material uniform defined on the material file
    if (uniform->handle < 0)
        return;

    switch (uniform->type) {

        case UNIFORM_BOOLEAN:
            glUniform1i(uniform->handle, uniform->current_value.bool_value);
            break;
        case UNIFORM_BYTE:
            glUniform1i(uniform->handle, uniform->current_value.byte_value);
            break;
        case UNIFORM_UBYTE:
            glUniform1ui(uniform->handle, uniform->current_value.ubyte_value);
            break;
        case UNIFORM_SHORT:
            glUniform1i(uniform->handle, uniform->current_value.short_value);
            break;
        case UNIFORM_USHORT:
            glUniform1ui(uniform->handle, uniform->current_value.ushort_value);
            break;
        case UNIFORM_INT:
            glUniform1i(uniform->handle, uniform->current_value.int_value);
            break;
        case UNIFORM_UINT:
            glUniform1ui(uniform->handle, uniform->current_value.uint_value);
            break;
        case UNIFORM_LONG:
            glUniform1i(uniform->handle, uniform->current_value.long_value);
            break;
        case UNIFORM_FLOAT:
            glUniform1f(uniform->handle, uniform->current_value.float_value);
            break;
        case UNIFORM_DOUBLE:
            glUniform1d(uniform->handle, uniform->current_value.double_value);
            break;
        case UNIFORM_VEC2:
            glUniform2fv(uniform->handle, 1, (const GLfloat *) glm::value_ptr(uniform->current_value.vector2_value));
            break;
        case UNIFORM_VEC3:
            glUniform3fv(uniform->handle, 1, (const GLfloat *) glm::value_ptr(uniform->current_value.vector3_value));
            break;
        case UNIFORM_VEC4:
            glUniform4fv(uniform->handle, 1, (const GLfloat *) glm::value_ptr(uniform->current_value.vector4_value));
            break;
        case UNIFORM_MATRIX:
            glUniformMatrix4fv(
                    uniform->handle,
                    1,
                    GL_FALSE,
                    (const GLfloat *) glm::value_ptr(uniform->current_value.matrix_value)
            );

            break;
        case UNIFORM_TEXTURE2D:
            glActiveTexture(uniform->current_value.texture_value.texture_target_index);
            uint handle = 0;
            if (uniform->current_value.texture_value.texture != null)
                handle = uniform->current_value.texture_value.texture->handle;

            glBindTexture(GL_TEXTURE_2D, handle);

            // texture target index is GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE{X}
            // But the parameter must be 0, 1, {x}. So we subtract the index from the GL_TEXTURE0
            // to get only the "{x}"
            // NOTE: this don't need to be done every frame
            glUniform1i(uniform->handle, uniform->current_value.texture_value.texture_target_index - GL_TEXTURE0);
            break;
    }

    CHECK_GL_ERROR();
}

void buff_uniforms(list<uniform_t *> *uniforms) {
    for (int i = 0; i < uniforms->length; ++i) {
        buff_uniform(uniforms->items[i]);
    }
}

// DRAWING

static graphics_state_t *gl_state;

color_rgba_t black() {
    return glm::vec4(0, 0, 0, 1);
}

color_rgba_t white() {
    return glm::vec4(1, 1, 1, 1);
}

color_rgba_t red() {
    return glm::vec4(1, 0, 0, 1);
}

color_rgba_t green() {
    return glm::vec4(0, 1, 0, 1);
}

color_rgba_t blue() {
    return glm::vec4(0, 0, 1, 1);
}

color_rgba_t transparent() {
    return glm::vec4(0, 0, 0, 0);
}

color_mask_t get_default_color_mask() {
    color_mask_t color_mask;
    color_mask.red = true;
    color_mask.green = true;
    color_mask.blue = true;
    color_mask.alpha = true;
    return color_mask;
}

stencil_settings_t get_default_stencil_settings() {
    stencil_settings_t stencil_settings;

    stencil_settings.compare_func = COMPARE_DISABLED;
    stencil_settings.stencil_mask = 0xFF;

    stencil_settings.stencil_fail = STENCIL_OP_KEEP;
    stencil_settings.stencil_pass_depth_fail = STENCIL_OP_KEEP;
    stencil_settings.stencil_depth_pass = STENCIL_OP_REPLACE;

    stencil_settings.reference_value = 1;
    stencil_settings.stencil_func_mask = 0xFF;

    return stencil_settings;
}

CULL_FUNCTIONS get_default_cull_func() {
    return CULL_DISABLED;
}

depth_settings_t get_default_depth_settings() {
    depth_settings_t settings;
    settings.compare_func = COMPARE_LESS;
    settings.mask = DEPTH_BUFFER_DISABLED;
    return settings;
}

void prepare_graphics() {
    gl_state = (graphics_state_t *) memalloc(sizeof(graphics_state_t));

    gl_state->rendereres = create_list<mesh_renderer_t *>(10);
    gl_state->cameras = create_list<camera_t *>(1);

    gl_state->current_cull_func = get_default_cull_func();

    gl_state->current_view_port = get_screen_view_port();

    gl_state->color_mask = get_default_color_mask();

    gl_state->clear_color = black();

    gl_state->current_stencil_config = get_default_stencil_settings();
    gl_state->current_depth_settings = get_default_depth_settings();
}

void release_graphics() {

    //TODO: maybe store all shaders, materials, etc and delete all of them here?!
    //TODO: maybe store all shaders, materials, etc and delete all of them here?!
    //TODO: maybe store all shaders, materials, etc and delete all of them here?!
    //TODO: maybe store all shaders, materials, etc and delete all of them here?!
    //TODO: maybe store all shaders, materials, etc and delete all of them here?!

    destroy_list(gl_state->rendereres);
    memfree(gl_state);
}

graphics_state_t get_graphics_state() {
    return *gl_state;
}

void set_cull_func(CULL_FUNCTIONS func) {
    if (gl_state->current_cull_func != func) {
        if (func == CULL_DISABLED) {
            glDisable(GL_CULL_FACE);
        } else {
            if (gl_state->current_cull_func == CULL_DISABLED)
                glEnable(GL_CULL_FACE);

            glCullFace(func);
        }

        CHECK_GL_ERROR();

        gl_state->current_cull_func = func;
    }
}

void use_material(material_t *material) {
    ENSURE(material != null);

    if (gl_state->current_shader_program != material->shader->handle) {
        glUseProgram(material->shader->handle);
        gl_state->current_shader_program = material->shader->handle;
        CHECK_GL_ERROR();
    }

    set_depth_func(material->depth_func);
    set_cull_func(material->cull_func);

    // set camera matrices
    set_uniform_matrix(material, "PROJECTION", gl_state->current_camera->projection);
    set_uniform_matrix(material, "VIEW", gl_state->current_camera->view);
    set_uniform_matrix(material, "VP", gl_state->current_camera->_matrix);

    buff_uniforms(material->uniforms);
}

void prepare_to_draw(mesh_renderer_t *renderer) {
    // TODO: maybe flag this as already prepare to prevent preparing unnecessarily?!
    update_transform_matrix(renderer->entity->transform);

    material_t *material = renderer->material;

    // set mesh matrices
    set_uniform_matrix(material, "MODEL", renderer->entity->transform->_matrix);

    glm::mat4 mvp = gl_state->current_camera->_matrix * renderer->entity->transform->_matrix;
    set_uniform_matrix(material, "MVP", mvp);

    use_material(material);
}

void draw_renderer_with_material(mesh_renderer_t *renderer, material_t *material) {
    material_t *old_material = renderer->material;
    renderer->material = material;
    draw_renderer(renderer);
    renderer->material = old_material;
}

void draw_renderer(mesh_renderer_t *renderer) {
    prepare_to_draw(renderer);

    mesh_t *mesh = renderer->mesh;

    ENSURE(mesh != null);

    glBindVertexArray(mesh->vao_handle);
    CHECK_GL_ERROR();

    set_vbo_enable_state(mesh, true);

    if (mesh->indices_handle > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_handle);
        CHECK_GL_ERROR();

        glDrawElements(GL_TRIANGLES, mesh->model->indices->length, GL_UNSIGNED_INT, null);
        CHECK_GL_ERROR();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HANDLE_NONE);
        CHECK_GL_ERROR();
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh->model->positions->length);
        CHECK_GL_ERROR();
    }

    set_vbo_enable_state(mesh, false);

    glBindVertexArray(HANDLE_NONE);
}

void set_vbo_enable_state(mesh_t *mesh, bool state) {
    if (mesh->vertex_position_handle > 0) {
        if (state)
            glEnableVertexAttribArray(VERTEX_POSITION_ATTRIBUTE_INDEX);
        else
            glDisableVertexAttribArray(VERTEX_POSITION_ATTRIBUTE_INDEX);
    }

    if (mesh->vertex_color_handle > 0) {
        if (state)
            glEnableVertexAttribArray(VERTEX_COLOR_ATTRIBUTE_INDEX);
        else
            glDisableVertexAttribArray(VERTEX_COLOR_ATTRIBUTE_INDEX);
    }

    if (mesh->vertex_tex_coord_handle > 0) {
        if (state)
            glEnableVertexAttribArray(VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX);
        else
            glDisableVertexAttribArray(VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX);
    }

    if (mesh->vertex_normal_handle > 0) {
        if (state)
            glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIBUTE_INDEX);
        else
            glDisableVertexAttribArray(VERTEX_NORMAL_ATTRIBUTE_INDEX);
    }

    CHECK_GL_ERROR();
}

void draw_scene() {
    list<camera_t *> *cameras = gl_state->cameras;
    uint length = cameras->length;
    for (int i = 0; i < length; ++i) {
        camera_t *camera = cameras->items[i];

        if (camera->enabled) {
            use_camera(camera);
            draw_all_renderers();
        }
    }
}

void draw_all_renderers() {
    draw_renderers(gl_state->rendereres);
}

void draw_renderers(list<mesh_renderer_t *> *renderers) {
    ENSURE(gl_state->current_camera != null);

    camera_t *camera = gl_state->current_camera;
    for (int i = 0; i < renderers->length; ++i) {
        mesh_renderer_t *renderer = renderers->items[i];
        if (renderer->should_be_drawn) {

            // TODO: improve this!!
            // TODO: improve this!!
            // TODO: improve this!!
            // TODO: improve this!!
            if ((camera->culling_mask & renderer->layer_mask) == renderer->layer_mask)
                draw_renderer(renderer);
        }
    }
}

mesh_renderer_t *create_mesh_renderer(material_t *material, mesh_t *mesh) {
    mesh_renderer_t *renderer = (mesh_renderer_t *) memalloc(sizeof(mesh_renderer_t));

    renderer->material = material;
    renderer->mesh = mesh;
    renderer->entity = create_entity(BUILT_IN_ENTITIES::MESH_RENDERER, renderer);
    renderer->should_be_drawn = true;
    renderer->layer_mask = 1 << 0;

    add(gl_state->rendereres, renderer);

    return renderer;
}

void destroy_mesh_renderer(mesh_renderer_t *renderer) {
    remove(gl_state->rendereres, renderer);
    destroy_entity(renderer->entity);
    memfree(renderer);
}

camera_t *create_camera() {
    camera_t *camera = (camera_t *) memalloc(sizeof(camera_t));

    camera->entity = create_entity(BUILT_IN_ENTITIES::CAMERA, camera);
    camera->_matrix = glm::mat4();
    camera->clear_mode = CAMERA_CLEAR_DEFAULT;
    camera->view_port = get_screen_view_port();
    camera->full_screen = true;
    camera->clear_color = black();

    camera->depth_buffer_status = DEPTH_TEST_DEFAULT;
    camera->color_mask = get_default_color_mask();
    camera->stencil_settings = get_default_stencil_settings();

    camera->enabled = true;
    camera->culling_mask = 0xFFFFFFF;

    camera->target = null;

    add(gl_state->cameras, camera);

    return camera;
}

void update_perspetive_camera_matrix(camera_t *camera) {
    perspective_camera_t perspective = camera->perspective;
    camera->projection = glm::perspective(
            perspective.field_of_view,
            perspective.aspect_ratio,
            perspective.near_plane,
            perspective.far_plane
    );
}

camera_t *create_perspective_camera(
        float field_of_view,
        float ratio,
        float near_plane,
        float far_plane
) {
    camera_t *camera = create_camera();
    camera->type = CAMERA_TYPE::PERSPECTIVE;
    camera->perspective.field_of_view = field_of_view;
    camera->perspective.aspect_ratio = ratio;
    camera->perspective.near_plane = near_plane;
    camera->perspective.far_plane = far_plane;
    update_perspetive_camera_matrix(camera);
    return camera;
}

void update_ortho_camera_matrix(camera_t *camera) {
    orthogonal_camera_t ortho = camera->ortho;
    camera->projection = glm::ortho(
            ortho.left,
            ortho.right,
            ortho.top,
            ortho.bottom,
            ortho.near_plane,
            ortho.far_plane
    );
}

camera_t *create_ortho_camera(
        float left,
        float right,
        float bottom,
        float top,
        float near_plane,
        float far_plane
) {
    camera_t *camera = create_camera();
    camera->type = CAMERA_TYPE::ORTHO;
    camera->ortho.left = left;
    camera->ortho.right = right;
    camera->ortho.bottom = bottom;
    camera->ortho.top = top;
    camera->ortho.near_plane = near_plane;
    camera->ortho.far_plane = far_plane;
    update_ortho_camera_matrix(camera);
    return camera;
}

void destroy_camera(camera_t *camera) {
    remove(gl_state->cameras, camera);
    destroy_entity(camera->entity);
    memfree(camera);
}

void update_camera_matrix(camera_t *camera) {
    if (camera->type == CAMERA_TYPE::PERSPECTIVE)
        update_perspetive_camera_matrix(camera);
    else
        update_ortho_camera_matrix(camera);

    transform_t *trans = camera->entity->transform;
    glm::vec3 forward_dir = get_forward(trans);

    // makes the forward vector into a screen_position in front of the camera
    glm::vec3 center = trans->position + (forward_dir * 1000000.f);

    camera->view = glm::lookAt(trans->position, center, world_up());
    camera->_matrix = camera->projection * camera->view;
}

void use_camera(camera_t *camera) {

    ENSURE(camera != null);

    if (gl_state->current_camera != camera) {
        gl_state->current_camera = camera;
    }

    use_frame_buffer(camera->target);
    set_view_port(camera->view_port);
    set_clear_color(camera->clear_color);
    set_color_mask(camera->color_mask);

    set_stencil_settings(camera->stencil_settings);
    set_depth_test_status(camera->depth_buffer_status);
    update_camera_matrix(camera);

    if (camera->clear_mode != CAMERA_CLEAR_NONE) {
        CLEAR_MASK clear_mask = CLEAR_COLOR_AND_DEPTH;
        switch (camera->clear_mode) {
            case CAMERA_CLEAR_COLOR:
                clear_mask = CLEAR_COLOR;
                break;
            case CAMERA_CLEAR_DEPTH:
                clear_mask = CLEAR_DEPTH;
                break;
            case CAMERA_CLEAR_STENCIL:
                clear_mask = CLEAR_STENCIL;
                break;
            case CAMERA_CLEAR_COLOR_DEPTH:
                clear_mask = CLEAR_COLOR_AND_DEPTH;
                break;
            case CAMERA_CLEAR_ALL:
                clear_mask = CLEAR_ALL;
                break;
        }

        clear_current_view_port(clear_mask);
    }
}

void update_cameras_view_port_to_screen_size() {
    list<camera_t *> *cameras = gl_state->cameras;
    uint length = cameras->length;
    for (int i = 0; i < length; ++i) {
        camera_t *camera = cameras->items[i];
        if (camera->full_screen) {
            camera->view_port.size = glm::vec2(1, 1);
        }
    }
}

view_port_t get_view_port_absolute(view_port_t relative_view_port) {
    glm::vec2 screen_size = get_screen_size();

    glm::vec2 view_port_size = glm::vec2(
            relative_view_port.size.x * screen_size.x,
            relative_view_port.size.y * screen_size.y
    );

    glm::vec2 view_port_position = glm::vec2(
            relative_view_port.position.x * screen_size.x,
            relative_view_port.position.y * screen_size.y
    );

    view_port_t absolute_view_port;
    absolute_view_port.size = view_port_size;
    absolute_view_port.position = view_port_position;
    return absolute_view_port;
}

view_port_t get_screen_view_port() {
    view_port_t screen_view_port;
    screen_view_port.position = glm::vec2(0, 0);
    screen_view_port.size = glm::vec2(1, 1);

    return screen_view_port;
}

void set_view_port(view_port_t view_port) {
    if (view_port.size != gl_state->current_view_port.size ||
        view_port.position != gl_state->current_view_port.position
            ) {
        gl_state->current_view_port = view_port;

        view_port_t absolute = get_view_port_absolute(view_port);
        glViewport(absolute.position.x, absolute.position.y, absolute.size.x, absolute.size.y);
    }
}

void clear_current_view_port(CLEAR_MASK clear_mask) {
    clear_view_port(gl_state->current_view_port, clear_mask);
}

void clear_view_port(view_port_t view_port, CLEAR_MASK clear_mask) {
    view_port_t absolute = get_view_port_absolute(view_port);
    glm::ivec2 pos = absolute .position;
    glm::ivec2 size = absolute .size;

    // TODO: maybe validate if the viewport is fullscreen before enabling scissors test?!
    // Only clear the view-port, not the whole screen
    glEnable(GL_SCISSOR_TEST);
    glScissor(pos.x, pos.y, size.x, size.y);
    glClear(clear_mask);
    glDisable(GL_SCISSOR_TEST);
}

void set_clear_color(color_rgba_t color) {
    if (gl_state->clear_color != color) {
        gl_state->clear_color = color;
        glClearColor(color.r, color.g, color.b, color.a);
    }
}

void set_depth_test_status(DEPTH_BUFFER_STATUS status) {
    depth_settings_t current = gl_state->current_depth_settings;
    current.mask = status;
    set_depth_settings(current);
}

void set_depth_func(COMPARE_FUNCTIONS func) {
    depth_settings_t current = gl_state->current_depth_settings;
    current.compare_func = func;
    set_depth_settings(current);
}

void set_depth_settings(depth_settings_t settings) {
    depth_settings_t current_settings = gl_state->current_depth_settings;
    if (current_settings.mask != settings.mask) {
        glDepthMask(settings.mask);
    }

    if (current_settings.compare_func != settings.compare_func) {
        if (settings.compare_func == COMPARE_DISABLED)
            glDisable(GL_DEPTH_TEST);
        else {
            if (current_settings.compare_func == COMPARE_DISABLED)
                glEnable(GL_DEPTH_TEST);

            glDepthFunc(settings.compare_func);
        }
    }

    CHECK_GL_ERROR();

    gl_state->current_depth_settings = settings;
}

void set_color_mask(color_mask_t mask) {
    color_mask_t current = gl_state->color_mask;
    if (mask.red != current.red ||
        mask.green != current.green ||
        mask.blue != current.blue ||
        mask.alpha != current.alpha
            ) {
        glColorMask((GLboolean) mask.red, (GLboolean) mask.green, (GLboolean) mask.blue, (GLboolean) mask.alpha);
        gl_state->color_mask = mask;
    }
}

color_mask_t get_color_mask_disabled() {
    color_mask_t mask;
    mask.red = false;
    mask.green = false;
    mask.blue = false;
    mask.alpha = false;
    return mask;
}


void set_stencil_func(COMPARE_FUNCTIONS func, int ref_value, uint func_mask) {
    stencil_settings_t current_settings = gl_state->current_stencil_config;
    current_settings.compare_func = func;
    current_settings.reference_value = ref_value;
    current_settings.stencil_func_mask = func_mask;
    set_stencil_settings(current_settings);
}

void set_stencil_op(STENCIL_OP_ACTION fail, STENCIL_OP_ACTION depth_fail_stencil_pass, STENCIL_OP_ACTION all_pass) {
    stencil_settings_t settings = gl_state->current_stencil_config;
    settings.stencil_fail = fail;
    settings.stencil_pass_depth_fail = depth_fail_stencil_pass;
    settings.stencil_pass_depth_fail = all_pass;
    set_stencil_settings(settings);
}

void set_stencil_mask(uint stencil_mask) {
    stencil_settings_t settings = gl_state->current_stencil_config;
    settings.stencil_mask = stencil_mask;
    set_stencil_settings(settings);
}

void use_screen_frame_buffer() {
    use_frame_buffer(null);
}

void use_frame_buffer(frame_buffer_t *frame_buffer) {

    if (frame_buffer != gl_state->current_frame_buffer) {
        if (frame_buffer == null) {
            glBindFramebuffer(GL_FRAMEBUFFER, HANDLE_NONE);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->handle);
        }

        CHECK_GL_ERROR();
    }
}

void set_stencil_settings(stencil_settings_t settings) {
    stencil_settings_t current_settings = gl_state->current_stencil_config;

    if (current_settings.compare_func != settings.compare_func ||
        current_settings.reference_value != settings.reference_value ||
        current_settings.stencil_func_mask != settings.stencil_func_mask
    ) {
        if (settings.compare_func == COMPARE_DISABLED) {
            glDisable(GL_STENCIL_TEST);
            CHECK_GL_ERROR();
        } else {
            if (current_settings.compare_func == COMPARE_DISABLED) {
                glEnable(GL_STENCIL_TEST);
                CHECK_GL_ERROR();
            }

            glStencilFunc(settings.compare_func, settings.reference_value, settings.stencil_func_mask);

            CHECK_GL_ERROR();
        }
    }

    if (current_settings.stencil_fail != settings.stencil_fail ||
        current_settings.stencil_pass_depth_fail != settings.stencil_pass_depth_fail ||
        current_settings.stencil_depth_pass != settings.stencil_depth_pass
            ) {
        glStencilOp(
                settings.stencil_fail,
                settings.stencil_pass_depth_fail,
                settings.stencil_depth_pass
        );

        CHECK_GL_ERROR();
    }

    if (current_settings.stencil_mask != settings.stencil_mask) {
        glStencilMask(settings.stencil_mask);


        CHECK_GL_ERROR();
    }

    CHECK_GL_ERROR();

    gl_state->current_stencil_config = settings;
}

frame_buffer_t *create_frame_buffer(glm::vec2 resolution) {
    uint handle;
    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    texture_config_t texture_config = get_default_texture_config();

    image_t *color_image = create_image_full(null, TEXTURE_RGB, resolution);
    texture_t *color_texture = create_texture(color_image, texture_config);
    glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, // NOTE: Maybe we need to receive this as parameter?!
            GL_TEXTURE_2D,
            color_texture->handle,
            0 //mip-map level
    );
    CHECK_GL_ERROR();
    CHECK_FBO_STATUS();

    // NOTE: Maybe this should be done using Frame Buffers instead of textures,
    // since the caller might not need to use them as textures?! Maybe this configurations should be parameterized

    // NOTE: Maybe we should give the option to make a depth-only texture?
    image_t *depth_image = create_image_full(null, TEXTURE_DEPTH, resolution);
    texture_t *depth_texture = create_texture(depth_image, texture_config);
    glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            depth_texture->handle,
            0 //mip-map level
    );
    CHECK_GL_ERROR();
    CHECK_FBO_STATUS();

    glBindFramebuffer(GL_FRAMEBUFFER, HANDLE_NONE);

    frame_buffer_t *frame_buffer = (frame_buffer_t *) memalloc(sizeof(frame_buffer_t));
    frame_buffer->handle = handle;
    frame_buffer->color_texture = color_texture;
    frame_buffer->depth_texture = depth_texture;
    frame_buffer->resolution = resolution;
    return frame_buffer;
}

void destroy_frame_buffer(frame_buffer_t *frame_buffer) {
    destroy_image(frame_buffer->color_texture->image);
    destroy_texture(frame_buffer->color_texture);

    destroy_image(frame_buffer->depth_texture->image);
    destroy_texture(frame_buffer->depth_texture);

    glDeleteFramebuffers(1, &frame_buffer->handle);
    memfree(frame_buffer);
}