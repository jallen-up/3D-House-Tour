/*
 * Matrix-stack code; allows pushing and popping of 4x4 matrices.
 *
 * Adapted from OpenGL code by Angel and Schreiner (6th ed.) on
 * 23 August 2014 by Steven R. Vegdahl.
 */
 
#ifndef __MATSTACK_H__
#define __MATSTACK_H__

#include "mat.h"

// class deinition
class MatrixStack {
    int    _index; // # matrices pushed
    int    _size; // capacity
    mat4*  _matrices; // array of matrices
	
public:
	// constructor
    MatrixStack( int numMatrices = 32 );	
	
	// destructor
    ~MatrixStack();
	
	// push and pop methods
    void push( const mat4& m );	
    mat4& pop( void );
	
	//  print method
	void print(std::ostream& os) const;
};

// operator version of print method
std::ostream& operator << ( std::ostream& os, const MatrixStack& m );

#endif