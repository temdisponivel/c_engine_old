//
// Created by temdisponivel on 10/02/2018.
//

#include "graphics.h"
#include <log.h>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

static graphics_state_t gl_state = {};

graphics_state_t gl::get_graphics_state() {
    return gl_state;
}

void gl::set_depth_func(COMPARE_FUNCTIONS func) {
    if (gl_state.current_depth_func != func) {
        if (func == COMPARE_DISABLED) {
            glDisable(GL_DEPTH_TEST);
        } else {
            if (gl_state.current_depth_func == COMPARE_DISABLED)
                glEnable(GL_DEPTH_TEST);

            if (func == COMPARE_DEFAULT)
                glDepthFunc(DEFAULT_COMPARE_FUNC);
            if (func == COMPARE_LESS)
                glDepthFunc(GL_LESS);
            else if (func == COMPARE_LESS_OR_EQUAL)
                glDepthFunc(GL_LEQUAL);
            else if (func == COMPARE_EQUAL)
                glDepthFunc(GL_EQUAL);
            else if (func == COMPARE_GREATER)
                glDepthFunc(GL_GREATER);
            else if (func == COMPARE_GREATER_OR_EQUAL)
                glDepthFunc(GL_GEQUAL);
            else if (func == COMPARE_DIFFERENT)
                glDepthFunc(GL_NOTEQUAL);
        }

        gl_state.current_depth_func = func;
    }
}

image_t *gl::create_image(const char *image_file_path) {
    image_t *image;

    int width, height, channels;
    stbi_uc *data = stbi_load(image_file_path, &width, &height, &channels, IMAGE_CHANNELS::RGBA);

#if DEV
    if (data == null) {
        ERRORF(FILE_LINE, "IMAGE '%s' COULDN'T BE LOADED!", image_file_path);
    }

    if (channels < 3 || channels > 4) {
        WARNINGF("THE IMAGE '%s' has %i channels! We only support 3 and 4.", image_file_path, channels);
    }
#endif

    IMAGE_CHANNELS image_channels = IMAGE_CHANNELS::RGB;
    if (channels == 4)
        image_channels = IMAGE_CHANNELS::RGBA;

    image = (image_t *) memalloc(sizeof(image_t));
    image->data = data;
    image->channels = image_channels;
    image->size.x = width;
    image->size.y = height;
    return image;
}

void gl::destroy_image(image_t *image) {
    stbi_image_free(image->data);
    memfree(image);
}

