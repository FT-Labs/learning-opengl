#include "window.h"

static struct Window window;

void handle_keys(GLFWwindow* win, int key, int code, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            window.keys[key] = true;
        else if (action == GLFW_RELEASE)
            window.keys[key] = false;
    }
}

void handle_mouse(GLFWwindow* win, double x, double y)
{
    if (window.mouse_flag) {
        window.x_last = x;
        window.y_last = y;
        window.mouse_flag = false;
    }

    window.x_change = x - window.x_last;
    window.y_change = y - window.y_last;

    window.x_last = x;
    window.y_last = y;
}


struct Window *init_window()
{
    window.w = 1898;
    window.h = 992;
    window.x_change = 0.0f;
    window.y_change = 0.0f;

    if (!glfwInit()) {
        glfwTerminate();
        exit(1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window.win = glfwCreateWindow(window.w, window.h, "Test", NULL, NULL);

    if (!window.win) {
        glfwTerminate();
        exit(1);
    }

    struct timespec t = {.tv_sec = 1};
    nanosleep(&t, &t);

    glfwGetFramebufferSize(window.win, &window.b_width, &window.b_height);
    glfwMakeContextCurrent(window.win);

    glfwSetKeyCallback(window.win, handle_keys);
    glfwSetCursorPosCallback(window.win, handle_mouse);
    glfwSetInputMode(window.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = true;

    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window.win);
        glfwTerminate();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, window.b_width, window.b_height);
    glfwSetWindowUserPointer(window.win, window.win);
    return &window;
}
