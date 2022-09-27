#pragma once
#include <cglm/types.h>
typedef struct Camera Camera;
void key_control (Camera *, bool *, float);
void mouse_control (Camera *, float, float);
void update (Camera *);

struct Camera {
    vec3 pos, front, up, right, world_up;
    float yaw, pitch;
    float move_speed, turn_speed;
    mat4 view_matrix;

    void(*key_control) (Camera *, bool *, float);
    void(*mouse_control) (Camera *, float, float);
    void(*update) (Camera *);
};

Camera init_camera(vec3 start_pos, vec3 start_up, float start_yaw, float start_pitch, float start_move_speed, float start_turn_speed);
void mouse_control(Camera *c, float x_diff, float y_diff);
void key_control(Camera *c, bool* keys, float delta_time);
void update(Camera *c);
