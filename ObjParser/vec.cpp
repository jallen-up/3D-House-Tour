/*
 * Vector code; defines 2-, 3- and 4-element vectors.
 *
 * Adapted from OpenGL code by Angel and Schreiner (6th ed.) on
 * 23 August 2014 by Steven R. Vegdahl.
 */

#include <iostream>
#include "vec.h"

//////////////////////////////////////////////////////////////////////////////
//
//  vec2 - 2D vector
//
//////////////////////////////////////////////////////////////////////////////

vec2::vec2( GLfloat s) :
x(s), y(s) {}

vec2::vec2( GLfloat x, GLfloat y ) :
x(x), y(y) {}

vec2::vec2( const vec2& v )
{ x = v.x;  y = v.y;  }

//
//  --- Indexing Operator ---
//

GLfloat& vec2::operator [] ( int i ) { return *(&x + i); }
const GLfloat vec2::operator [] ( int i ) const { return *(&x + i); }

//
//  --- (non-modifying) Arithematic Operators ---
//
vec2 vec2::operator - () const // unary minus operator
{ return vec2( -x, -y ); }

vec2 vec2::operator + ( const vec2& v ) const
{ return vec2( x + v.x, y + v.y ); }

vec2 vec2::operator - ( const vec2& v ) const
{ return vec2( x - v.x, y - v.y ); }

vec2 vec2::operator * ( const GLfloat s ) const
{ return vec2( s*x, s*y ); }

vec2 vec2::operator * ( const vec2& v ) const
{ return vec2( x*v.x, y*v.y ); }

vec2 mult( const GLfloat s, const vec2& v )
{ return v * s; }

vec2 vec2::operator / ( const GLfloat s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return vec2();
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
}

//
//  --- (modifying) Arithmetic Operators ---
//

vec2&  vec2::operator+= ( const vec2& v )
{ x += v.x;  y += v.y;   return *this; }

vec2&  vec2::operator -= ( const vec2& v )
{ x -= v.x;  y -= v.y;  return *this; }

vec2&  vec2::operator *= ( const GLfloat s )
{ x *= s;  y *= s;   return *this; }

vec2& vec2::operator *= ( const vec2& v )
{ x *= v.x;  y *= v.y; return *this; }

vec2&  vec2::operator /= ( const GLfloat s ) {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	*this *= r;
	
	return *this;
}

//
//  --- Conversion Operators ---
//

vec2::operator const GLfloat* () const
{ return static_cast<const GLfloat*>( &x ); }

vec2::operator GLfloat* ()
{ return static_cast<GLfloat*>( &x ); }

void vec2::print(std::ostream& os) const {
	os << "( " << x << "  " << y << " )";
}

void vec2::read(std::istream& is) {
	is >> x;
	is >> y;
}


//////////////////////////////////////////////////////////////////////////////
//
//  vec3 - 3D vector
//
//////////////////////////////////////////////////////////////////////////////

//
//  --- Constructors and Destructors ---
//

vec3::vec3( GLfloat s) :
x(s), y(s), z(s) {}

vec3::vec3( GLfloat x, GLfloat y, GLfloat z ) :
x(x), y(y), z(z) {}

vec3::vec3( const vec3& v ) { x = v.x;  y = v.y;  z = v.z; }

vec3::vec3( const vec2& v, const float f ) { x = v.x;  y = v.y;  z = f; }

//
//  --- Indexing Operator ---
//

GLfloat& vec3::operator [] ( int i ) { return *(&x + i); }
const GLfloat vec3::operator [] ( int i ) const { return *(&x + i); }

//
//  --- (non-modifying) Arithmetic Operators ---
//

vec3 vec3::operator - () const  // unary minus operator
{ return vec3( -x, -y, -z ); }

vec3 vec3::operator + ( const vec3& v ) const
{ return vec3( x + v.x, y + v.y, z + v.z ); }

vec3 vec3::operator - ( const vec3& v ) const
{ return vec3( x - v.x, y - v.y, z - v.z ); }

vec3 vec3::operator * ( const GLfloat s ) const
{ return vec3( s*x, s*y, s*z ); }

vec3 vec3::operator * ( const vec3& v ) const
{ return vec3( x*v.x, y*v.y, z*v.z ); }

