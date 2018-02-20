//#include <core.h>
//#include <graphics.h>
//#include <input.h>
//
//camera_t *perspective, *ortho_stencil, *ortho;
//list<mesh_renderer_t *> *renderers;
//texture_t *texture, *texture_large, *mask_texture;
//image_t *image, *image_large, *mask_image;
//shader_t *vertex_shader, *frag_shader, *geometry_shader;
//shader_program_t *shader;
//mesh_t *mesh;
//model_t *model;
//mesh_renderer_t *mask_renderer;
//
//void create_texture(const char *path, texture_t **texture, image_t **image) {
//    *image = create_image(path);
//    *texture = create_texture(*image, get_default_texture_config());
//}
//
//material_t *create_default_material() {
//    // TODO: read this form file
//    // TODO: read this form file
//    // TODO: read this form file
//    // TODO: read this form file
//
//    uniform_definition_t mvp_uniform = {};
//    mvp_uniform.name = (char *) "MVP";
//    mvp_uniform.type = UNIFORM_TYPE::UNIFORM_MATRIX;
//    mvp_uniform.default_value.matrix_value = glm::mat4();
//
//    uniform_definition_t my_texture = {};
//    my_texture.name = (char *) "my_texture";
//    my_texture.type = UNIFORM_TYPE::UNIFORM_TEXTURE2D;
//    my_texture.default_value.texture_value.texture = texture;
//    my_texture.default_value.texture_value.texture_target_index = TEXTURE_0;
//
//    uniform_definition_t tint = {};
//    tint.name = (char *) "tint";
//    tint.type = UNIFORM_TYPE::UNIFORM_VEC4;
//    tint.default_value.vector4_value = glm::vec4(1, 1, 1, 1);
//
//    uniform_definition_t offset = {};
//    offset.name = (char *) "offset";
//    offset.type = UNIFORM_TYPE::UNIFORM_VEC2;
//    offset.default_value.vector2_value = glm::vec2(.5, .5);
//
//    uniform_definition_t wrap = {};
//    wrap.name = (char *) "wrap";
//    wrap.type = UNIFORM_TYPE::UNIFORM_VEC2;
//    wrap.default_value.vector2_value = glm::vec2(1, 1);
//
//    list<uniform_definition_t> *uniforms = create_list<uniform_definition_t>(5);
//    add(uniforms, mvp_uniform);
//    add(uniforms, my_texture);
//    add(uniforms, tint);
//    add(uniforms, offset);
//    add(uniforms, wrap);
//
//    material_t *material = create_material(
//            shader,
//            uniforms
//    );
//
//    return material;
//}
//
//stencil_settings_t get_normal_camera_stencil_setting() {
//    stencil_settings_t settings;
//    settings.reference_value = 1;
//    settings.stencil_func_mask = 0xFF;
//    settings.compare_func = COMPARE_ALWAYS_TRUE;
//    settings.stencil_pass_depth_fail = STENCIL_OP_KEEP;
//    settings.stencil_fail = STENCIL_OP_KEEP;
//    settings.stencil_depth_pass = STENCIL_OP_REPLACE;
//    settings.stencil_mask = 0x00;
//    return settings;
//}
//
//void basic_setup() {
//    char *vertex_shader_code = read_file_text("data/shaders/default_vertex_shader.glsl");
//    char *fragment_shader_code = read_file_text("data/shaders/default_fragment_shader.glsl");
//    char *geometry_shader_code = read_file_text("data/shaders/default_geometry_shader.glsl");
//
//    vertex_shader = create_shader(vertex_shader_code, SHADER_TYPE::VERTEX_SHADER);
//    frag_shader = create_shader(fragment_shader_code, SHADER_TYPE::FRAGMENT_SHADER);
//    geometry_shader = create_shader(geometry_shader_code, SHADER_TYPE::GEOMETRY_SHADER);
//
//    free_file_content(vertex_shader_code);
//    free_file_content(fragment_shader_code);
//    free_file_content(geometry_shader_code);
//
//    // Needs to set this so that stb_image loads the way opengl expects it!
//    stbi_set_flip_vertically_on_load(true);
//
//    create_texture("data/textures/the_witness.png", &texture_large, &image_large);
//    create_texture("data/textures/the_witness_small.png", &texture, &image);
//    create_texture("data/textures/braid.png", &mask_texture, &mask_image);
//
//    model = create_model("data/models/plane.cm");
//    mesh = create_mesh(model);
//
//    shader = create_shader_program(
//            *vertex_shader,
//            *frag_shader,
//            *geometry_shader,
//            VERTEX_POSITION_ATTRIBUTE_NAME,
//            VERTEX_COLOR_ATTRIBUTE_NAME,
//            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
//            VERTEX_NORMAL_ATTRIBUTE_NAME
//    );
//}
//
//void setup() {
//    basic_setup();
//
//    const int RENDER_QUANTITY = 10;
//
//    material_t *mask_material = create_default_material();
//    set_uniform_texture(mask_material, "my_texture", mask_texture);
//    mask_renderer = create_mesh_renderer(mask_material, mesh);
//    mask_renderer->layer_mask = 1 << 2;
//    //mask_renderer->should_be_drawn = false;
//
//    renderers = create_list<mesh_renderer_t *>(100);
//    for (int j = 0; j < RENDER_QUANTITY; ++j) {
//
//        material_t *material = create_default_material();
//
//        mesh_renderer_t *renderer = create_mesh_renderer(material, mesh);
//        add(renderers, renderer);
//    }
//
//    /*
//    ortho_stencil = create_ortho_camera(-1, 1, 1, -1, -100, 100);
//    ortho_stencil->clear_mode = CAMERA_CLEAR_ALL;
//    ortho_stencil->culling_mask = 1 << 2;
//    //ortho_stencil->color_mask = get_color_mask_disabled();
//    //ortho_stencil->depth_buffer_status = DEPTH_BUFFER_DISABLED; // dont write to depth buffer
//
//    // This will replace the stencil value with 1
//    // since it will REPLACE the stencil value with the reference value
//    // regardless of the comparison (COMPARE_ALWAYS_TRUE)
//    ortho_stencil->stencil_settings.compare_func = COMPARE_ALWAYS_TRUE;
//    ortho_stencil->stencil_settings.reference_value = 1;
//    ortho_stencil->stencil_settings.stencil_depth_pass = STENCIL_OP_REPLACE;
//    ortho_stencil->stencil_settings.stencil_pass_depth_fail = STENCIL_OP_REPLACE;
//    ortho_stencil->stencil_settings.stencil_fail = STENCIL_OP_REPLACE;
//
//     */
//
//    perspective = create_perspective_camera(45.f, 0, .1f, 100.f);
//    perspective->clear_mode = CAMERA_CLEAR_COLOR_DEPTH;
//    //perspective->culling_mask = ((0xFF) & ~(1 << 2));
//
//    ortho = create_ortho_camera(-1, 1, 1, -1, -100, 100);
//    ortho->clear_mode = CAMERA_CLEAR_COLOR_DEPTH;
//    //ortho->culling_mask = ((0xFF) & ~(1 << 2));
//
//    glm::vec2 screen_size = get_screen_size();
//    perspective->full_screen = false;
//    perspective->view_port.position = glm::vec2(.5f, 0);
//    perspective->view_port.size = glm::vec2(.5f, .5f);
//
//    ortho->full_screen = false;
//    ortho->view_port.position = glm::vec2(0, 0);
//    ortho->view_port.size = glm::vec2(.5f, .5f);
//
//    perspective->clear_color = red() + green();
//    ortho->clear_color = blue() + green();
//}
//
//void load_texture_2() {
//    image_t *image = create_image("data/textures/braid.png");
//    texture_t *texture = create_texture(image, get_default_texture_config());
//
//    for (int i = 0; i < renderers->length; ++i) {
//        mesh_renderer_t *renderer = renderers->items[i];
//        material_t *material = renderer->material;
//        set_uniform_texture(material, "my_texture", texture);
//    }
//}
//
//static int layer = 0;
//
//void update_renderers(camera_t *camera) {
//    float dt = get_dt();
//
//    if (is_key_pressed(KEY_8)) {
//        load_texture_2();
//    }
//
//    if (is_key_down(KEY_W)) {
//        mask_renderer->entity->transform->position.y -= dt;
//    } else if (is_key_down(KEY_S)) {
//        mask_renderer->entity->transform->position.y += dt;
//    } else if (is_key_down(KEY_A)) {
//        mask_renderer->entity->transform->position.x -= dt;
//    } else if (is_key_down(KEY_D)) {
//        mask_renderer->entity->transform->position.x += dt;
//    }
//
//    for (int i = 0; i < renderers->length; ++i) {
//        mesh_renderer_t *renderer = renderers->items[i];
//        transform_t *transform = renderer->entity->transform;
//        material_t *material = renderer->material;
//
//        //transform->listener_pos = glm::vec3(0, 0, -1);
//
//        if (is_key_down(KEY_SPACE)) {
//            transform->rotation = glm::quat();
//            transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 0, 1));
//            transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i * 2), glm::vec3(1, 0, 0));
//            transform->rotation *= glm::angleAxis((float) (glfwGetTime() + i), glm::vec3(0, 1, 0));
//        }
//
//        if (is_key_down(KEY_KP_8)) {
//            camera->entity->transform->rotation *= glm::angleAxis(dt, world_right());
//        } else if (is_key_down(KEY_KP_2)) {
//            camera->entity->transform->rotation *= glm::angleAxis(dt, world_left());
//        }
//
//        if (is_key_down(KEY_KP_4)) {
//            camera->entity->transform->rotation *= glm::angleAxis(dt, world_up());
//        } else if (is_key_down(KEY_KP_6)) {
//            camera->entity->transform->rotation *= glm::angleAxis(dt, world_down());
//        }
//
//        if (is_key_down(KEY_RIGHT)) {
//            look_at(transform, world_right());
//        } else if (is_key_down(KEY_LEFT)) {
//            look_at(transform, world_left());
//        } else if (is_key_down(KEY_UP)) {
//            look_at(transform, world_up());
//        } else if (is_key_down(KEY_DOWN)) {
//            look_at(transform, world_down());
//        }
//
//        if (is_key_pressed(KEY_L)) {
//            set_uniform_texture(material, "my_texture", texture_large);
//        } else if (is_key_pressed(KEY_M)) {
//            set_uniform_texture(material, "my_texture", texture);
//        }
//
//        if (is_key_pressed(KEY_O)) {
//            material->cull_func = CULL_FRONT;
//        } else if (is_key_pressed(KEY_P)) {
//            material->cull_func = CULL_BACK;
//        } else if (is_key_pressed(KEY_R)) {
//            material->cull_func = CULL_DISABLED;
//        }
//
//        if (is_key_pressed(KEY_LEFT_BRACKET)) {
//            material->depth_func = COMPARE_LESS;
//        } else if (is_key_pressed(KEY_LEFT_BRACKET)) {
//            material->depth_func = COMPARE_GREATER;
//        } else if (is_key_pressed(KEY_SLASH)) {
//            material->depth_func = COMPARE_DISABLED;
//        }
//
//        transform->position.y += get_mouse_delta_scroll().y * 10 * get_dt();
//
//        if (is_mouse_button_down(MOUSE_BUTTON_LEFT)) {
//            transform->position.x -= 10 * get_dt();
//        } else if (is_mouse_button_down(MOUSE_BUTTON_RIGHT)) {
//            transform->position.x += 10 * get_dt();
//        }
//    }
//
//    color_mask_t mask;
//
//    mask.red = true;
//    mask.green = true;
//    mask.blue = true;
//    mask.alpha = true;
//
//    bool pressed = false;
//    if (is_key_pressed(KEY_1)) {
//        mask.red = false;
//        pressed = true;
//    } else if (is_key_pressed(KEY_2)) {
//        mask.green = false;
//        pressed = true;
//    } else if (is_key_pressed(KEY_3)) {
//        mask.blue = false;
//        pressed = true;
//    }
//
//    if (pressed)
//        set_color_mask(mask);
//
//    if (is_key_pressed(KEY_J)) {
//        set_depth_test_status(DEPTH_BUFFER_DISABLED);
//    } else if (is_key_pressed(KEY_H)) {
//        set_depth_test_status(DEPTH_BUFFER_ENABLED);
//    }
//
//    if (is_key_pressed(KEY_BACKSPACE)) {
//        MESSAGE("CLEAR ALL");
//        set_clear_color(black());
//        clear_view_port(get_screen_view_port(), CLEAR_COLOR_AND_DEPTH);
//    }
//}
//
//void update() {
//    glClear(GL_DEPTH_BUFFER_BIT);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    perspective->perspective.aspect_ratio = get_screen_ratio();
//
//    update_renderers(perspective);
//    update_renderers(ortho);
//}
//
//void close() {
//    for (int k = 0; k < renderers->length; ++k) {
//        mesh_renderer_t *renderer = renderers->items[k];
//
//        destroy_material(renderer->material);
//        destroy_mesh_renderer(renderer);
//    }
//
//    destroy_shader_program(shader);
//    destroy_shader(vertex_shader);
//    destroy_shader(frag_shader);
//
//    destroy_mesh(mesh);
//    destroy_model(model);
//
//    destroy_texture(texture);
//    destroy_image(image);
//
//    release();
//}
//
//void callback_at(void *payload) {
//    MESSAGEF((char *) payload);
//}
//
//void callback(void *payload) {
//    MESSAGEF((char *) payload);
//}
//
//void setup_fbo();
//
//void update_fbo();
//
//void close_fbo();
//
//camera_t *fbo_camera, *normal_camera;
//mesh_renderer_t *fbo_mesh, *normal_mesh;
//material_t *normal_material;
//
//void setup_fbo() {
//    char *vertex_shader_code = read_file_text("data/shaders/default_vertex_shader.glsl");
//    char *fragment_shader_code = read_file_text("data/shaders/default_fragment_shader.glsl");
//    char *geometry_shader_code = read_file_text("data/shaders/default_geometry_shader.glsl");
//
//    vertex_shader = create_shader(vertex_shader_code, SHADER_TYPE::VERTEX_SHADER);
//    frag_shader = create_shader(fragment_shader_code, SHADER_TYPE::FRAGMENT_SHADER);
//    geometry_shader = create_shader(geometry_shader_code, SHADER_TYPE::GEOMETRY_SHADER);
//
//    free_file_content(vertex_shader_code);
//    free_file_content(fragment_shader_code);
//    free_file_content(geometry_shader_code);
//
//    // Needs to set this so that stb_image loads the way opengl expects it!
//    stbi_set_flip_vertically_on_load(true);
//
//    create_texture("data/textures/the_witness.png", &texture, &image_large);
//    create_texture("data/textures/the_witness_small.png", &texture, &image);
//    create_texture("data/textures/braid.png", &texture, &mask_image);
//
//    model = create_model("data/models/plane.cm");
//    mesh = create_mesh(model);
//
//    shader = create_shader_program(
//            *vertex_shader,
//            *frag_shader,
//            *geometry_shader,
//            VERTEX_POSITION_ATTRIBUTE_NAME,
//            VERTEX_COLOR_ATTRIBUTE_NAME,
//            VERTEX_TEXTURE_COORD_ATTRIBUTE_NAME,
//            VERTEX_NORMAL_ATTRIBUTE_NAME
//    );
//
//    fbo_camera = create_ortho_camera(-1, 1, 1, -1, -100, 100);
//    normal_camera = create_ortho_camera(-1, 1, 1, -1, -100, 100);
//    normal_camera->clear_mode = CAMERA_CLEAR_NONE;
//
//    fbo_camera->target = create_frame_buffer(get_screen_size());
//    fbo_camera->culling_mask = 1 << 2;
//    fbo_camera->clear_color = red();
//
//    normal_camera->culling_mask = 0xFF & ~(1 << 2);
//    normal_camera->clear_color = blue();
//
//    material_t *fbo_material = create_default_material();
//    fbo_mesh = create_mesh_renderer(fbo_material, mesh);
//    fbo_mesh->layer_mask = 1 << 2;
//    set_uniform_vec2(fbo_material, "offset", glm::vec2(0, 0));
//    set_uniform_vec2(fbo_material, "wrap", glm::vec2(1, 1));
//    fbo_material->depth_func = COMPARE_ALWAYS_TRUE;
//
//    normal_material = create_default_material();
//    normal_mesh = create_mesh_renderer(fbo_material, mesh);
//    set_uniform_texture(normal_material, "my_texture", fbo_camera->target->color_texture);
//    set_uniform_vec2(normal_material, "offset", glm::vec2(0, 0));
//    set_uniform_vec2(normal_material, "wrap", glm::vec2(1, 1));
//
//    normal_mesh->should_be_drawn = false;
//}
//
//void update_fbo() {
//    float dt = get_dt();
//    if (is_key_down(KEY_W)) {
//        normal_mesh->entity->transform->position.y -= dt;
//    } else if (is_key_down(KEY_S)) {
//        normal_mesh->entity->transform->position.y += dt;
//    } else if (is_key_down(KEY_A)) {
//        normal_mesh->entity->transform->position.x -= dt;
//    } else if (is_key_down(KEY_D)) {
//        normal_mesh->entity->transform->position.x += dt;
//    }
//
//    if (is_key_down(KEY_UP)) {
//        fbo_mesh->entity->transform->position.y -= dt;
//    } else if (is_key_down(KEY_DOWN)) {
//        fbo_mesh->entity->transform->position.y += dt;
//    } else if (is_key_down(KEY_RIGHT)) {
//        fbo_mesh->entity->transform->position.x -= dt;
//    } else if (is_key_down(KEY_LEFT)) {
//        fbo_mesh->entity->transform->position.x += dt;
//    }
//
//    if (is_key_down(KEY_SPACE)) {
//        draw_renderer_with_material(normal_mesh, normal_material);
//    }
//
//    if (is_key_pressed(KEY_T)) {
//        normal_mesh->should_be_drawn = !normal_mesh->should_be_drawn;
//    }
//
//    if (normal_mesh != null)
//        normal_mesh->entity->transform->position.z += get_mouse_delta_scroll().y;
//}
//
//void close_fbo() {
//    destroy_shader_program(shader);
//    destroy_shader(vertex_shader);
//    destroy_shader(frag_shader);
//
//    destroy_mesh_renderer(normal_mesh);
//    destroy_mesh_renderer(fbo_mesh);
//
//    destroy_model(model);
//
//    destroy_texture(texture);
//    destroy_image(image);
//
//    release();
//}
//
//audio_source_t *source;
//
//void update_music() {
//    //AudioStreamUpdate(stream);
//    if (is_key_down(KEY_LEFT)) {
//        source->pitch -= 1 * get_dt();
//    } else if (is_key_down(KEY_RIGHT)) {
//        source->pitch += 1 * get_dt();
//    } else if (is_key_down(KEY_DOWN)) {
//        source->volume -= 1 * get_dt();
//    } else if (is_key_down(KEY_UP)) {
//        source->volume += 1 * get_dt();
//    }
//
//    float dt = get_dt();
//    if (is_key_down(KEY_W)) {
//        source->position.y -= dt;
//    } else if (is_key_down(KEY_S)) {
//        source->position.y += dt;
//    } else if (is_key_down(KEY_A)) {
//        source->position.x -= dt;
//    } else if (is_key_down(KEY_D)) {
//        source->position.x += dt;
//    }
//
//    if (is_key_pressed(KEY_SPACE)) {
//        if (source->state == SOURCE_PAUSED || source->state == SOURCE_STOPPED)
//            start_audio_source(source);
//        else
//            pause_audio_source(source);
//    }
//
//    if (is_key_pressed(KEY_ENTER)) {
//        if (source->state == SOURCE_STOPPED)
//            start_audio_source(source);
//        else
//            stop_audio_source(source);
//    }
//}
//
//mesh_renderer_t *wire_frame_render;
//material_t *wire_frame_material;
//
//void setup_wire_frame() {
//    basic_setup();
//
//    perspective = create_perspective_camera(45, get_screen_ratio(), 0.1f, 100);
//    perspective->clear_color = green();
//
//    wire_frame_material = create_default_material();
//    wire_frame_render = create_mesh_renderer(wire_frame_material, mesh);
//    set_uniform_texture(wire_frame_material, "my_texture", texture_large);
//
//    wire_frame_render->entity->transform->position.z = -10;
//}
//
//int index = 0;
//
//void update_wire_frame() {
//    POLYGON_MODE modes[3] = {POLYGON_POINT, POLYGON_LINE, POLYGON_FILL};
//    if (is_key_pressed(KEY_SPACE)) {
//        set_polygon_mode(modes[index]);
//        index = (index + 1) % 3;
//    }
//
//    if (is_key_down(KEY_LEFT)) {
//        set_line_width(get_graphics_state().line_width - 1);
//    } else if (is_key_down(KEY_RIGHT)) {
//        set_line_width(get_graphics_state().line_width + 1);
//    } else if (is_key_down(KEY_DOWN)) {
//        set_point_size(get_graphics_state().point_size - 1);
//    } else if (is_key_down(KEY_UP)) {
//        set_point_size(get_graphics_state().point_size + 1);
//    }
//}
//
//const char *vertex_shader_code =
//        "#version 150\n"
//        "in vec3 position;\n"
//        "in vec2 uv_in;\n"
//        "out vec2 uv;\n"
//        "void main() {\n"
//        "gl_Position = vec4(position, 1);\n"
//        "uv = uv_in;\n"
//        "}\n";
//
//const char *fragment_shader_code =
//        "#version 150\n"
//        "out vec4 color;\n"
//        "in vec2 uv;\n"
//        "uniform sampler2D tex;\n"
//        "void main() {\n"
//        "color = texture(tex, uv);\n"
//        "}\n";
//
//int main(void) {
///*
//    // TODO: Read this from file
//    engine_params_t params;
//    params.window_title = (char *) "My game!!!";
//    params.window_size = glm::ivec2(1024, 768);
//    params.update_callback = &update_music;
//    params.gl_major_version = 4;
//    params.gl_minor_version = 0;
//
//    prepare(params);
//
//    music_t *music = create_music("data/sounds/test_music.ogg");
//    source = create_audio_source();
//    set_music_on_source(source, music);
//    start_audio_source(source);
//
//    loop();
//
//    release();
//    */
//
//    GLfloat quad[18] = {
//            -.5f, .5f, 0,
//            -.5f, -.5f, 0,
//            .5f, .5f, 0,
//            .5f, -.5f, 0
//    };
//
//    GLfloat uv[12] = {
//            0, 1,
//            0, 0,
//            1, 1,
//            1, 0,
//    };
//
//    GLint indices[6] = {
//            0,
//            1,
//            2,
//            2,
//            3,
//            1
//    };
//
//    if (!glfwInit()) {
//        return -1;
//    }
//
//    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_VERSION_MINOR, 4);
//
//    GLFWwindow *window = glfwCreateWindow(800, 600, "Window", null, null);
//
//    if (window == null) {
//        return -2;
//    }
//
//    glfwMakeContextCurrent(window);
//
//    glewInit();
//
//    glClearColor(1, 0, 0, 0);
//
//    uint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    MESSAGE("OK 1");
//
//    uint vbo;
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(
//            GL_ARRAY_BUFFER,
//            sizeof(GLfloat) * 18,
//            quad,
//            GL_STATIC_DRAW
//    );
//    CHECK_GL_ERROR();
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, null);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    uint vbo_uv;
//    glGenBuffers(1, &vbo_uv);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
//    glBufferData(
//            GL_ARRAY_BUFFER,
//            sizeof(GLfloat) * 12,
//            uv,
//            GL_STATIC_DRAW
//    );
//    CHECK_GL_ERROR();
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, null);
//
//    MESSAGE("OK 3");
//
//    uint indice_vbo;
//    glGenBuffers(1, &indice_vbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_vbo);
//    glBufferData(
//            GL_ELEMENT_ARRAY_BUFFER,
//            sizeof(GLint) * 6,
//            indices,
//            GL_STATIC_DRAW
//    );
//
//    glBindVertexArray(0);
//
//    uint vertex = glCreateShader(GL_VERTEX_SHADER);
//    uint fragment = glCreateShader(GL_FRAGMENT_SHADER);
//
//    MESSAGE("OK 4");
//
//    glShaderSource(vertex, 1, &vertex_shader_code, null);
//    glCompileShader(vertex);
//    CHECK_SHADER_COMPILE_STATUS(vertex, vertex_shader_code);
//
//    MESSAGE("OK 5");
//
//    glShaderSource(fragment, 1, &fragment_shader_code, null);
//    glCompileShader(fragment);
//    CHECK_SHADER_COMPILE_STATUS(fragment, fragment_shader_code);
//
//    MESSAGE("OK 5");
//
//    uint shader = glCreateProgram();
//    glAttachShader(shader, vertex);
//    glAttachShader(shader, fragment);
//
//    glBindAttribLocation(shader, 0, "position");
//    glBindAttribLocation(shader, 1, "uv");
//
//    glLinkProgram(shader);
//    CHECK_GL_ERROR();
//
//    MESSAGE("OK 6");
//
//    glUseProgram(shader);
//
//    stbi_set_flip_vertically_on_load(true);
//    int width, height, channels;
//    stbi_uc *tex_data = stbi_load("data/textures/the_witness.png", &width, &height, &channels, 4);
//
//    uint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    CHECK_GL_ERROR();
//
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    uint tex_uniform = glGetUniformLocation(shader, "tex");
//    glUniform1i(tex_uniform, 1);
//
//    CHECK_GL_ERROR();
//
//    while (!glfwWindowShouldClose(window)) {
//        glClear(GL_COLOR_BUFFER_BIT);
//
//
//        glBindVertexArray(vao);
//        glEnableVertexAttribArray(0);
//        glEnableVertexAttribArray(1);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, null);
//        glBindVertexArray(0);
//        CHECK_GL_ERROR();
//
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//}