texture_t *gl::create_texture(image_t *image, texture_config_t config) {
    uint texture_handle;

    GLenum image_format = GL_RGB;
    if (image->channels == IMAGE_CHANNELS::RGBA)
        image_format = GL_RGBA;

    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    CHECK_GL_ERROR();

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            image_format,
            (uint) image->size.x,
            (uint) image->size.y,
            0,
            image_format,
            GL_UNSIGNED_BYTE,
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

void gl::destroy_texture(texture_t *texture) {
    glDeleteTextures(1, &texture->handle);
    memfree(texture);
}

texture_config_t gl::get_default_texture_config() {
    texture_config_t default_config = {};

    default_config.texture_wrap_r = TEX_WRAP_MIRRORED_REPEAT;
    default_config.texture_wrap_s = TEX_WRAP_MIRRORED_REPEAT;
    default_config.texture_wrap_t = TEX_WRAP_MIRRORED_REPEAT;

    default_config.texture_min_filter = TEX_MIN_FILTER_NEAREST;
    default_config.texture_mag_filter = TEX_MAG_FILTER_NEAREST;

    return default_config;
}

void gl::buff_texture_config_to_gl(texture_t *texture) {
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

model_t *gl::create_model(const char *model_file_path) {
    // TODO: optimize this!!!

    list<float> *positions = lists::create<float>(256);
    list<float> *colors = lists::create<float>(256);
    list<float> *tex_coords = lists::create<float>(256);
    list<float> *normals = lists::create<float>(256);
    list<int> *indices = lists::create<int>(256);

    const char line_buffer[256] = {};

    FILE *file = fopen(model_file_path, "rb");
    ENSURE(file != null);

    while (fscanf(file, "%s", line_buffer) != EOF) {
        if (strcmp(line_buffer, "vp") == 0) {
            float x, y, z;
            if (fscanf(file, "%f %f %f\n", &x, &y, &z)) {
                lists::add<float>(positions, x);
                lists::add<float>(positions, y);
                lists::add<float>(positions, z);
            }
        } else if (strcmp(line_buffer, "vc") == 0) {
            float r, g, b;
            if (fscanf(file, "%f %f %f\n", &r, &g, &b)) {
                lists::add<float>(colors, r);
                lists::add<float>(colors, g);
                lists::add<float>(colors, b);
            }
        } else if (strcmp(line_buffer, "vt") == 0) {
            float x, y;
            if (fscanf(file, "%f %f\n", &x, &y)) {
                lists::add<float>(tex_coords, x);
                lists::add<float>(tex_coords, y);
            }
        } else if (strcmp(line_buffer, "vn") == 0) {
            float x, y, z;
            if (fscanf(file, "%f %f %f\n", &x, &y, &z)) {
                lists::add<float>(normals, x);
                lists::add<float>(normals, y);
                lists::add<float>(normals, z);
            }
        } else if (strcmp(line_buffer, "vi") == 0) {
            int indice;
            if (fscanf(file, "%i\n", &indice)) {
                lists::add<int>(indices, indice);
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

void gl::destroy_model(model_t *mesh) {

    lists::destroy(mesh->positions);
    lists::destroy(mesh->colors);
    lists::destroy(mesh->tex_coords);
    lists::destroy(mesh->normals);
    lists::destroy(mesh->indices);

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

mesh_t *gl::create_mesh(model_t *model) {
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

    if (!lists::null_or_empty(model->indices)) {
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

    if (!lists::null_or_empty(model->positions)) {
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

    if (!lists::null_or_empty(model->colors)) {
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

    if (!lists::null_or_empty(model->tex_coords)) {
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

    if (!lists::null_or_empty(model->normals)) {
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

void gl::destroy_mesh(mesh_t *mesh) {
    if (mesh->indices_handle > 0)
        glDeleteBuffers(1, &mesh->indices_handle);

    if (mesh->vertex_position_handle > 0)
        glDeleteBuffers(1, &mesh->vertex_position_handle);

    if (mesh->vertex_color_handle > 0)
        glDeleteBuffers(1, &mesh->vertex_color_handle);

    if (mesh->vertex_tex_coord_handle > 0)
        glDeleteBuffers(1, &mesh->vertex_tex_coord_handle);

    if (mesh->vertex_normal_handle > 0)
        glDeleteBuffers(1, &mesh->vertex_normal_handle);

    glDeleteVertexArrays(1, &mesh->indices_handle);
    memfree(mesh);

    CHECK_GL_ERROR();
}

shader_t *gl::create_shader(
        const char *shader_code,
        SHADER_TYPE type
) {
    GLenum shader_type;
    if (type == SHADER_TYPE::VERTEX_SHADER)
        shader_type = GL_VERTEX_SHADER;
    else
        shader_type = GL_FRAGMENT_SHADER;

    uint handle = glCreateShader(shader_type);
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

void gl::destroy_shader(shader_t *shader) {
    glDeleteShader(shader->handle);
    memfree(shader);
}

shader_program_t *gl::create_shader_program(
        shader_t vertex_shader,
        shader_t fragment_shader,
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

    return program;
}

void gl::destroy_shader_program(shader_program_t *shader) {
    glDeleteProgram(shader->handle);
    memfree(shader);
}

void gl::create_and_add_uniform(
        material_t *material,
        uniform_definition_t uniform_def
) {
    ENSURE(uniform_def.name != null);

    int handle = glGetUniformLocation(material->shader->handle, uniform_def.name);
    CHECK_GL_ERROR();

    if (handle < 0) {
        WARNINGF("UNIFORM '%s' NOT FOUND!" FILE_LINE, uniform_def.name);
    }

    uniform_t *uniform = (uniform_t *) memalloc(sizeof(uniform_t));

    uniform->handle = handle;
    uniform->name_hash = hash(uniform_def.name);
    uniform->type = uniform_def.type;
    uniform->current_value = uniform_def.default_value;

    lists::add(material->uniforms, uniform);
}

material_t *gl::create_material(
        shader_program_t *shader,
        COMPARE_FUNCTIONS depth_func,
        list<uniform_definition_t> *uniform_definitions
) {
    material_t *material = (material_t *) memalloc(sizeof(material_t));
    list<uniform_t *> *uniforms = lists::create<uniform_t *>(uniform_definitions->length);

    material->shader = shader;
    material->depth_func = depth_func;
    material->shader = shader;
    material->uniforms = uniforms;

    for (int i = 0; i < uniform_definitions->length; ++i) {
        uniform_definition_t uniform_def = uniform_definitions->items[i];
        create_and_add_uniform(material, uniform_def);
    }

    return material;
}

void gl::destroy_material(material_t *material) {
    for (int i = 0; i < material->uniforms->length; ++i) {
        uniform_t *uni = material->uniforms->items[i];
        memfree(uni);
    }
    lists::destroy(material->uniforms);
    memfree(material);
}

void gl::set_uniform_bool(material_t *material, const char *name, bool value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.bool_value = value;
}

void gl::set_uniform_byte(material_t *material, const char *name, byte value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.byte_value = value;
}

void gl::set_uniform_ubyte(material_t *material, const char *name, ubyte value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.ubyte_value = value;
}

void gl::set_uniform_short(material_t *material, const char *name, short value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.short_value = value;
}

void gl::set_uniform_ushort(material_t *material, const char *name, ushort value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.ushort_value = value;
}

void gl::set_uniform_int(material_t *material, const char *name, int value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.int_value = value;
}

void gl::set_uniform_uint(material_t *material, const char *name, uint value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.uint_value = value;
}

void gl::set_uniform_long(material_t *material, const char *name, long value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.long_value = value;
}

void gl::set_uniform_float(material_t *material, const char *name, float value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.float_value = value;
}

void gl::set_uniform_double(material_t *material, const char *name, double value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.double_value = value;
}

void gl::set_uniform_vec2(material_t *material, const char *name, glm::vec2 value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.vector2_value = value;
}

void gl::set_uniform_vec3(material_t *material, const char *name, glm::vec3 value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.vector3_value = value;
}

void gl::set_uniform_vec4(material_t *material, const char *name, glm::vec4 value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.vector4_value = value;
}

void gl::set_uniform_matrix(material_t *material, const char *name, glm::mat4 value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.matrix_value = value;
}

void gl::set_uniform_texture(material_t *material, const char *name, texture_t *value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.texture_value.texture = value;
}

void gl::set_uniform_texture_property(material_t *material, const char *name, texture_material_propery_t value){
    uniform_t *uniform = find_uniform_by_name(name, material);
    ENSURE(uniform != null);
    uniform->current_value.texture_value = value;
}

uniform_t *gl::find_uniform_by_name(const char *name, material_t *material) {
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

void gl::buff_uniform(uniform_t *uniform) {

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
        case UNIFORM_MAT4:
            glUniformMatrix4fv(
                    uniform->handle,
                    1,
                    GL_FALSE,
                    (const GLfloat *) glm::value_ptr(uniform->current_value.matrix_value)
            );

            break;
        case UNIFORM_TEXTURE2D:
            glActiveTexture(uniform->current_value.texture_value.texture_target_index);
            glBindTexture(GL_TEXTURE_2D, uniform->current_value.texture_value.texture->handle);

            // texture target index is GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE{X}
            // But the parameter must be 0, 1, {x}. So we subtract the index from the GL_TEXTURE0
            // to get only the "{x}"
            glUniform1i(uniform->handle, uniform->current_value.texture_value.texture_target_index - GL_TEXTURE0);
            break;
    }

    CHECK_GL_ERROR();
}

void gl::buff_uniforms(list<uniform_t *> *uniforms) {
    for (int i = 0; i < uniforms->length; ++i) {
        buff_uniform(uniforms->items[i]);
    }
}

void gl::use_material(material_t *material) {
    if (gl_state.current_shader_program != material->shader->handle) {
        glUseProgram(material->shader->handle);
        gl_state.current_shader_program = material->shader->handle;
        CHECK_GL_ERROR();
    }

    set_depth_func(material->depth_func);

    gl::buff_uniforms(material->uniforms);
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

void gl::draw_mesh(mesh_t *mesh) {
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