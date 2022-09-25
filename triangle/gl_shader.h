#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "log.h"


#define GLSL(version, ...) "#version " #version "\n" #__VA_ARGS__
#define QUOTE(...) #__VA_ARGS__

typedef struct Shader Shader;

struct Shader {
    GLuint vertex;
    GLuint fragment;
    GLuint program;
    char *vertex_code;
    char *fragment_code;
    void(*load)(Shader *, const char *, const char *);
    void(*destroy)(Shader *);
};

void shader_load(Shader *shader, const char *vertex_fp, const char *fragment_fp);
void shader_destroy(Shader *shader);
long file_size(FILE *fp);
Shader shader_init();
void shader_load(Shader *shader, const char *vertex_fp, const char *fragment_fp);
void shader_destroy(Shader *shader);
GLuint gl_create_shader(GLenum shader_type, const char *shader_str);
GLuint gl_create_program(const GLuint *const shaders, int nshaders);
GLuint gl_create_program_from_str(const char *vert_shader_str, const char *frag_shader_str);
