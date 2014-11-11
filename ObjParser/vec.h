/*
 * Vector code; defines 2-, 3- and 4-element vectors.
 *
 * Adapted from OpenGL code by Angel and Schreiner (6th ed.) on
 * 23 August 2014 by Steven R. Vegdahl.
 */

#ifndef __VEC_H__
#define __VEC_H__

#include <iostream>
#include <cmath>

typedef float GLfloat;

//////////////////////////////////////////////////////////////////////////////
//
//  vec2 - 2D vector
//

struct vec2 {
	
	// instance variables: the x and y components
    GLfloat  x;
    GLfloat  y;
	
    //
    //  --- Constructors and Destructors ---
    //
	
    vec2( GLfloat s = GLfloat(0.0) );
    vec2( GLfloat x, GLfloat y ) ;
	
    vec2( const vec2& v );
	
    //
    //  --- Indexing Operator ---
    //
	
    GLfloat& operator [] ( int i ) ;
    const GLfloat operator [] ( int i ) const ;
	
    //
    //  --- (non-modifying) Arithmetic Operators ---
    //
	
    vec2 operator - () const ;
	
    vec2 operator + ( const vec2& v ) const;
	
    vec2 operator - ( const vec2& v ) const;
	
    vec2 operator * ( const GLfloat s ) const;
    vec2 operator * ( const vec2& v ) const;
	// friend vec2 operator * ( const GLfloat s, const vec2& v );
    vec2 operator / ( const GLfloat s ) const ;
    //
    //  --- (modifying) Arithematic Operators ---
    //
	
    vec2& operator += ( const vec2& v );
	
    vec2& operator -= ( const vec2& v );
	
    vec2& operator *= ( const GLfloat s );
	
    vec2& operator *= ( const vec2& v );
	
    vec2& operator /= ( const GLfloat s ) ;
	
    //
    //  --- Conversion Operators ---
    //
	
    operator const GLfloat* () const;
	
	operator GLfloat* ();
	
	void print(std::ostream& os) const;
	
	void read(std::istream& is);

	
};


//----------------------------------------------------------------------------
//
//  Non-class vec2 Methods
//

inline std::ostream& operator << (std::ostream& os, const vec2 p) {
	p.print(os);
	return os;
}

inline std::istream& operator >> (std::istream& is, vec2& p) {
	p.read(is);
	return is;
}

inline vec2 operator * ( const GLfloat s, const vec2& v ) { return v*s; }

inline
GLfloat dot( const vec2& u, const vec2& v ) {
    return u.x * v.x + u.y * v.y;
}

inline
GLfloat length( const vec2& v ) {
    return sqrt( dot(v,v) );
}

inline
vec2 normalize( const vec2& v ) {
    return v / length(v);
}

//////////////////////////////////////////////////////////////////////////////
//
//  vec3 - 3D vector
//
//////////////////////////////////////////////////////////////////////////////

struct vec3 {
	
	// instance variables: the x, y and z components
    GLfloat  x;
    GLfloat  y;
    GLfloat  z;
	
    //
    //  --- Constructors and Destructors ---
    //
	
    vec3( GLfloat s = GLfloat(0.0) );
    vec3( GLfloat x, GLfloat y, GLfloat z );
    vec3( const vec3& v );
	
    vec3( const vec2& v, const float f );
	
	void print(std::ostream& os) const;
	
	void read(std::istream& is);
    
	//
    //  --- Indexing Operator ---
    //
	
    GLfloat& operator [] ( int i ) ;
    const GLfloat operator [] ( int i ) const;
	
    //
    //  --- (non-modifying) Arithematic Operators ---
    //
	
    vec3 operator - () const ; // unary minus operator
	
    vec3 operator + ( const vec3& v ) const;
	
    vec3 operator - ( const vec3& v ) const;
	
    vec3 operator * ( const GLfloat s ) const;
	
    vec3 operator * ( const vec3& v ) const;
	
	//friend vec3 operator * ( const GLfloat s, const vec3& v );
	
