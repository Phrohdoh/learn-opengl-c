#include <stdio.h>
#include <stdbool.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLchar *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 pos;"
    "layout (location = 1) in vec2 uv;"
    "uniform mat4 mOrtho;"
    "out vec2 vTexCoord;"
    "void main() {"
    "    vTexCoord = uv;"
    "    gl_Position = mOrtho * vec4(pos, 0.0, 1.0);"
    "}";

const GLchar *fragmentShaderSource =
    "#version 330 core\n"
    "in vec2 vTexCoord;"
    "uniform usampler2D spriteData;"
    "uniform sampler1D palette;"
    "out vec4 color;"
    "void main() {"
    "    uint index = texture(spriteData, vTexCoord).r;"
//  "    color = texelFetch(palette, int(index), 0);"
    "    color = vec4(255.0/float(index), 0.0, 0.0, 1.0);"
    "}";

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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
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
        printf("Failed to create GLFW window\n");
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

    GLenum err;
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

    glUseProgram(shaderProgram);

    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);

    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);

    // World-space coords
    Vertex verts[4];
    create_4_verts(verts, 0.0f, 0.0f, 5.0f, 3.0f);

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

    // Store the uv in attrib 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, uv)));

    // Unbind VAO
    glBindVertexArray(0);

    if ((err = glGetError()) != GL_NO_ERROR) {
        printf("glGetError(bind buffer objs): %x\n", err);
        return 1;
    }

    { // ****
        GLbyte sprite_data[] = {
            0, 1, 1, 0, 2,
            0, 0, 0, 2, 1,
            0, 0, 1, 1, 1,
        };

        GLsizei len_sprite_data = sizeof(sprite_data);
        printf("len_sprite_data: %d\n", len_sprite_data);

        GLuint tex_sprite;
        glGenTextures(1, &tex_sprite);
        glBindTexture(GL_TEXTURE_2D, tex_sprite);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, len_sprite_data / 3, len_sprite_data / 5, 0, GL_RED, GL_UNSIGNED_BYTE, &sprite_data[0]);
        GLint loc_sprite_data = glGetUniformLocation(shaderProgram, "spriteData");
        glUniform1i(loc_sprite_data, 0);

        if ((err = glGetError()) != GL_NO_ERROR) {
            printf("glGetError(tex_sprite): %x\n", err);
            return 1;
        }
    } // ****

    { // ****
        GLbyte palette[] = {
            255, 255, 255,
            255,   0,   0,
            0, 255,   0,
        };

        GLsizei len_palette = sizeof(palette) / 3;
        printf("len_palette: %d\n", len_palette);

        // Setup the `palette` uniform
        GLuint tex_palette;
        glGenTextures(1, &tex_palette);
        glBindTexture(GL_TEXTURE_1D, tex_palette);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, len_palette, 0, GL_RGB, GL_UNSIGNED_BYTE, &palette[0]);
        GLint loc_palette = glGetUniformLocation(shaderProgram, "palette");
        glUniform1i(loc_palette, 1);

        if ((err = glGetError()) != GL_NO_ERROR) {
            printf("glGetError(tex_palette): %x\n", err);
            return 1;
        }
    } // ****

    M ortho = matrix_ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -10.0f, 10.0f);
    GLint loc_mat_ortho = glGetUniformLocation(shaderProgram, "mOrtho");
    glUniformMatrix4fv(loc_mat_ortho, 1, false, (GLvoid const *)&ortho);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO); // Bind VAO
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        if ((err = glGetError()) != GL_NO_ERROR) {
            printf("glGetError(render): %x\n", err);
            return 1;
        }

        glBindVertexArray(0); // Unbind VAO

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}