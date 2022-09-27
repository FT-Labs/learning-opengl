#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <time.h>

struct Window {
    int w, h;
    bool keys[1024];
    GLfloat x_change, y_change;
    GLfloat x_last, y_last;
    int b_width, b_height;
    bool mouse_flag;
    GLFWwindow *win;
};


struct Window *init_window();
void handle_mouse(GLFWwindow* win, double x, double y);
void handle_keys(GLFWwindow* win, int key, int code, int action, int mode);
