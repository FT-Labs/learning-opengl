#include "gl_shader.h"
#include "window.h"
#include "camera.h"

#define VEC3_SUM(v1, v2) (vec3) {v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]}
#define VEC3(x, y, z) (vec3) {x, y, z}
#define P_GLUMAT(m) (&m[0][0])

static GLuint mesh_arr[2][4];
GLfloat last_time = 0.0f;
GLfloat delta_time = 0.0f;

enum {
    VAO,
    VBO,
    IBO,
    INDEX_COUNT
};

const char *frag_s = GLSL(330,
    in vec4 vcol;

    out vec4 color;
    void main() {
        color = vcol;
    }
);

const char *vert_s = GLSL(330,
    layout (location = 0) in vec3 pos;

    out vec4 vcol;

    uniform mat4 model;
    uniform mat4 projection;
    uniform mat4 view;

    void main() {
        gl_Position = projection * view * model * vec4(pos, 1.0);
        vcol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    }
);

void create_mesh(GLuint mesh[4], GLfloat *vertices, unsigned int *indices, unsigned int len_vertices, unsigned int len_indices)
{
    mesh[INDEX_COUNT] = len_indices;
    glGenVertexArrays(1, &mesh[VAO]);
    glBindVertexArray(mesh[VAO]);

    glGenBuffers(1, &mesh[IBO]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh[IBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * len_indices, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh[VBO]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh[VBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * len_vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void render_mesh(GLuint mesh[4])
{
    glBindVertexArray(mesh[VAO]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh[IBO]);
    glDrawElements(GL_TRIANGLES, mesh[INDEX_COUNT], GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void clear_mesh(GLuint mesh[4])
{
    if (mesh[IBO]) {
        glDeleteBuffers(1, &mesh[IBO]);
        mesh[IBO] = 0;
    }

    if (mesh[VBO]) {
        glDeleteBuffers(1, &mesh[VBO]);
        mesh[VBO] = 0;
    }

    if (mesh[VAO]) {
        glDeleteVertexArrays(1, &mesh[VAO]);
        mesh[VAO] = 0;
    }

    mesh[INDEX_COUNT] = 0;
}

void create_objects()
{
    unsigned int inds[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat verts[] = {
    	-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
    };

    create_mesh(mesh_arr[0], verts, inds, 12, 12);
    create_mesh(mesh_arr[1], verts, inds, 12, 12);
}


int main ()
{
    struct Window *window = init_window();

    Camera c = init_camera(VEC3(0.0f, 0.0f, 0.0f), VEC3(0.0f, 1.0f, 1.0f), -90.0f, 0.0f, 5.0f, 0.01f);
    GLuint uniform_projection = 0, uniform_model = 0, uniform_view = 0;
    mat4 projection;

    create_objects();
    GLuint prog = gl_create_program_from_str(vert_s, frag_s);

    glm_perspective(
        glm_rad(45.0f),
        16.0f / 9.0f,
        0.1f,
        100.0f,
        projection
    );

    while (!glfwWindowShouldClose(window->win)) {
        GLfloat now = glfwGetTime();
        delta_time = now - last_time; last_time = now;

        glfwPollEvents();

        c.key_control(&c, window->keys, delta_time);
        c.mouse_control(&c, window->x_change, window->y_change);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);
        uniform_model = glGetUniformLocation(prog, "model");
        uniform_projection = glGetUniformLocation(prog, "projection");
        uniform_view = glGetUniformLocation(prog, "view");

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3) {0.0f, 0.0f, -2.5f});
        glm_scale(model, (vec3) {0.4f, 0.4f, 1.0f});
        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, P_GLUMAT(model));
        glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, P_GLUMAT(projection));
        mat4 view;
        glm_lookat(
            c.pos,
            VEC3_SUM(c.pos, c.front),
            c.up,
            view
        );
        glUniformMatrix4fv(uniform_view, 1, GL_FALSE, &view[0][0]);
        render_mesh(mesh_arr[0]);

        glm_mat4_identity(model);
        glm_translate(model, (vec3) {0.0f, 1.0f, -2.5f});
        glm_scale(model, (vec3) {0.4f, 0.4f, 1.0f});
        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, &model[0][0]);
        render_mesh(mesh_arr[1]);

        glUseProgram(0);
        glfwSwapBuffers(window->win);
    }

    return 0;
}
