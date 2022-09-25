#include "gl_shader.h"

#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <cglm/affine.h>
#include <time.h>

#include <cglm/cglm.h>
#include <unistd.h>
#include <stdlib.h>

GLFWwindow *win;
GLuint VBO, VAO, prog, uniform_model;

bool dir = true;
float tri_maxoffset = 0.7f;
float tri_offset = 0.05f;
mat4 model;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (GLFW_REPEAT == action) {
        switch (key) {
        case 'W':
            // glm_translate_y(model, tri_offset);
            glm_translate(model, (vec3) {0, 0, tri_offset});
            break;
        case 'A':
            glm_translate_x(model, -tri_offset);
            break;
        case 'D':
            glm_translate_x(model, tri_offset);
            break;
        case 'S':
            glm_translate_z(model, -tri_offset);
            break;
        }
    }
}

static const char *vshader = GLSL(330,
    layout (location = 0) in vec3 pos;
    out vec4 vCol;
    uniform mat4 model;
    void main() {
        gl_Position = model * vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);
        vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    }
);

static const char *fshader = GLSL(330,
    in vec4 vCol;
    out vec4 color;
    void main() {
        color = vCol;
    }
);

static GLfloat *rand_colors(int vert)
{
    srand(time(NULL));
    GLfloat *arr = malloc(sizeof(GLfloat) * vert * 3);
    for (int i = 0; i < vert * 3; i++) {
        arr[i] = (float)rand()/RAND_MAX;
    }
    return arr;
}

void create_triangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 10.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

int main ()
{
    if (!glfwInit()) {
    log_warn("GLFW failed");
    glfwTerminate();
    return 1;
    }

    glm_mat4_identity(model);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win = glfwCreateWindow(2520, 1342, "Translate", NULL, NULL);
    struct timespec t;
    t.tv_nsec = 500000000L;
    t.tv_sec = 0;
    nanosleep(&t, &t);

    if (!win) {
        glfwTerminate();
        return 1;
    }

    int buffer_width, buffer_height;
    glfwGetFramebufferSize(win, &buffer_width, &buffer_height);

    glfwMakeContextCurrent(win);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(win);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, buffer_width, buffer_height);

    glfwSetKeyCallback(win, key_callback);

    prog = gl_create_program_from_str(vshader, fshader);
    create_triangle();

    glClearColor(0.0f, 0.f, 0.3f, 0.0f);
    do {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog);
        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

        glfwSwapBuffers(win);
        glfwPollEvents();
    } while (glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(win));

    return 0;
}
