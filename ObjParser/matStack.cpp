/*
 * Matrix-stack code; allows pushing and popping of 4x4 matrices.
 *
 * Adapted from OpenGL code by Angel and Schreiner (6th ed.) on
 * 23 August 2014 by Steven R. Vegdahl.
 */

#include "mat.h"
#include "matStack.h"
#include <assert.h>

// constructor:
//  parameter gives capacity of stack
MatrixStack::MatrixStack( int numMatrices):_index(0), _size(numMatrices)
{ _matrices = new mat4[numMatrices]; }

// destructor
MatrixStack::~MatrixStack()
{ delete[]_matrices; }

// push a matrix onto the stack
void MatrixStack::push( const mat4& m ) {
	assert( _index + 1 < _size );
	_matrices[_index++] = m;
}

// pop a matrix from the stack
mat4& MatrixStack::pop( void ) {
	assert( _index - 1 >= 0 );
	_index--;
	return _matrices[_index];
}

// print the matrix to an output stream
void MatrixStack::print(std::ostream& os) const {
	for (int i = 0; i < _index; i++) {
		os << _matrices[i];
	}
}

// print the matrix to an output stream (operator version)
std::ostream& operator << ( std::ostream& os, const MatrixStack& m ) {
	m.print(os);
	return os;
}