    vec3 operator / ( const GLfloat s ) const ;
    //
    //  --- (modifying) Arithmetic Operators ---
    //
	
    vec3& operator += ( const vec3& v );
	
    vec3& operator -= ( const vec3& v );
	
    vec3& operator *= ( const GLfloat s );
	
    vec3& operator *= ( const vec3& v );
	
    vec3& operator /= ( const GLfloat s ) ;	
	
    operator const GLfloat* () const;
	
    operator GLfloat* ();
};

//----------------------------------------------------------------------------
//
//  Non-class vec3 Methods
//

inline vec3 operator * ( const GLfloat s, const vec3& v ) { return v*s; }


inline std::ostream& operator << (std::ostream& os, const vec3 p) {
	p.print(os);
	return os;
}

inline std::istream& operator >> (std::istream& is, vec3& p) {
	p.read(is);
	return is;
}

inline
GLfloat dot( const vec3& u, const vec3& v ) {
    return u.x*v.x + u.y*v.y + u.z*v.z ;
}

inline
GLfloat length( const vec3& v ) {
    return std::sqrt( dot(v,v) );
}

inline
vec3 normalize( const vec3& v ) {
    return v / length(v);
}

inline
vec3 cross(const vec3& a, const vec3& b )
{
    return vec3( a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x );
}



//////////////////////////////////////////////////////////////////////////////
//
//  vec4 - 4D vector
//
//////////////////////////////////////////////////////////////////////////////
struct vec4 {
			
	// instance variables: the x, y, z and w components
		GLfloat  x;
		GLfloat  y;
		GLfloat  z;
		GLfloat  w;
		
		//
		//  --- Constructors and Destructors ---
		//	
		vec4( GLfloat s = GLfloat(0.0) ) ;
		
		vec4( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) ;
		
		vec4( const vec4& v ) ;
	
    vec4( const vec3& v, const float s = 1.0 ) ;
    vec4( const vec2& v, const float z, const float w );
	
	void print(std::ostream& os) const;
	
	void read(std::istream& is);
	
    //
    //  --- Indexing Operator ---
    //
	
    GLfloat& operator [] ( int i ) ;
    const GLfloat operator [] ( int i ) const ;
	
    //
    //  --- (non-modifying) Arithematic Operators ---
    //
	
    vec4 operator - () const ; // unary minus operator
	
    vec4 operator + ( const vec4& v ) const;
	
    vec4 operator - ( const vec4& v ) const;
	
    vec4 operator * ( const GLfloat s ) const;
	
    vec4 operator * ( const vec4& v ) const;
		
    vec4 operator / ( const GLfloat s ) const ;
	
    //
    //  --- (modifying) Arithematic Operators ---
    //
	
    vec4& operator += ( const vec4& v );
	
    vec4& operator -= ( const vec4& v );
	
    vec4& operator *= ( const GLfloat s );
	
    vec4& operator *= ( const vec4& v );
	
    vec4& operator /= ( const GLfloat s ) ;
	
    //
    //  --- Conversion Operators ---
    //
	
    operator const GLfloat* () const;
	
    operator GLfloat* ();
	
};

//----------------------------------------------------------------------------
//
//  Non-class vec4 Methods
//

inline
GLfloat dot( const vec4& u, const vec4& v ) {
    return u.x*v.x + u.y*v.y + u.z*v.z + u.w+v.w;
}

inline
GLfloat length( const vec4& v ) {
    return std::sqrt( dot(v,v) );
}

inline
vec4 normalize( const vec4& v ) {
    return v / length(v);
}

inline
vec3 cross(const vec4& a, const vec4& b )
{
    return vec3( a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x );
}

inline std::ostream& operator << (std::ostream& os, const vec4& p) {
	p.print(os);
	return os;
}

inline std::istream& operator >> (std::istream& is, vec4& p) {
	p.read(is);
	return is;
}

inline vec4 operator * ( const GLfloat s, const vec4& v ) { return v*s; }

#endif // __VEC_H__

