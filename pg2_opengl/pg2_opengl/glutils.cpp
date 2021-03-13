#include "pch.h"
#include "glutils.h"

void SetMatrix4x4(const GLuint program, const GLfloat* data, const char* matrix_name)
{
	const GLint location = glGetUniformLocation(program, matrix_name);

	if (location == -1)
	{
		printf("Matrix '%s' not found in active shader.\n", matrix_name);
	}
	else
	{
		glUniformMatrix4fv(location, 1, GL_TRUE, data);
	}
}

void SetVector3(const GLuint program,  Vector3 data, const char* vector_name)
{
	const GLint location = glGetUniformLocation(program, vector_name);

	if (location == -1)
	{
		printf("Matrix '%s' not found in active shader.\n", vector_name);
	}
	else
	{
		GLfloat vec[3];
		vec[0] = data.x;
		vec[1] = data.y;
		vec[2] = data.z;

		glUniform3fv(location, 1, vec);
	}
}