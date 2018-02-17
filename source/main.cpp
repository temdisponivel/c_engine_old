#include <core.h>
#include <graphics.h>
#include <input.h>

camera_t *perspective, *ortho_stencil, *ortho;
list<mesh_renderer_t *> *renderers;
texture_t *texture, *texture_large, *mask_texture;
image_t *image, *image_large, *mask_image;
shader_t *vertex_shader, *frag_shader, *geometry_shader;
shader_program_t *shader;
mesh_t *mesh;
model_t *model;
mesh_renderer_t *mask_renderer;

void create_texture(const char *path, texture_t **texture, image_t **image) {
    *image = create_image(path);
    *texture = create_texture(*image, get_default_texture_config());
}

material_t *create_default_material() {
    // TODO: read this form file
    // TODO: read this form file
    // TODO: read this form file
    // TODO: read this form file

    uniform_definition_t mvp_uniform = {};
    mvp_uniform.name = (char *) "MVP";
    mvp_uniform.type = UNIFORM_TYPE::UNIFORM_MATRIX;
    mvp_uniform.default_value.matrix_value = glm::mat4();

    uniform_definition_t my_texture = {};
    my_texture.name = (char *) "my_texture";
    my_texture.type = UNIFORM_TYPE::UNIFORM_TEXTURE2D;
    my_texture.default_value.texture_value.texture = texture;
    my_texture.default_value.texture_value.texture_target_index = TEXTURE_0;

    uniform_definition_t tint = {};
    tint.name = (char *) "tint";
    tint.type = UNIFORM_TYPE::UNIFORM_VEC4;
    tint.default_value.vector4_value = glm::vec4(1, 1, 1, 1);

    uniform_definition_t offset = {};
    offset.name = (char *) "offset";
    offset.type = UNIFORM_TYPE::UNIFORM_VEC2;
    offset.default_value.vector2_value = glm::vec2(.5, .5);

    uniform_definition_t wrap = {};
    wrap.name = (char *) "wrap";
    wrap.type = UNIFORM_TYPE::UNIFORM_VEC2;
    wrap.default_value.vector2_value = glm::vec2(1, 1);

    list<uniform_definition_t> *uniforms = create_list<uniform_definition_t>(5);
    add(uniforms, mvp_uniform);
    add(uniforms, my_texture);
    add(uniforms, tint);
    add(uniforms, offset);
    add(uniforms, wrap);

    material_t *material = create_material(
            shader,
            uniforms
    );

    return material;
}

stencil_settings_t get_normal_camera_stencil_setting() {
    stencil_settings_t settings;
    settings.reference_value = 1;
    settings.stencil_func_mask = 0xFF;
    settings.compare_func = COMPARE_EQUAL;
    settings.stencil_pass_depth_fail = STENCIL_OP_KEEP;
    settings.stencil_fail = STENCIL_OP_KEEP;
    settings.stencil_depth_pass = STENCIL_OP_REPLACE;
    settings.stencil_mask = 0x00;
    return settings;
}

