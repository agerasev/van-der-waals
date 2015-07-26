#pragma once

#include <GL/glew.h>

#ifdef __cplusplus
extern "C" {
#endif

void initGL();
void disposeGL();
void drawGLBuffer(GLuint buffer, GLuint size, GLuint stride);

#ifdef __cplusplus
}
#endif