vec3 vec3::operator / ( const GLfloat s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return vec3();
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
}

//
//  --- (modifying) Arithematic Operators ---
//

vec3& vec3::operator += ( const vec3& v )
{ x += v.x;  y += v.y;  z += v.z;  return *this; }

vec3& vec3::operator -= ( const vec3& v )
{ x -= v.x;  y -= v.y;  z -= v.z;  return *this; }

vec3& vec3::operator *= ( const GLfloat s )
{ x *= s;  y *= s;  z *= s;  return *this; }

vec3& vec3::operator *= ( const vec3& v )
{ x *= v.x;  y *= v.y;  z *= v.z;  return *this; }

vec3& vec3::operator /= ( const GLfloat s ) {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	*this *= r;
	
	return *this;
}

//
//  --- Conversion Operators ---
//

vec3::operator const GLfloat* () const
{ return static_cast<const GLfloat*>( &x ); }

vec3::operator GLfloat* ()
{ return static_cast<GLfloat*>( &x ); }


void vec3::print(std::ostream& os) const {
	os << "( " << x << "  " << y << "  " << z << " )";
}

void vec3::read(std::istream& is) {
	is >> x;
	is >> y;
	is >> z;
}


//////////////////////////////////////////////////////////////////////////////
//
//  vec4 - 4D vector
//
//////////////////////////////////////////////////////////////////////////////

vec4::vec4( GLfloat s) :
x(s), y(s), z(s), w(s) {}

vec4::vec4( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) :
x(x), y(y), z(z), w(w) {}

vec4::vec4( const vec4& v ) { x = v.x;  y = v.y;  z = v.z;  w = v.w; }

vec4::vec4( const vec3& v, const float s) : w(w)
{ x = v.x;  y = v.y;  z = v.z; }

vec4::vec4( const vec2& v, const float z, const float w ) : z(z), w(w)
{ x = v.x;  y = v.y; }

void vec4::print(std::ostream& os) const {
	os << "( " << x << "  " << y << "  " << z << "  " << w << " )";
}

void vec4::read(std::istream& is) {
	is >> x;
	is >> y;
	is >> z;
	is >> w;
}

//
//  --- Indexing Operator ---
//

GLfloat& vec4::operator [] ( int i ) { return *(&x + i); }
const GLfloat vec4::operator [] ( int i ) const { return *(&x + i); }

//
//  --- (non-modifying) Arithematic Operators ---
//

vec4 vec4::operator - () const  // unary minus operator
{ return vec4( -x, -y, -z, -w ); }

vec4 vec4::operator + ( const vec4& v ) const
{ return vec4( x + v.x, y + v.y, z + v.z, w + v.w ); }

vec4 vec4::operator - ( const vec4& v ) const
{ return vec4( x - v.x, y - v.y, z - v.z, w - v.w ); }

vec4 vec4::operator * ( const GLfloat s ) const
{ return vec4( s*x, s*y, s*z, s*w ); }

vec4 vec4::operator * ( const vec4& v ) const
{ return vec4( x*v.x, y*v.y, z*v.z, w*v.z ); }

vec4 vec4::operator / ( const GLfloat s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return vec4();
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
}

//
//  --- (modifying) Arithematic Operators ---
//

vec4& vec4::operator += ( const vec4& v )
{ x += v.x;  y += v.y;  z += v.z;  w += v.w;  return *this; }

vec4& vec4::operator -= ( const vec4& v )
{ x -= v.x;  y -= v.y;  z -= v.z;  w -= v.w;  return *this; }

vec4& vec4::operator *= ( const GLfloat s )
{ x *= s;  y *= s;  z *= s;  w *= s;  return *this; }

vec4& vec4::operator *= ( const vec4& v )
{ x *= v.x, y *= v.y, z *= v.z, w *= v.w;  return *this; }

vec4& vec4::operator /= ( const GLfloat s ) {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	*this *= r;
	
	return *this;
}

//
//  --- Conversion Operators ---
//

vec4::operator const GLfloat* () const
{ return static_cast<const GLfloat*>( &x ); }

vec4::operator GLfloat* ()
{ return static_cast<GLfloat*>( &x ); }	

