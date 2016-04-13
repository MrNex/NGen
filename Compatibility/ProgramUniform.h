#ifndef PROGRAMUNIFORM_H
#define PROGRAMUNIFORM_H

#include <GL/glew.h>

///
// glUnfirormProgram & glUniform wrappers for backwards compatibility
//

///
//Parameters
//	program: Specifies the handle of the program containing the
//		uniform variable to be modified.
//	location: Specifies the location of the uniform value to be modified
//	count: Specifies the number of matrices that are to be modified.
//		This should be 1 if the targeted uniform variable is not an
//		array of matrices, and 1 or more if it is an array of
//		matrices.
//	transpose: Specifies whether to transpose the matrix as the values
//		 are loaded into the uniform variable.
//	value: Specifies a pointer to an array of count values or a sequence of values 
//		that will be used to update the specified uniform variable.
void ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* components);

void ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* components);

void ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* components);

void ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* components);

void ProgramUniform1i(GLuint program, GLint location, GLsizei count);

void ProgramUniform1f(GLuint program, GLint location, const float value); 
#endif
