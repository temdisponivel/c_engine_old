#include <core.h>
#include <graphics.h>
#include <input.h>

camera_t *perspective, *ortho;
list<mesh_renderer_t *> *renderers;
texture_t *texture, *texture_large;
shader_t *vertex_shader, *frag_shader;
shader_program_t *shader;
mesh_t *mesh;
model_t *model;
image_t *image, *image_large;

void setup() {
    char *vertex_shader_code = read_file_text("data/shaders/default_vertex_shader.glsl");
    char *fragment_shader_code = read_file_text("data/shaders/default_fragment_shader.glsl");

    vertex_shader = create_shader(vertex_shader_code, SHADER_TYPE::VERTEX_SHADER);
    frag_shader = create_shader(fragment_shader_code, SHADER_TYPE::FRAGMENT_SHADER);

    free_file_text(vertex_shader_code);
    free_file_text(fragment_shader_code);

    // Needs to set this so that stb_image loads the way opengl expects it!
    stbi_set_flip_vertically_on_load(true);

    image = create_image("data/textures/the_witness_small.png");
    image_large = create_image("data/textures/the_witness.png");

    texture_config_t config = get_default_texture_config();
    texture = create_texture(image, config);
    texture_large = create_texture(image_large, config);

    model = create_model("data/models/plane.cm");
    mesh = create_mesh(model);

    const int RENDER_QUANTITY = 10;

    shader = create_shader_program(
            *vertex_shader,
            *frag_shader,
            VERTEX_POSITION_ATTRIBUTE_NAME,
            VERTEX_COLOR_ATTRIBUTE_NAME,
            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
            VERTEX_NORMAL_ATTRIBUTE_NAME
    );

    renderers = create_list<mesh_renderer_t *>(100);
    for (int j = 0; j < RENDER_QUANTITY; ++j) {

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

        material->cull_func = CULL_BACK;

        mesh_renderer_t *renderer = create_mesh_renderer(material, mesh);
        add(renderers, renderer);
    }

    perspective = create_perspective_camera(45.f, 0, .1f, 100.f);
    ortho = create_ortho_camera(-1, 1, 1, -1, -100, 100);

    //perspective->clear_mode = CAMERA_CLEAR_NONE;
    //ortho->clear_mode = CAMERA_CLEAR_NONE;

    glm::vec2 screen_size = get_screen_size();
    perspective->full_screen = false;
    perspective->view_port.position = glm::ivec2(1024/2, 0);
    perspective->view_port.size = glm::ivec2(screen_size.x / 2, screen_size.y / 2);

    ortho->full_screen = false;
    ortho->view_port.position = glm::ivec2(0, 0);
    ortho->view_port.size = glm::ivec2(screen_size.x / 2, screen_size.y / 2);

    perspective->clear_color = red() + green();
    ortho->clear_color = blue() + green();
}

void update_renderers(camera_t *camera) {
    float dt = get_dt();

    for (int i = 0; i < renderers->length; ++i) {
        mesh_renderer_t *renderer = renderers->items[i];
        transform_t *transform = renderer->entity->transform;
        material_t *material = renderer->material;

        //transform->position = glm::vec3(0, 0, -1);

        transform->rotation = glm::quat();
        transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 0, 1));
        transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i*2), glm::vec3(1, 0, 0));
        transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 1, 0));

        if (is_key_down(KEY_W)) {
            camera->entity->transform->position.z -= dt;
        } else if (is_key_down(KEY_S)) {
            camera->entity->transform->position.z += dt;
        } else if (is_key_down(KEY_A)) {
            camera->entity->transform->position.x -= dt;
        } else if (is_key_down(KEY_D)) {
            camera->entity->transform->position.x += dt;
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
        } else if (is_key_pressed(KEY_R)) {
            look_at(transform, world_forward());
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

int main(void) {
    // TODO: Read this from file
    engine_params_t params;
    params.window_title = (char *) "My game!!!";
    params.window_size = glm::ivec2(1024, 768);
    params.update_callback = &update;
    params.gl_major_version = 4;
    params.gl_minor_version = 0;

    prepare(params);

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