#include "gl_shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow *win;

static const char *frag_str = GLSL(330,
    out vec3 color;
    void main() {
        color = vec3(1.0f, 0.0f, 0.0f);
    }
);

static const char *vert_str = GLSL(330,
    layout(location = 0) in vec3 vertexPosition_modelspace;
    void main(){
        gl_Position.xyz = vertexPosition_modelspace;
        gl_Position.w = 1.0;
    }
);


int main ()
{
    if (!glfwInit())
        exit(1);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win = glfwCreateWindow(1024, 768, "Triangle", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(win);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        exit(1);
    }

    glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.0f, 0.4f, 0.0f, 0.0f);

    GLuint vertex_arr_id;
    glGenVertexArrays(1, &vertex_arr_id);
    glBindVertexArray(vertex_arr_id);

    static const GLfloat g_vertex_buffer[] = {
        1.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f,  0.0f, 0.0f
    };

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer), g_vertex_buffer, GL_STATIC_DRAW);

    GLuint prog = gl_create_program_from_str(vert_str, frag_str);

    do {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            NULL
        );

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(win);
        glfwPollEvents();

    } while (glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(win));

    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &vertex_arr_id);
    glDeleteBuffers(1, &vertex_buffer);
    glfwTerminate();

    return 0;
}
