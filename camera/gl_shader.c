#include "gl_shader.h"

long file_size(FILE *fp)
{
    fseek(fp, 0, SEEK_END); /* Set pointer to end of file */
    long fsize = ftell(fp); /* get file size */
    rewind(fp); /* rewind pointer */
    return fsize;
}

Shader shader_init()
{
    Shader shader;
    shader.load = shader_load;
    shader.destroy = shader_destroy;
    return shader;
}


void shader_load(Shader *shader, const char *vertex_fp, const char *fragment_fp)
{
    /* load vertex file */
    FILE *vertex_stream;
    if (!(vertex_stream = fopen(vertex_fp, "r"))) {
        printf("Can't open vertex file");
        exit(EXIT_FAILURE);
    }

    long fsize = file_size(vertex_stream);
    int vlength = fsize;

    shader->vertex_code = malloc(fsize + 1); /* Set length */
    shader->vertex_code[fsize] = 0; /* Set size+1 to null */

    int i = 0;
    printf("Reading shader...\n");
    while ((shader->vertex_code[i] = fgetc(vertex_stream)) != EOF)
        i++;
    shader->vertex_code[i] = 0;
    fclose(vertex_stream);

    FILE *fragment_stream;
    if (!(fragment_stream = fopen(fragment_fp, "r"))) {
        printf("Can't open fragment file");
        exit(EXIT_FAILURE);
    }

    fsize = file_size(fragment_stream);
    int flength = fsize;

    shader->fragment_code = malloc(fsize + 1); /* Set length */
    shader->fragment_code[fsize] = 0; /* Set size+1 to null */

    i = 0;
    printf("Reading fragment...\n");
    while ((shader->fragment_code[i] = fgetc(fragment_stream)) != EOF)
        i++;
    shader->fragment_code[i] = 0;
    fclose(fragment_stream);

	/* Creating shader objects */
	if(!(shader->vertex = glCreateShader(GL_VERTEX_SHADER))){
		printf("Shaders: Error creating vertex shader object!\n");
	}

	if(!(shader->fragment = glCreateShader(GL_FRAGMENT_SHADER))){
		printf("Shaders: Error creating fragment shader object!\n");
	}

    const char *ptr_vertex = shader->vertex_code;
    const char *ptr_fragment = shader->fragment_code;

    /* Attach shaders */
    glShaderSource(shader->vertex, 1, &ptr_vertex, &vlength);
    glShaderSource(shader->fragment, 1, &ptr_fragment, &flength);

    /* Compile */
    printf("Compiling...\n");
    glCompileShader(shader->vertex);
    glCompileShader(shader->fragment);

    /* Link it */
    if (!(shader->program = glCreateProgram())) {
        printf("Creating program object failed.\n");
    }

    glAttachShader(shader->program, shader->vertex);
    glAttachShader(shader->program, shader->fragment);

    glLinkProgram(shader->program);

    free(shader->vertex_code);
    free(shader->fragment_code);
}

void shader_destroy(Shader *shader)
{
	glDetachShader(shader->vertex, shader->program);
	glDetachShader(shader->fragment, shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
	glDeleteProgram(shader->program);

	shader->load = NULL;
	shader->destroy = NULL;
}

GLuint gl_create_shader(GLenum shader_type, const char *shader_str) {
	log_trace("===\n%s\n===", shader_str);

	bool success = false;
	GLuint shader = glCreateShader(shader_type);
	if (!shader) {
		log_error("Failed to create shader with type %#x.", shader_type);
		goto end;
	}
	glShaderSource(shader, 1, &shader_str, NULL);
	glCompileShader(shader);

	// Get shader status
	{
		GLint status = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (GL_FALSE == status) {
			GLint log_len = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
			if (log_len) {
				char log[log_len + 1];
				glGetShaderInfoLog(shader, log_len, NULL, log);
				log_error("Failed to compile shader with type %d: %s",
				          shader_type, log);
			}
			goto end;
		}
	}

	success = true;

end:
	if (shader && !success) {
		glDeleteShader(shader);
		shader = 0;
	}

	return shader;
}


GLuint gl_create_program(const GLuint *const shaders, int nshaders) {
	bool success = false;
	GLuint program = glCreateProgram();
	if (!program) {
		log_error("Failed to create program.");
		goto end;
	}

	for (int i = 0; i < nshaders; ++i)
		glAttachShader(program, shaders[i]);
	glLinkProgram(program);

	// Get program status
	{
		GLint status = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (GL_FALSE == status) {
			GLint log_len = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			if (log_len) {
				char log[log_len + 1];
				glGetProgramInfoLog(program, log_len, NULL, log);
				log_error("Failed to link program: %s", log);
			}
			goto end;
		}
	}
	success = true;

end:
	if (program) {
		for (int i = 0; i < nshaders; ++i)
			glDetachShader(program, shaders[i]);
	}
	if (program && !success) {
		glDeleteProgram(program);
		program = 0;
	}

	return program;
}


GLuint gl_create_program_from_str(const char *vert_shader_str, const char *frag_shader_str) {
	GLuint vert_shader = 0;
	GLuint frag_shader = 0;
	GLuint prog = 0;

	if (vert_shader_str)
		vert_shader = gl_create_shader(GL_VERTEX_SHADER, vert_shader_str);
	if (frag_shader_str)
		frag_shader = gl_create_shader(GL_FRAGMENT_SHADER, frag_shader_str);

	{
		GLuint shaders[2];
		int count = 0;
		if (vert_shader) {
			shaders[count++] = vert_shader;
		}
		if (frag_shader) {
			shaders[count++] = frag_shader;
		}
		if (count) {
			prog = gl_create_program(shaders, count);
		}
	}

	if (vert_shader)
		glDeleteShader(vert_shader);
	if (frag_shader)
		glDeleteShader(frag_shader);

	return prog;
}