void setup() {
    char *vertex_shader_code = read_file_text("data/shaders/default_vertex_shader.glsl");
    char *fragment_shader_code = read_file_text("data/shaders/default_fragment_shader.glsl");
    char *geometry_shader_code = read_file_text("data/shaders/default_geometry_shader.glsl");

    vertex_shader = create_shader(vertex_shader_code, SHADER_TYPE::VERTEX_SHADER);
    frag_shader = create_shader(fragment_shader_code, SHADER_TYPE::FRAGMENT_SHADER);
    geometry_shader = create_shader(geometry_shader_code, SHADER_TYPE::GEOMETRY_SHADER);

    free_file_text(vertex_shader_code);
    free_file_text(fragment_shader_code);
    free_file_text(geometry_shader_code);

    // Needs to set this so that stb_image loads the way opengl expects it!
    stbi_set_flip_vertically_on_load(true);

    create_texture("data/textures/the_witness.png", &texture_large, &image_large);
    create_texture("data/textures/the_witness_small.png", &texture, &image);
    create_texture("data/textures/braid.png", &mask_texture, &mask_image);

    model = create_model("data/models/plane.cm");
    mesh = create_mesh(model);

    const int RENDER_QUANTITY = 10;

    shader = create_shader_program(
            *vertex_shader,
            *frag_shader,
            *geometry_shader,
            VERTEX_POSITION_ATTRIBUTE_NAME,
            VERTEX_COLOR_ATTRIBUTE_NAME,
            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
            VERTEX_NORMAL_ATTRIBUTE_NAME
    );

    material_t *mask_material = create_default_material();
    set_uniform_texture(mask_material, "my_texture", mask_texture);
    mask_renderer = create_mesh_renderer(mask_material, mesh);
    mask_renderer->layer_mask = 1 << 2;
    //mask_renderer->should_be_drawn = false;

    renderers = create_list<mesh_renderer_t *>(100);
    for (int j = 0; j < RENDER_QUANTITY; ++j) {

        material_t *material = create_default_material();

        mesh_renderer_t *renderer = create_mesh_renderer(material, mesh);
        add(renderers, renderer);
    }

    ortho_stencil = create_ortho_camera(-1, 1, 1, -1, -100, 100);
    ortho_stencil->clear_mode = CAMERA_CLEAR_ALL;
    ortho_stencil->culling_mask = 1 << 2;
    //ortho_stencil->color_mask = get_color_mask_disabled();
    //ortho_stencil->depth_buffer_status = DEPTH_BUFFER_DISABLED; // dont write to depth buffer

    // This will replace the stencil value with 1
    // since it will REPLACE the stencil value with the reference value
    // regardless of the comparison (COMPARE_ALWAYS_TRUE)
    ortho_stencil->stencil_settings.compare_func = COMPARE_ALWAYS_TRUE;
    ortho_stencil->stencil_settings.reference_value = 1;
    ortho_stencil->stencil_settings.stencil_depth_pass = STENCIL_OP_REPLACE;
    ortho_stencil->stencil_settings.stencil_pass_depth_fail = STENCIL_OP_REPLACE;
    ortho_stencil->stencil_settings.stencil_fail = STENCIL_OP_REPLACE;

    perspective = create_perspective_camera(45.f, 0, .1f, 100.f);
    perspective->stencil_settings = get_normal_camera_stencil_setting();
    perspective->clear_mode = CAMERA_CLEAR_NONE;
    perspective->culling_mask = ((0xFF) & ~(1 << 2));

    ortho = create_ortho_camera(-1, 1, 1, -1, -100, 100);
    ortho->stencil_settings = get_normal_camera_stencil_setting();
    ortho->clear_mode = CAMERA_CLEAR_NONE;
    ortho->culling_mask = ((0xFF) & ~(1 << 2));

    //perspective->clear_mode = CAMERA_CLEAR_NONE;
    //ortho->clear_mode = CAMERA_CLEAR_NONE;

    glm::vec2 screen_size = get_screen_size();
    perspective->full_screen = false;
    perspective->view_port.position = glm::ivec2(1024 / 2, 0);
    perspective->view_port.size = glm::ivec2(screen_size.x / 2, screen_size.y / 2);

    ortho->full_screen = false;
    ortho->view_port.position = glm::ivec2(0, 0);
    ortho->view_port.size = glm::ivec2(screen_size.x / 2, screen_size.y / 2);

    perspective->clear_color = red() + green();
    ortho->clear_color = blue() + green();
}

void load_texture_2() {
    image_t *image = create_image("data/textures/braid.png");
    texture_t *texture = create_texture(image, get_default_texture_config());

    for (int i = 0; i < renderers->length; ++i) {
        mesh_renderer_t *renderer = renderers->items[i];
        material_t *material = renderer->material;
        set_uniform_texture(material, "my_texture", texture);
    }
}

static int layer = 0;

void update_renderers(camera_t *camera) {
    float dt = get_dt();

    if (is_key_pressed(KEY_8)) {
        load_texture_2();
    }

    if (is_key_down(KEY_W)) {
        mask_renderer->entity->transform->position.y -= dt;
    } else if (is_key_down(KEY_S)) {
        mask_renderer->entity->transform->position.y += dt;
    } else if (is_key_down(KEY_A)) {
        mask_renderer->entity->transform->position.x -= dt;
    } else if (is_key_down(KEY_D)) {
        mask_renderer->entity->transform->position.x += dt;
    }

    for (int i = 0; i < renderers->length; ++i) {
        mesh_renderer_t *renderer = renderers->items[i];
        transform_t *transform = renderer->entity->transform;
        material_t *material = renderer->material;

        //transform->position = glm::vec3(0, 0, -1);

        if (is_key_down(KEY_SPACE)) {
            transform->rotation = glm::quat();
            transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 0, 1));
            transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i * 2), glm::vec3(1, 0, 0));
            transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 1, 0));
        }

        if (is_key_down(KEY_KP_8)) {
            camera->entity->transform->rotation *= glm::angleAxis(dt, world_right());
        } else if (is_key_down(KEY_KP_2)) {
            camera->entity->transform->rotation *= glm::angleAxis(dt, world_left());
        }

        if (is_key_down(KEY_KP_4)) {
            camera->entity->transform->rotation *= glm::angleAxis(dt, world_up());
        } else if (is_key_down(KEY_KP_6)) {
            camera->entity->transform->rotation *= glm::angleAxis(dt, world_down());
        }

        if (is_key_down(KEY_RIGHT)) {
            look_at(transform, world_right());
        } else if (is_key_down(KEY_LEFT)) {
            look_at(transform, world_left());
        } else if (is_key_down(KEY_UP)) {
            look_at(transform, world_up());
        } else if (is_key_down(KEY_DOWN)) {
            look_at(transform, world_down());
        }

        if (is_key_pressed(KEY_L)) {
            set_uniform_texture(material, "my_texture", texture_large);
        } else if (is_key_pressed(KEY_M)) {
            set_uniform_texture(material, "my_texture", texture);
        }

        if (is_key_pressed(KEY_O)) {
            material->cull_func = CULL_FRONT;
        } else if (is_key_pressed(KEY_P)) {
            material->cull_func = CULL_BACK;
        } else if (is_key_pressed(KEY_R)) {
            material->cull_func = CULL_DISABLED;
        }

        if (is_key_pressed(KEY_LEFT_BRACKET)) {
            material->depth_func = COMPARE_LESS;
        } else if (is_key_pressed(KEY_LEFT_BRACKET)) {
            material->depth_func = COMPARE_GREATER;
        } else if (is_key_pressed(KEY_SLASH)) {
            material->depth_func = COMPARE_DISABLED;
        }

        transform->position.y += get_mouse_delta_scroll().y * 10 * get_dt();

        if (is_mouse_button_down(MOUSE_BUTTON_LEFT)) {
            transform->position.x -= 10 * get_dt();
        } else if (is_mouse_button_down(MOUSE_BUTTON_RIGHT)) {
            transform->position.x += 10 * get_dt();
        }
    }

    color_mask_t mask;

    mask.red = true;
    mask.green = true;
    mask.blue = true;
    mask.alpha = true;

    bool pressed = false;
    if (is_key_pressed(KEY_1)) {
        mask.red = false;
        pressed = true;
    } else if (is_key_pressed(KEY_2)) {
        mask.green = false;
        pressed = true;
    } else if (is_key_pressed(KEY_3)) {
        mask.blue = false;
        pressed = true;
    }

    if (pressed)
        set_color_mask(mask);

    if (is_key_pressed(KEY_J)) {
        set_depth_test_status(DEPTH_BUFFER_DISABLED);
    } else if (is_key_pressed(KEY_H)) {
        set_depth_test_status(DEPTH_BUFFER_ENABLED);
    }

    if (is_key_pressed(KEY_BACKSPACE)) {
        MESSAGE("CLEAR ALL");
        set_clear_color(black());
        clear_view_port(get_screen_view_port(), CLEAR_COLOR_AND_DEPTH);
    }
}

