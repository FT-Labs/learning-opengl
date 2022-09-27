#include "gl_shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

static GLuint mesh_arr[2][4];
GLfloat last_time = 0.0f;
GLfloat delta_time = 0.0f;

typedef struct Camera Camera;
void key_control (Camera *, bool *, GLfloat);
void mouse_control (Camera *, GLfloat, GLfloat);
void update (Camera *);

static struct {
    int w, h;
    bool keys[1024];
    GLfloat x_change, y_change;
    GLfloat x_last, y_last;
    int b_width, b_height;
    bool mouse_flag;
    GLFWwindow *win;
} Window;


void handle_keys(GLFWwindow* window, int key, int code, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            Window.keys[key] = true;
        else if (action == GLFW_RELEASE)
            Window.keys[key] = false;
    }
}

void handle_mouse(GLFWwindow* window, double x, double y)
{
    if (Window.mouse_flag) {
        Window.x_last = x;
        Window.y_last = y;
        Window.mouse_flag = false;
    }

    Window.x_change = x - Window.x_last;
    Window.y_change = y - Window.y_last;

    Window.x_last = x;
    Window.y_last = y;
}

void init_window()
{
    Window.w = 1898;
    Window.h = 992;
    Window.x_change = 0.0f;
    Window.y_change = 0.0f;

    if (!glfwInit()) {
        log_warn("GLFW failed");
        glfwTerminate();
        exit(1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Window.win = glfwCreateWindow(Window.w, Window.h, "Test", NULL, NULL);

    if (!Window.win) {
        glfwTerminate();
        exit(1);
    }

    struct timespec t = {.tv_sec = 1};
    nanosleep(&t, &t);

    glfwGetFramebufferSize(Window.win, &Window.b_width, &Window.b_height);
    glfwMakeContextCurrent(Window.win);

    glfwSetKeyCallback(Window.win, handle_keys);
    glfwSetCursorPosCallback(Window.win, handle_mouse);
    glfwSetInputMode(Window.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(Window.win);
        glfwTerminate();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, Window.b_width, Window.b_height);
    glfwSetWindowUserPointer(Window.win, Window.win);
}

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

struct Camera {
    vec3 pos, front, up, right, world_up;
    GLfloat yaw, pitch;
    GLfloat move_speed, turn_speed;
    mat4 view_matrix;

    void(*key_control) (Camera *, bool *, GLfloat);
    void(*mouse_control) (Camera *, GLfloat, GLfloat);
    void(*update) (Camera *);
};

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

Camera init_camera(vec3 start_pos, vec3 start_up, GLfloat start_yaw, GLfloat start_pitch, GLfloat start_move_speed, GLfloat start_turn_speed)
{
    Camera camera = {.pos = {start_pos[0], start_pos[1], start_pos[2]},
        .world_up = {start_up[0], start_up[1], start_up[2]}, .yaw = start_yaw,
        .pitch = start_pitch, .move_speed = start_move_speed, .turn_speed = start_turn_speed, .front = {0.0f, 0.0f, -1.0f}};
    camera.key_control = key_control;
    camera.mouse_control = mouse_control;
    camera.update = update;
    camera.update(&camera);
    return camera;
}

void mouse_control(Camera *c, GLfloat x_diff, GLfloat y_diff)
{
    x_diff *= c->turn_speed;
    y_diff *= c->turn_speed;
    c->yaw += x_diff;
    c->pitch += y_diff;

    if (c->pitch > 89.0f)
        c->pitch = 89.0f;
    if (c->pitch < -89.0f)
        c->pitch = -89.0f;

    c->update(c);
}

void key_control(Camera *c, bool* keys, GLfloat delta_time)
{
	GLfloat velocity = c->move_speed * delta_time;

	if (keys[GLFW_KEY_W]) {
        vec3 tmp;
        glm_vec3_scale(c->front, velocity, tmp);
        glm_vec3_add(c->pos, tmp, c->pos);
	}
	if (keys[GLFW_KEY_S]) {
        vec3 tmp;
        glm_vec3_scale(c->front, velocity, tmp);
        glm_vec3_sub(c->pos, tmp, c->pos);
	}
    if (keys[GLFW_KEY_A]) {
        vec3 tmp;
        glm_vec3_scale(c->right, velocity, tmp);
        glm_vec3_sub(c->pos, tmp, c->pos);
	}

	if (keys[GLFW_KEY_D]) {
        vec3 tmp;
        glm_vec3_scale(c->right, velocity, tmp);
        glm_vec3_add(c->pos, tmp, c->pos);
	}
}

void update(Camera *c)
{
    c->front[0] = cos(glm_rad(c->yaw)) * cos(glm_rad(c->pitch));
    c->front[1] = sin(glm_rad(c->pitch));
    c->front[2] = sin(glm_rad(c->yaw)) * cos(glm_rad(c->pitch));
    glm_vec3_normalize(c->front);
    glm_cross(c->front, c->world_up, c->right);
    glm_vec3_normalize(c->right);
    glm_cross(c->right, c->front, c->up);
    glm_normalize(c->up);
}

int main ()
{
    init_window();

    Camera c = init_camera((vec3) {0.0f, 0.0f, 0.0f}, (vec3) {0.0f, 1.0f, 1.0f}, -90.0f, 0.0f, 5.0f, 0.01f);
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

    while (!glfwWindowShouldClose(Window.win)) {
        GLfloat now = glfwGetTime();
        delta_time = now - last_time; last_time = now;

        glfwPollEvents();

        c.key_control(&c, Window.keys, delta_time);
        c.mouse_control(&c, Window.x_change, Window.y_change);

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
        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, &projection[0][0]);
        mat4 view;
        glm_lookat(
            c.pos,
            (vec3) {c.pos[0] + c.front[0], c.pos[1] + c.front[1], c.pos[2] + c.front[2]},
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
        glfwSwapBuffers(Window.win);
    }

    return 0;
}
