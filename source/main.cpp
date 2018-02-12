#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <collections.h>
#include <graphics.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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

    shader_t *vertex_shader = gl::create_shader(vertex_shader_code, SHADER_TYPE::VERTEX_SHADER);
    shader_t *frag_shader = gl::create_shader(fragment_shader_code, SHADER_TYPE::FRAGMENT_SHADER);

    free_file_text(vertex_shader_code);
    free_file_text(fragment_shader_code);

    shader_program_t *shader = gl::create_shader_program(
            *vertex_shader,
            *frag_shader,
            VERTEX_POSITION_ATTRIBUTE_NAME,
            VERTEX_COLOR_ATTRIBUTE_NAME,
            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
            VERTEX_NORMAL_ATTRIBUTE_NAME
    );

    image_t *image = gl::create_image("data/textures/the_witness.png");

    texture_config_t config = gl::get_default_texture_config();
    texture_t *texture = gl::create_texture(image, config);

    uniform_definition_t mvp_uniform = {};
    mvp_uniform.name = (char *) "MVP";
    mvp_uniform.type = UNIFORM_TYPE::UNIFORM_MAT4;
    mvp_uniform.default_value.matrix_value = glm::mat4();

    uniform_definition_t my_texture = {};
    my_texture.name = (char *) "my_texture";
    my_texture.type = UNIFORM_TYPE::UNIFORM_TEXTURE2D;
    my_texture.default_value.texture_value.texture = texture;
    my_texture.default_value.texture_value.texture_target_index = TEXTURE_0;

    uniform_definition_t  tint = {};
    tint.name = (char *) "tint";
    tint.type = UNIFORM_TYPE::UNIFORM_VEC4;
    tint.default_value.vector4_value = glm::vec4(1, 1, 1, 1);

    uniform_definition_t  offset = {};
    offset.name = (char *) "offset";
    offset.type = UNIFORM_TYPE::UNIFORM_VEC2;
    offset.default_value.vector2_value = glm::vec2(0, 0);

    uniform_definition_t  wrap = {};
    wrap.name = (char *) "wrap";
    wrap.type = UNIFORM_TYPE::UNIFORM_VEC2;
    wrap.default_value.vector2_value = glm::vec2(1, 1);

    list<uniform_definition_t> *uniforms = lists::create<uniform_definition_t>(5);
    lists::add(uniforms, mvp_uniform);
    lists::add(uniforms, my_texture);
    lists::add(uniforms, tint);
    lists::add(uniforms, offset);
    lists::add(uniforms, wrap);

    material_t *material = gl::create_material(
            shader,
            uniforms
    );

    model_t *model = gl::create_model("data/models/plane.cm");
    mesh_t *mesh = gl::create_mesh(model);

    uniform_t *mvp_uni = gl::find_uniform_by_name("MVP", material);
    ENSURE(mvp_uni != null);

    while (!glfwWindowShouldClose(window)) {

        float ratio;
        int width, height;
        glm::mat4 m, r, t, s, p, v, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        m = glm::mat4();

        m = glm::translate(m, glm::vec3(0, 0, -10));;

//        m = glm::rotate(m, (float) (glfwGetTime()), glm::vec3(0, 0, 1));
//        m = glm::rotate(m, (float) (glfwGetTime()), glm::vec3(1, 0, 0));
//        m = glm::rotate(m, (float) (glfwGetTime()), glm::vec3(0, 1, 0));

        m = glm::scale(m, glm::vec3(2, 2, 2));

        p = glm::perspective(glm::radians(45.f), ratio, 0.1f, 100.f);
        v = glm::lookAt(glm::vec3(), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

        mvp = p * v * m;

        if (mvp_uni != null)
            mvp_uni->current_value.matrix_value = mvp;

        //print_mat4(material->uniforms->items[0]->current_value.matrix_value);

        gl::use_material(material);
        gl::draw_mesh(mesh);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gl::destroy_material(material);

    gl::destroy_shader_program(shader);
    gl::destroy_shader(vertex_shader);
    gl::destroy_shader(frag_shader);

    gl::destroy_mesh(mesh);
    gl::destroy_model(model);

    gl::destroy_texture(texture);
    gl::destroy_image(image);

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