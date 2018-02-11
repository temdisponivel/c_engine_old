//
// Created by temdisponivel on 10/02/2018.
//

#include "graphics.h"
#include <log.h>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

image_t *gl::create_image(const char *image_file_path) {
    image_t *image;

    int width, height, channels;
    stbi_uc *data = stbi_load(image_file_path, &width, &height, &channels, IMAGE_CHANNELS::RGBA);

#if DEV
    if (data == null) {
        ERRORF("IMAGE '%s' COULDN'T BE LOADED!", image_file_path);
    }

    if (channels < 3 || channels > 4) {
        WARNINGF("THE IMAGE '%s' has %i channels! We only support 3 and 4.", image_file_path, channels);
    }
#endif

    IMAGE_CHANNELS image_channels = IMAGE_CHANNELS::RGB;
    if (channels == 4)
        image_channels = IMAGE_CHANNELS::RGBA;

    image = (image_t *) memalloc(sizeof(image));
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.texture_mag_filter);

    glBindTexture(GL_TEXTURE_2D, 0);

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


void gl::buff_texture_config_to_gl(texture_t *texture) {
    texture_config_t config = texture->config;

    glBindTexture(GL_TEXTURE_2D, texture->handle);

    // TODO: validate values according to the parameter!

    if (config.depth_stencil_texture_mode > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, config.depth_stencil_texture_mode);

    if (config.texture_base_level > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, config.texture_base_level );

    if (config.texture_compare_func > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, config.texture_compare_func);

    if (config.texture_compare_mode > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, config.texture_compare_mode);

    if (config.texture_lod_bias > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, config.texture_lod_bias);

    if (config.texture_min_filter > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.texture_min_filter);

    if (config.texture_mag_filter > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.texture_mag_filter);

    if (config.texture_min_lod > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, config.texture_min_lod);

    if (config.texture_max_lod > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, config.texture_max_lod);

    if (config.texture_max_level > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, config.texture_max_level);

    if (config.texture_swizzle_r > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, config.texture_swizzle_r);

    if (config.texture_swizzle_g > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, config.texture_swizzle_g);

    if (config.texture_swizzle_b > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, config.texture_swizzle_b);

    if (config.texture_swizzle_a > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, config.texture_swizzle_a);

    if (config.texture_wrap_s > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, config.texture_wrap_s);

    if (config.texture_wrap_t > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, config.texture_wrap_t);

    if (config.texture_wrap_r > 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, config.texture_wrap_r);

    glBindTexture(GL_TEXTURE_2D, 0);
}

