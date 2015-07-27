#include "gl.h"

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>

static void __printShaderCompilationErrors(GLuint id, const char *name)
{
	int   infologLen   = 0;
	int   charsWritten = 0;
	char *infoLog;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infologLen);
	if(infologLen > 1)
	{
		infoLog = (char*)malloc(sizeof(char)*infologLen);
		glGetShaderInfoLog(id, infologLen, &charsWritten, infoLog);
		fprintf(stderr,"%s:\n%s\n",name,infoLog);
		free(infoLog);
	}
}

static char *__loadSource(const char *fn)
{
	FILE *fp;
	size_t size;
	char *source = NULL;
	
	fp = fopen(fn,"r");
	if(!fp)
	{
			fprintf(stderr,"Failed to load shader: %s\n",fn);
			return NULL;
	}
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	source = (char*)malloc(sizeof(char)*(size+1));
	fread(source,1,size,fp);
	source[size] = '\0';
	fclose(fp);
	return source;
}

static void __freeSource(char *source)
{
	free(source);
}

// Global variables
static char *source_vert = NULL;
static char *source_frag = NULL;
static GLuint shader_vert = 0;
static GLuint shader_frag = 0;
static GLuint program = 0;

void initGL()
{
	// Load, create and compile shaders
	source_vert = __loadSource("gl/shader.vert");
	shader_vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader_vert, 1, (const GLchar**)&source_vert, NULL);
	glCompileShader(shader_vert);
	__printShaderCompilationErrors(shader_vert,"shader.vert");
	__freeSource(source_vert);
	source_frag = __loadSource("gl/shader.frag");
	shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader_frag, 1, (const GLchar**)&source_frag, NULL);
	glCompileShader(shader_frag);
	__printShaderCompilationErrors(shader_frag,"shader.frag");
	__freeSource(source_frag);
	
	// Create program, attach shaders and link program
	program = glCreateProgram();
	glAttachShader(program, shader_vert);
	glAttachShader(program, shader_frag);
	glLinkProgram(program);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glPointSize(8.0f);
}

void drawGLBuffer(GLuint buffer, GLuint size, GLuint stride)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	// Draw buffer
	glUseProgram(program);
	{
		// glUniform1i(glGetUniformLocation(program,"uTexture"),0);
		
		glEnableVertexAttribArray(glGetAttribLocation(program,"aVertex"));
		{
			glBindBuffer(GL_ARRAY_BUFFER,buffer);
			glVertexAttribPointer(glGetAttribLocation(program,"aVertex"),3,GL_FLOAT,GL_FALSE,stride,NULL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_POINTS,0,size);
		}
		glDisableVertexAttribArray(glGetAttribLocation(program,"aVertex"));
	}
	glUseProgram(0);
}

void disposeGL()
{
	// Delete program
	glDetachShader(program, shader_vert);
	glDetachShader(program, shader_frag);
	glDeleteProgram(program);
	
	// Delete shaders
	glDeleteShader(shader_vert);
	glDeleteShader(shader_frag);
}
