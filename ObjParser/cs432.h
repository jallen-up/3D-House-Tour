//////////////////////////////////////////////////////////////////////////////
//
//  --- cs432.h ---
//
//   The main header file for all examples, adapted from Angel.h from Angel 6th Edition
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __CS432_H__
#define __CS432_H__

//----------------------------------------------------------------------------
// 
// --- Include system headers ---
//

#include <cmath>
#include <iostream>

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

//----------------------------------------------------------------------------
//
// --- Include OpenGL header files and helpers ---
//
//   The location of these files vary by operating system.  We've included
//     copies of open-soruce project headers in the "GL" directory local
//     this this "include" directory.
//

#ifdef __APPLE__  // include Mac OS X verions of headers and functions
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
inline void glGenVertexArrays(GLsizei n, GLuint *arrays) {
	glGenVertexArraysAPPLE(n, arrays);
}
inline void glBindVertexArray(GLuint array) {
	glBindVertexArrayAPPLE(array);
}
inline int fopen_s(FILE** fpp, const char* name, const char* mode) {
	*fpp = fopen(name, mode);
	return *fpp != NULL;
}
inline void glewInit() { }
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

//----------------------------------------------------------------------------
//
//  --- Include our class libraries and constants ---
//
	
	//  Helper function to load vertex and fragment shader files
	GLuint InitShader( const char* vertexShaderFile,
					  const char* fragmentShaderFile );
	
	//  Helper function to load vertex and fragment shader files
	GLuint InitShader2( const char* vShaderCode,
					  const char* fShaderCode );
	
	//  Degrees-to-radians constant 
	const GLfloat  DegreesToRadians = M_PI / 180.0;

inline int min(int a, int b) {
	return a > b ? b : a;
}

//----------------------------------------------------------------------------
// class that defines a reference to an object on the GPU
//----------------------------------------------------------------------------
class ObjRef {
	int startIdx;
	int count;
public:
	inline ObjRef(int start=0, int end=0):startIdx(start),count(end-start) { } // constructor
	inline int getStartIdx() { return startIdx; } // getter-function for start index
	inline int getCount() { return count; } // getter-function for count
};
	
#endif // __CS432_H__
