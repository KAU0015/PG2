#include "vector3.h"
#ifndef GL_UTILS_H_
#define GL_UTILS_H_

void SetMatrix4x4(const GLuint program, const GLfloat* data, const char* matrix_name);

void SetVector3(const GLuint program,  Vector3 data, const char* vector_name);

void SetHandle(const GLuint program, GLuint64 texture_handle, const char* sampler_name);

void SetInt(const GLuint program, const GLint data, const char* name);

#endif
