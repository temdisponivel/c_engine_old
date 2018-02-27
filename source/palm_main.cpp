//
// Created by temdisponivel on 19/02/2018.
//

#include "engine.h"
#include <core.h>
#include <graphics.h>
#include <input.h>
#include <editor.h>

mesh_renderer_t *renderer;
shader_program_t *shader;
material_t *bark_material, *leaf_material;
texture_t *texture, *leafs_texture;
camera_t *camera;

void fill_uniforms(list<uniform_definition_t> *uniforms) {
    uniform_definition_t tint = {};
    tint.type = UNIFORM_VEC4;
    tint.name = (char *) "tint";
    tint.default_value.vector4_value = glm::vec4(1, 1, 1, 1);
    add(uniforms, tint);

    uniform_definition_t offset = {};
    offset.type = UNIFORM_VEC2;
    offset.name = (char *) "offset";
    offset.default_value.vector2_value = glm::vec2(0, 0);
    add(uniforms, offset);

    uniform_definition_t wrap = {};
    wrap.type = UNIFORM_VEC2;
    wrap.name = (char *) "wrap";
    wrap.default_value.vector2_value = glm::vec2(1, 1);
    add(uniforms, wrap);
}

void create_scene() {
    list<model_t *> *model = create_model_from_obj_file("data/models/PowerLoader.obj");

    char *vertex_shader_code = read_file_text("data/shaders/default_vertex_shader.glsl");
    shader_t vertex = create_shader(vertex_shader_code, VERTEX_SHADER);

    char *fragment_shader_code = read_file_text("data/shaders/default_fragment_shader.glsl");
    shader_t fragment = create_shader(fragment_shader_code, FRAGMENT_SHADER);

    shader = create_shader_program_ex(
            vertex,
            fragment,
            {},
            VERTEX_POSITION_ATTRIBUTE_NAME,
            VERTEX_COLOR_ATTRIBUTE_NAME,
            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
            VERTEX_NORMAL_ATTRIBUTE_NAME
    );

    char* paths[] = {
            "data/textures/powerloader/PowerLoaderGrate_D.tga",
            "data/textures/powerloader/PowerLoaderLower_D.tga",
            "data/textures/powerloader/PowerLoaderUpper_D.tga",
    };

    material_t *materials[3];

    for (int j = 0; j < 3; ++j) {
        char *path = paths[j];

        image_t *image = create_image(path);
        texture = create_texture(image, get_default_texture_config());


        list<uniform_definition_t> *uniforms = create_list<uniform_definition_t>(5);

        uniform_definition_t bark_texture_uniform = {};
        bark_texture_uniform.type = UNIFORM_TEXTURE2D;
        bark_texture_uniform.name = (char *) "my_texture";
        bark_texture_uniform.default_value.texture_value.texture_target_index = TEXTURE_0;
        bark_texture_uniform.default_value.texture_value.texture = texture;
        add(uniforms, bark_texture_uniform);

        fill_uniforms(uniforms);

        materials[j] = create_material(shader, uniforms);
    }
    list<mesh_t *> *meshs = create_list<mesh_t *>(model->length);
    for (int i = 0; i < model->length; ++i) {
        material_t *material;
        material = materials[i % 3];
        mesh_t *mesh = create_mesh(model->items[i], material);
        add(meshs, mesh);
    }

    renderer = create_mesh_renderer(meshs);
    renderer->entity->transform->position.z -= 10;

    camera = create_perspective_camera(45, get_screen_ratio(), 0.1f, 1000.f);
    camera->full_screen = true;
}

void update_palm() {
    update_editor_camera();
}

int main() {
    // TODO: Read this from file
    engine_params_t params;
    params.window_title = (char *) "My game!!!";
    params.window_size = glm::ivec2(1024, 768);
    params.update_callback = &update_palm;
    params.gl_major_version = 4;
    params.gl_minor_version = 0;

    prepare(params);

    create_scene();
    setup_editor(camera);

    loop();

    release_editor();
    release();

    return 1;
}