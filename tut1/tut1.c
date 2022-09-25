#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

enum {
    false = 0,
    true = 1
};

int main ()
{

    if (!glfwInit()) {
        fprintf(stderr, "Error");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    GLFWwindow *window = glfwCreateWindow(1024, 768, "Tut1", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glewExperimental = true;
    glfwMakeContextCurrent(window);

    GLuint vertex_arr_id;
    glGenVertexArrays(1, &vertex_arr_id);
    glBindVertexArray(vertex_arr_id);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    /* vertex buffer */
    GLuint vertex_buffer;
    /* Gen 1 buffer, put result in vertex buffer */
    glGenBuffers(1, &vertex_buffer);
    /* bind it */
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    /* Give vertices */
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    if (glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    do {
        /* Clear the screen */
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(false);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(
            0, /* attribtue 0 */
            3, /* size */
            GL_FLOAT, /* type */
            GL_FALSE, /* normalize? */
            0, /* stride */
            (void *)0/* buffer offset */
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vertex_arr_id);
    glfwTerminate();


    return 0;
}
