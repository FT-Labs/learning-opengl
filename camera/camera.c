#include "window.h"
#include "camera.h"

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