model_t *gl::create_model(const char *model_file_path) {
    char *file_content = read_file_text(model_file_path);
    ASSERT(file_content == null);

    // TODO: optimize this!!!

    list<float> *positions = lists::create<float>(256);
    list<float> *colors = lists::create<float>(256);
    list<float> *tex_coords = lists::create<float>(256);
    list<float> *normals = lists::create<float>(256);
    list<int> *indices = lists::create<int>(256);

    const char line_buffer[256] = {};

    FILE *file = fopen(model_file_path, "rb");
    ASSERT(file == null);

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

    ASSERT(positions->length == 0);

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
        uint size,
        uint gl_data_type,
        GLboolean normalized,
        uint stride,
        void *offset,
        GLenum usage
) {
    glBindBuffer(GL_ARRAY_BUFFER, handle);

    glBufferData(
            GL_ARRAY_BUFFER,
            size,
            data,
            usage
    );

    glVertexAttribPointer(
            array_index,
            size,
            gl_data_type,
            normalized,
            stride,
            offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint create_vbo_from_list_helper(
        list<float> *list,
        uint array_index
) {
    uint handle;
    glGenBuffers(1, &handle);

    buff_vbo(
            handle,
            array_index,
            list->items,
            list->length,
            GL_FLOAT,
            GL_FALSE,
            0,
            0,
            GL_STATIC_DRAW
    );
}

mesh_t *gl::create_mesh(model_t *model) {
    uint vao;

    uint indices_vbo = 0;
    uint position_vbo = 0;
    uint color_vbo = 0;
    uint tex_coord_vbo = 0;
    uint normal_vbo = 0;

    if (!lists::null_or_empty(model->indices)) {
        glGenBuffers(1, &indices_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                model->indices->length,
                model->indices->items,
                GL_STATIC_DRAW
        );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if (!lists::null_or_empty(model->positions)) {
        position_vbo = create_vbo_from_list_helper(model->positions, VERTEX_POSITION_ATTRIBUTE_INDEX);
    }

    if (!lists::null_or_empty(model->colors)) {
        color_vbo = create_vbo_from_list_helper(model->colors, VERTEX_COLOR_ATTRIBUTE_INDEX);
    }

    if (!lists::null_or_empty(model->tex_coords)) {
        tex_coord_vbo = create_vbo_from_list_helper(model->tex_coords, VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX);
    }

    if (!lists::null_or_empty(model->normals)) {
        normal_vbo = create_vbo_from_list_helper(model->normals, VERTEX_NORMAL_ATTRIBUTE_INDEX);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    if (position_vbo > 0)
        glEnableVertexAttribArray(position_vbo);

    if (color_vbo > 0)
        glEnableVertexAttribArray(color_vbo);

    if (tex_coord_vbo > 0)
        glEnableVertexAttribArray(tex_coord_vbo);

    if (normal_vbo > 0)
        glEnableVertexAttribArray(normal_vbo);

    glBindVertexArray(0);

    mesh_t *mesh = (mesh_t *) memalloc(sizeof(mesh_t));
    mesh->model = model;
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
    glShaderSource(handle, 1, &shader_code, 0);
    glCompileShader(handle);

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
    glAttachShader(handle, vertex_shader.handle);
    glAttachShader(handle, fragment_shader.handle);

    glBindAttribLocation(handle, VERTEX_POSITION_ATTRIBUTE_INDEX, vertex_position_name);
    glBindAttribLocation(handle, VERTEX_COLOR_ATTRIBUTE_INDEX, vertex_color_name);
    glBindAttribLocation(handle, VERTEX_TEXTURE_COORD_ATTRIBUTE_INDEX, vertex_tex_coord_name);
    glBindAttribLocation(handle, VERTEX_NORMAL_ATTRIBUTE_INDEX, vertex_normal_name);

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

material_t *gl::create_material(
        shader_program_t *shader,
        list<uniform_definition_t> *uniform_definitions
) {
    material_t *material = (material_t *) memalloc(sizeof(material_t));
    material->shader = shader;

    list<uniform_t> *uniforms = lists::create<uniform_t>(uniform_definitions->length);
    for (int i = 0; i < uniform_definitions->length; ++i) {
        uniform_definition_t uniform_def = uniform_definitions->items[i];
        uniform_t uniform = {};

        int handle = glGetUniformLocation(shader->handle, uniform_def.name);
        if (handle < 0) {
            WARNINGF("UNIFORM '{0}' NOT FOUND!", uniform.name_hash);
            handle = 0;
        }

        uniform.handle = (uint) handle;
        uniform.name_hash = hash(uniform_def.name);
        uniform.type = uniform_def.type;
        uniform.current_value = uniform.current_value;

        lists::add(uniforms, uniform);
    }

    material->shader = shader;
    material->uniforms = uniforms;
}

void gl::destroy_material(material_t *material) {
    lists::destroy(material->uniforms);
    memfree(material);
}


void gl::buff_uniform(uniform_t uniform) {

    // The shader doesn't have the material uniform defined on the material file
    if (uniform.handle < 0)
        return;

    switch (uniform.type) {

        case UNIFORM_BOOLEAN:
            glUniform1i(uniform.handle, uniform.current_value.bool_value);
            break;
        case UNIFORM_BYTE:
            glUniform1i(uniform.handle, uniform.current_value.byte_value);
            break;
        case UNIFORM_UBYTE:
            glUniform1ui(uniform.handle, uniform.current_value.ubyte_value);
            break;
        case UNIFORM_SHORT:
            glUniform1i(uniform.handle, uniform.current_value.short_value);
            break;
        case UNIFORM_USHORT:
            glUniform1ui(uniform.handle, uniform.current_value.ushort_value);
            break;
        case UNIFORM_INT:
            glUniform1i(uniform.handle, uniform.current_value.int_value);
            break;
        case UNIFORM_UINT:
            glUniform1ui(uniform.handle, uniform.current_value.uint_value);
            break;
        case UNIFORM_LONG:
            glUniform1i(uniform.handle, uniform.current_value.long_value);
            break;
        case UNIFORM_FLOAT:
            glUniform1f(uniform.handle, uniform.current_value.float_value);
            break;
        case UNIFORM_DOUBLE:
            glUniform1d(uniform.handle, uniform.current_value.double_value);
            break;
        case UNIFORM_VEC2:
            glUniform2fv(uniform.handle, 1, (const GLfloat *) &uniform.current_value.vector2_value);
            break;
        case UNIFORM_VEC3:
            glUniform3fv(uniform.handle, 1, (const GLfloat *) &uniform.current_value.vector3_value);
            break;
        case UNIFORM_VEC4:
            glUniform4fv(uniform.handle, 1, (const GLfloat *) &uniform.current_value.vector4_value);
            break;
        case UNIFORM_MAT4:
            glUniformMatrix4fv(uniform.handle, 1, GL_FALSE, (const GLfloat *) &uniform.current_value.matrix_value);
            break;
        case UNIFORM_TEXTURE2D:
            glActiveTexture(uniform.current_value.texture_value.texture_target_index);
            glBindTexture(GL_TEXTURE_2D, uniform.current_value.texture_value.texture->handle);

            // texture target index is GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE{X}
            // But the parameter must be 0, 1, {x}. So we subtract the index from the GL_TEXTURE0
            // to get only the "{x}"
            glUniform1i(uniform.handle, uniform.current_value.texture_value.texture_target_index - GL_TEXTURE0);
            break;
    }
}

void gl::buff_uniforms(list<uniform_t> *uniforms) {
    for (int i = 0; i < uniforms->length; ++i) {
        buff_uniform(uniforms->items[i]);
    }
}

void gl::use_material(material_t *material) {
    glUseProgram(material->shader->handle);
    buff_uniforms(material->uniforms);
}

void set_vbo_enable_state(mesh_t *mesh, bool state) {
    if (mesh->vertex_position_handle > 0) {
        if (state)
            glEnableVertexAttribArray(mesh->vertex_position_handle);
        else
            glDisableVertexAttribArray(mesh->vertex_position_handle);
    }

    if (mesh->vertex_color_handle > 0) {
        if (state)
            glEnableVertexAttribArray(mesh->vertex_color_handle);
        else
            glDisableVertexAttribArray(mesh->vertex_color_handle);
    }

    if (mesh->vertex_tex_coord_handle > 0) {
        if (state)
            glEnableVertexAttribArray(mesh->vertex_tex_coord_handle);
        else
            glDisableVertexAttribArray(mesh->vertex_tex_coord_handle);
    }

    if (mesh->vertex_normal_handle > 0) {
        if (state)
            glEnableVertexAttribArray(mesh->vertex_normal_handle);
        else
            glDisableVertexAttribArray(mesh->vertex_normal_handle);
    }
}

void gl::draw_mesh(mesh_t *mesh) {
    glBindVertexArray(mesh->vao_handle);

    set_vbo_enable_state(mesh, true);

    if (mesh->indices_handle > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_handle);
        glDrawElements(GL_TRIANGLES, mesh->model->indices->length, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh->model->positions->length);
    }

    set_vbo_enable_state(mesh, false);

    glBindVertexArray(0);
}