#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// A callback to change the size of the viewport when the framebuffer size is
// changed
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// Process any user input that occurs in the main loop
void process_input(GLFWwindow *window);

// Initialise the window, initialise GLAD, set the OpenGL viewport
GLFWwindow *init_window();

const char *vertex_shader_source =
    "#version 330 core\n"
    "layout(location=0) in vec3 aPos;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}";

const char *fragment_shader_source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

// Compile the vertex and fragment shader, compile them, and link them together,
// returning an ID for the shader program
unsigned int get_shader_program();

int main() {
    GLFWwindow *window = init_window();

    GLuint shader_program_id = get_shader_program();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,
    };

    // Unique IDs corresponding to our Vertex Attribute Object and Vertex Buffer
    // Object respectively
    GLuint vao_id, vbo_id;
    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);
    /* Bind the VAO first, then bind and set vertex buffer(s), and then
       configure vertex attribute(s) */
    glBindVertexArray(vao_id);
    // Bind the buffer to the GL_ARRAY_BUFFER target
    // From now on, any calls we make to the GL_ARRAY_BUFFER target will use
    // this buffer specifically
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    // Copy 'vertices' into the vertex buffer (GL_STATIC_DRAW = set once, used
    // by the GPU a few times)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(
        // Which vertex attribute we want to configure, indicated by `layout
        // (location = x)` in the shader
        0,
        // The size of the vertex attribute, i.e. vec3
        3,
        // The type of each field
        GL_FLOAT,
        // Whether the data should be normalised
        GL_FALSE,
        // The stride (space between each vertex attribute)
        3 * sizeof(float),
        // The offset of the position data in the buffer
        (void *)0);

    glEnableVertexAttribArray(0);
    // This is allowed because the call to 'glVertexAttribPointer' registered
    // `vbo_id' as the vertex attribute's bound VBO so afterwards we can safely
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program_id);
        glBindVertexArray(vao_id);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Let's be courteous and de-allocate all resources now that they've
    // outlived their purpose
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &vbo_id);
    glDeleteProgram(shader_program_id);

    glfwTerminate();
    return 0;
}

GLFWwindow *init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window =
        glfwCreateWindow(800, 600, "OpenGL Template", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        std::exit(-1);
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

GLuint get_shader_program() {
    GLuint vertex_shader_id;
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_id);

    GLint success;
    GLchar info_log[512];
    // Check if compilation was successful
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, info_log);
        std::cout << "Fatal Error - Compilation of vertex shader failed:\n"
                  << info_log << std::endl;
        std::exit(-1);
    }

    GLuint fragment_shader_id;
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_id);

    // Link the vertex and fragment shader into a shader program
    GLuint shader_program_id;
    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);
    glUseProgram(shader_program_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    // Same thing again
    if (!success) {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, info_log);
        std::cout << "Fatal Error - Compilation of fragment shader failed:\n"
                  << info_log << std::endl;
        std::exit(-1);
    }

    // Delete the individual shaders as they have been linked together and are
    // no longer needed separately
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return shader_program_id;
}
