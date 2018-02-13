#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <collections.h>
#include <graphics.h>
#include <maths.h>
#include <entities.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {

    GLFWwindow *window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Cynical Engine", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    char *vertex_shader_code = read_file_text("data/shaders/default_vertex_shader.glsl");
    char *fragment_shader_code = read_file_text("data/shaders/default_fragment_shader.glsl");

    shader_t *vertex_shader = create_shader(vertex_shader_code, SHADER_TYPE::VERTEX_SHADER);
    shader_t *frag_shader = create_shader(fragment_shader_code, SHADER_TYPE::FRAGMENT_SHADER);

    free_file_text(vertex_shader_code);
    free_file_text(fragment_shader_code);

    // Needs to set this so that stb_image loads the way opengl expects it!
    stbi_set_flip_vertically_on_load(true);

    image_t *image = create_image("data/textures/the_witness_small.png");
    image_t *image_large = create_image("data/textures/the_witness.png");

    texture_config_t config = get_default_texture_config();
    texture_t *texture = create_texture(image, config);
    texture_t *texture_large = create_texture(image_large, config);

    model_t *model = create_model("data/models/plane.cm");
    mesh_t *mesh = create_mesh(model);

    const int RENDER_QUANTITY = 10;

    shader_program_t *shader = create_shader_program(
            *vertex_shader,
            *frag_shader,
            VERTEX_POSITION_ATTRIBUTE_NAME,
            VERTEX_COLOR_ATTRIBUTE_NAME,
            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
            VERTEX_NORMAL_ATTRIBUTE_NAME
    );

    list<mesh_renderer_t *> *renderers = create_list<mesh_renderer_t *>(100);
    for (int j = 0; j < RENDER_QUANTITY; ++j) {

        uniform_definition_t mvp_uniform = {};
        mvp_uniform.name = (char *) "MVP";
        mvp_uniform.type = UNIFORM_TYPE::UNIFORM_MAT4;
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
                COMPARE_LESS,
                uniforms
        );

        mesh_renderer_t *renderer = create_mesh_renderer(material, mesh);
        add(renderers, renderer);
    }

    camera_t *perspective = create_perspective_camera(45.f, 0, .1f, 100.f);
    camera_t *ortho = create_ortho_camera(-1, 1, -1, 1, -100, 100);

    camera_t *camera = ortho;

    float dt = 0;
    float start_time = 0;
    float end_time = 0;
    while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_TAB)) {
            if (camera == perspective)
                camera = ortho;
            else
                camera = perspective;
        }

        dt = end_time - start_time;

        start_time = glfwGetTime();

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        if (camera == perspective)
            camera->perspective.aspect_ratio = ratio;

        for (int i = 0; i < renderers->length; ++i) {
            mesh_renderer_t *renderer = renderers->items[i];
            transform_t *transform = renderer->entity->transform;
            material_t *material = renderer->material;

            transform->position = glm::vec3(0, 0, -1);

            //transform->rotation = glm::quat();
            //transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 0, 1));
            //transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(1, 0, 0));
            //transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 1, 0));

            if (glfwGetKey(window, GLFW_KEY_W)) {
                camera->entity->transform->position.z -= dt;
            } else if (glfwGetKey(window, GLFW_KEY_S)) {
                camera->entity->transform->position.z += dt;
            } else if (glfwGetKey(window, GLFW_KEY_A)) {
                camera->entity->transform->position.x -= dt;
            } else if (glfwGetKey(window, GLFW_KEY_D)) {
                camera->entity->transform->position.x += dt;
            } else if (glfwGetKey(window, GLFW_KEY_8)) {
                camera->entity->transform->rotation *= glm::angleAxis(dt, world_right());
            } else if (glfwGetKey(window, GLFW_KEY_2)) {
                camera->entity->transform->rotation *= glm::angleAxis(dt, world_left());
            }

            update_transform_matrix(transform);
            update_camera_matrix(camera);
            glm::mat4 mvp = camera->_matrix * transform->_matrix;

            set_uniform_matrix(material, "MVP", mvp);

            if (glfwGetKey(window, GLFW_KEY_L)) {
                set_uniform_texture(material, "my_texture", texture_large);
            } else if (glfwGetKey(window, GLFW_KEY_M)) {
                set_uniform_texture(material, "my_texture", texture);
            }
        }

        draw_renderers(renderers);

        glfwSwapBuffers(window);
        glfwPollEvents();

        end_time = glfwGetTime();
    }

    for (int k = 0; k < renderers->length; ++k) {
        mesh_renderer_t *renderer = renderers->items[k];

        destroy_material(renderer->material);
        destroy_shader_program(renderer->material->shader);

        destroy_mesh_renderer(renderer);
    }

    destroy_shader(vertex_shader);
    destroy_shader(frag_shader);

    destroy_mesh(mesh);
    destroy_model(model);

    destroy_texture(texture);
    destroy_image(image);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
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