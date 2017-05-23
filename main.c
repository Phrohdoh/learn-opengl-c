#include <stdio.h>
#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLchar *vertexShaderSource =
    "#version 330\n"
    "layout (location = 0) in vec2 pos2;"
    "uniform mat4 mat_vm;"
    "void main() {"
    "  gl_Position = mat_vm * vec4(pos2, 1.0, 1.0);"
    "}";

const GLchar *fragmentShaderSource = "#version 330 core\n"
    "layout (location = 0) out vec4 color; void main() { color = vec4(1.0f, 0.0f, 0.0f, 1.0f); }";

const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 768;

typedef struct Vertex {
    float pos[2];
    float uv[2];
} Vertex;

static Vertex _create_vert(float x, float y, float u, float v) {
    Vertex vv = { {x, y}, {u, v}, };
    return vv;
}

static void create_4_verts(Vertex *verts, float x, float y, float w, float h) {
    verts[0] = _create_vert(x + 0.0f, y + 0.0f, 0.0f, 0.0f);
    verts[1] = _create_vert(x + 0.0f, y +    h, 0.0f, 1.0f);
    verts[2] = _create_vert(x +    w, y +    h, 1.0f, 1.0f);
    verts[3] = _create_vert(x +    w, y + 0.0f, 1.0f, 0.0f);
}

typedef struct M {
    float m11; float m12; float m13; float m14;
    float m21; float m22; float m23; float m24;
    float m31; float m32; float m33; float m34;
    float m41; float m42; float m43; float m44;
} M;

static M matrix_identity() {
    M m = {
        .m11 = 1.0, .m12 = 0.0, .m13 = 0.0, .m14 = 0.0,
        .m21 = 0.0, .m22 = 1.0, .m23 = 0.0, .m24 = 0.0,
        .m31 = 0.0, .m32 = 0.0, .m33 = 1.0, .m34 = 0.0,
        .m41 = 0.0, .m42 = 0.0, .m43 = 0.0, .m44 = 1.0,
    };

    return m;
}

static M matrix_ortho(float left, float right, float bottom, float top, float near, float far) {
    M m = matrix_identity();
    m.m11 = 2.0f / (right - left);
    m.m22 = 2.0 / (top - bottom);
    m.m33 = -2.0 / (far - near);
    m.m41 = -((right + left) / (right - left));
    m.m42 = -((top + bottom) / (top - bottom));
    m.m43 = -((far + near) / (far - near));
    return m;
}

void key_cb(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {
    printf("glfw init\n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_cb);

    if (glewInit() != GLEW_OK) {
        printf("Failed to init GLEW\n");
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    {
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

        if (infoLog[0] != 0) {
            if (!success) {
                printf("Vertex shader compilation failed!\n");
            }

            printf("Vertex shader info: %s\n", infoLog);
        }
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    {
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

        if (infoLog[0] != 0) {
            if (!success) {
                printf("Fragment shader compilation failed!\n");
            }

            printf("Fragment shader info: %s\n", infoLog);
        }
    }

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    {
        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

        if (infoLog[0] != 0) {
            if (!success) {
                printf("Program linking failed!\n");
            }

            printf("Program linking info: %s\n", infoLog);
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Vertex verts[4];
    create_4_verts(verts, 1.0f, 1.0f, 50.0f, 50.0f);
    printf("[%.f, %.f]\n", verts[0].pos[0], verts[0].pos[1]);
    printf("[%.f, %.f]\n", verts[1].pos[0], verts[1].pos[1]);
    printf("[%.f, %.f]\n", verts[2].pos[0], verts[2].pos[1]);
    printf("[%.f, %.f]\n", verts[3].pos[0], verts[3].pos[1]);

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VBO, VAO, IBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Store the pos in attrib 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, pos)));

    // Unbind VAO
    glBindVertexArray(0);

    M ortho = matrix_ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, 0.0f, 1000.0f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        GLint loc_mat_vm = glGetUniformLocation(shaderProgram, "mat_vm");
        glUniformMatrix4fv(loc_mat_vm, 1, false, (GLfloat const *)&ortho);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}