void update() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    perspective->perspective.aspect_ratio = get_window_ratio();

    update_renderers(perspective);
    update_renderers(ortho);
}

void close() {
    for (int k = 0; k < renderers->length; ++k) {
        mesh_renderer_t *renderer = renderers->items[k];

        destroy_material(renderer->material);
        destroy_mesh_renderer(renderer);
    }

    destroy_shader_program(shader);
    destroy_shader(vertex_shader);
    destroy_shader(frag_shader);

    destroy_mesh(mesh);
    destroy_model(model);

    destroy_texture(texture);
    destroy_image(image);

    release();
}

void callback_at(void *payload) {
    MESSAGEF((char *) payload);
}

void callback(void *payload) {
    MESSAGEF((char *) payload);
}

int main(void) {
    // TODO: Read this from file
    engine_params_t params;
    params.window_title = (char *) "My game!!!";
    params.window_size = glm::ivec2(1024, 768);
    params.update_callback = &update;
    params.gl_major_version = 4;
    params.gl_minor_version = 0;

    prepare(params);

    call_after(5, &callback, (void *) "call_after 5 seconds\n", false);
    call_at(15, &callback_at, (void *) "call_at 15 seconds\n", false);

    setup();

    loop();

    close();
}

/*
// NOTE: OpenGL error checks have been omitted for brevity
glGenBuffers(1, &vertex_buffer);
glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
vertex_shader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
glCompileShader(vertex_shader);
fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
glCompileShader(fragment_shader);
program = glCreateProgram();
glAttachShader(program, vertex_shader);
glAttachShader(program, fragment_shader);
glLinkProgram(program);
program = glCreateProgram();
glAttachShader(program, vertex_shader);
glAttachShader(program, fragment_shader);
glLinkProgram(program);
program = glCreateProgram();
glAttachShader(program, vertex_shader);
glAttachShader(program, fragment_shader);
glLinkProgram(program);
program = glCreateProgram();
glAttachShader(program, vertex_shader);
glAttachShader(program, fragment_shader);
glLinkProgram(program);
program = glCreateProgram();
glAttachShader(program, vertex_shader);
glAttachShader(program, fragment_shader);
glLinkProgram(program);
mvp_location = glGetUniformLocation(program, "MVP");
vpos_location = glGetAttribLocation(program, "vPos");
vcol_location = glGetAttribLocation(program, "vCol");
glEnableVertexAttribArray(vpos_location);
glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                      sizeof(float) * 5, (void*) 0);
glEnableVertexAttribArray(vcol_location);
glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                      sizeof(float) * 5, (void*) (sizeof(float) * 2));

while (!glfwWindowShouldClose(window))
{
    float ratio;
    int width, height;
    glm::mat4 m, p, v, mvp;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    m = glm::mat4();
    m = glm::rotate(m, (float) (glm::radians(30 * glfwGetTime())), glm::vec3(0, 0, 1));
    m = glm::translate(m, glm::vec3(0, 0, -10));

    p = glm::perspective(glm::radians(45.f), ratio, 0.1f, 100.f);
    v = glm::lookAt(glm::vec3(), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

    mvp = p * v * m;

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (GLfloat *) &mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
}
glfwDestroyWindow(window);
glfwTerminate();
exit(EXIT_SUCCESS);
}*/