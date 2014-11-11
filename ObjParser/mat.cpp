/*
 *  mat.cpp -- 2x2, 3x3, and 4x4 matrix operations. Adapted from Angel and Schreiner, 6th ed.
 *
 */

#include "vec.h"
#include "mat.h"

//----------------------------------------------------------------------------
//
//  mat2 - 2D square matrix
//

mat2::mat2( const GLfloat d)  // Create a diagional matrix
{ _m[0].x = d;  _m[1].y = d;   }

mat2::mat2( const vec2& a, const vec2& b )
{ _m[0] = a;  _m[1] = b;  }

mat2::mat2( GLfloat m00, GLfloat m10, GLfloat m01, GLfloat m11 )
{ _m[0] = vec2( m00, m10 ); _m[1] = vec2( m01, m11 ); }
// old version
// { _m[0] = vec2( m00, m01 ); _m[1] = vec2( m10, m11 ); }

mat2::mat2( const mat2& m ) {
	if ( *this != m ) {
		_m[0] = m._m[0];
		_m[1] = m._m[1];
	} 
}

//
//  --- Indexing Operator ---
//

vec2& mat2::operator [] 
( int i ) { return _m[i]; }

const vec2& mat2::operator [] ( int i ) const { return _m[i]; }

//
//  --- (non-modifying) Arithmatic Operators ---
//

mat2 mat2::operator + ( const mat2& m ) const
{ return mat2( _m[0]+m[0], _m[1]+m[1] ); }

mat2 mat2::operator - ( const mat2& m ) const
{ return mat2( _m[0]-m[0], _m[1]-m[1] ); }

mat2 mat2::operator * ( const GLfloat s ) const 
{ return mat2( s*_m[0], s*_m[1] ); }

mat2 mat2::operator / ( const GLfloat s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return mat2();
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
}

//mat2 operator mat2::* ( const GLfloat s, const mat2& m )

mat2 mat2::operator * ( const mat2& m ) const {
	mat2  a( 0.0 );
	
	for ( int i = 0; i < 2; ++i ) {
		for ( int j = 0; j < 2; ++j ) {
			for ( int k = 0; k < 2; ++k ) {
				a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	return a;
}

//
//  --- (modifying) Arithmetic Operators ---
//

mat2& mat2::operator += ( const mat2& m ) {
	_m[0] += m[0];  _m[1] += m[1];  
	return *this;
}

mat2& mat2::operator -= ( const mat2& m ) {
	_m[0] -= m[0];  _m[1] -= m[1];  
	return *this;
}

mat2& mat2::operator *= ( const GLfloat s ) {
	_m[0] *= s;  _m[1] *= s;   
	return *this;
}

mat2& mat2::operator *= ( const mat2& m ) {
	mat2  a( 0.0 );
	
	for ( int i = 0; i < 2; ++i ) {
		for ( int j = 0; j < 2; ++j ) {
			for ( int k = 0; k < 2; ++k ) {
				a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	return 	*this = a;
}

mat2& mat2::operator /= ( const GLfloat s ) {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return *this;
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this *= r;
}

//
//  --- Matrix / Vector operators ---
//

vec2 mat2::operator * ( const vec2& v ) const {  // m * v
	return vec2( _m[0][0]*v.x + _m[0][1]*v.y,
				_m[1][0]*v.x + _m[1][1]*v.y );
}

//
//  --- Insertion and Extraction Operators ---
//
void mat2::print ( std::ostream& os) const
{ os << std::endl << _m[0] << std::endl << _m[1] << std::endl; }

void mat2::read( std::istream& is)
{ is >> _m[0] >> _m[1] ; }

//
//  --- Conversion Operators ---
//

mat2::operator const GLfloat* () const
{ return static_cast<const GLfloat*>( &_m[0].x ); }

mat2::operator GLfloat* ()
{ return static_cast<GLfloat*>( &_m[0].x ); }

//----------------------------------------------------------------------------
//
//  mat3 - 3D square matrix 
//

//
//  --- Constructors and Destructors ---
//

mat3::mat3( const GLfloat d)  // Create a diagional matrix
{ _m[0].x = d;  _m[1].y = d;  _m[2].z = d;   }

mat3::mat3( const vec3& a, const vec3& b, const vec3& c )
{ _m[0] = a;  _m[1] = b;  _m[2] = c;  }

mat3::mat3( GLfloat m00, GLfloat m10, GLfloat m20,
		   GLfloat m01, GLfloat m11, GLfloat m21,
		   GLfloat m02, GLfloat m12, GLfloat m22 ) 
{
	_m[0] = vec3( m00, m10, m20 );
	_m[1] = vec3( m01, m11, m21 );
	_m[2] = vec3( m02, m12, m22 );
	// _m[0] = vec3( m00, m01, m02 );
	// _m[1] = vec3( m10, m11, m12 );
	// _m[2] = vec3( m20, m21, m22 );
}

mat3::mat3( const mat3& m )
{
	if ( *this != m ) {
		_m[0] = m._m[0];
		_m[1] = m._m[1];
		_m[2] = m._m[2];
	} 
}

//
//  --- Indexing Operator ---
//

vec3& mat3::operator [] ( int i ) { return _m[i]; }
const vec3& mat3::operator [] ( int i ) const { return _m[i]; }

//
//  --- (non-modifying) Arithmatic Operators ---
//

mat3 mat3::operator + ( const mat3& m ) const
{ return mat3( _m[0]+m[0], _m[1]+m[1], _m[2]+m[2] ); }

mat3 mat3::operator - ( const mat3& m ) const
{ return mat3( _m[0]-m[0], _m[1]-m[1], _m[2]-m[2] ); }

mat3 mat3::operator * ( const GLfloat s ) const 
{ return mat3( s*_m[0], s*_m[1], s*_m[2] ); }

mat3 mat3::operator / ( const GLfloat s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return mat3();
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
}

mat3 mat3::operator * ( const mat3& m ) const {
	mat3  a( 0.0 );
	
	for ( int i = 0; i < 3; ++i ) {
		for ( int j = 0; j < 3; ++j ) {
			for ( int k = 0; k < 3; ++k ) {
				a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	return a;
}

//
//  --- (modifying) Arithmetic Operators ---
//

mat3& mat3::operator += ( const mat3& m ) {
	_m[0] += m[0];  _m[1] += m[1];  _m[2] += m[2]; 
	return *this;
}

mat3& mat3::operator -= ( const mat3& m ) {
	_m[0] -= m[0];  _m[1] -= m[1];  _m[2] -= m[2]; 
	return *this;
}

mat3& mat3::operator *= ( const GLfloat s ) {
	_m[0] *= s;  _m[1] *= s;  _m[2] *= s; 
	return *this;
}

mat3& mat3::operator *= ( const mat3& m ) {
	mat3  a( 0.0 );
	
	for ( int i = 0; i < 3; ++i ) {
		for ( int j = 0; j < 3; ++j ) {
			for ( int k = 0; k < 3; ++k ) {
				a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	return *this = a;
}

mat3& mat3::operator /= ( const GLfloat s ) {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return *this;
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this *= r;
}


//
//  --- Insertion and Extraction Operators ---
//
void mat3::print ( std::ostream& os) const
{ os << std::endl << _m[0] << std::endl<< _m[1] << std::endl << _m[2] << std::endl; }

void mat3::read( std::istream& is)
{ is >> _m[0] >> _m[1] >> _m[2] ; }



//
//  --- Matrix / Vector operators ---
//

vec3 mat3::operator * ( const vec3& v ) const {  // m * v
	return vec3( _m[0][0]*v.x + _m[0][1]*v.y + _m[0][2]*v.z,
				_m[1][0]*v.x + _m[1][1]*v.y + _m[1][2]*v.z,
				_m[2][0]*v.x + _m[2][1]*v.y + _m[2][2]*v.z );
}

//
//  --- Conversion Operators ---
//

mat3::operator const GLfloat* () const
{ return static_cast<const GLfloat*>( &_m[0].x ); }

mat3::operator GLfloat* ()
{ return static_cast<GLfloat*>( &_m[0].x ); }

//----------------------------------------------------------------------------
//
//  mat4.h - 4D square matrix
//

//
//  --- Constructors and Destructors ---
//

mat4::mat4( const GLfloat d)  // Create a diagional matrix
{ _m[0].x = d;  _m[1].y = d;  _m[2].z = d;  _m[3].w = d; }

mat4::mat4( const vec4& a, const vec4& b, const vec4& c, const vec4& d )
{ _m[0] = a;  _m[1] = b;  _m[2] = c;  _m[3] = d; }

mat4::mat4( GLfloat m00, GLfloat m10, GLfloat m20, GLfloat m30,
		   GLfloat m01, GLfloat m11, GLfloat m21, GLfloat m31,
		   GLfloat m02, GLfloat m12, GLfloat m22, GLfloat m32,
		   GLfloat m03, GLfloat m13, GLfloat m23, GLfloat m33 )
{
	_m[0] = vec4( m00, m10, m20, m30 );
	_m[1] = vec4( m01, m11, m21, m31 );
	_m[2] = vec4( m02, m12, m22, m32 );
	_m[3] = vec4( m03, m13, m23, m33 );
	// _m[0] = vec4( m00, m01, m02, m03 );
	// _m[1] = vec4( m10, m11, m12, m13 );
	// _m[2] = vec4( m20, m21, m22, m23 );
	// _m[3] = vec4( m30, m31, m32, m33 );
}

mat4::mat4( const mat4& m )
{
	if ( *this != m ) {
		_m[0] = m._m[0];
		_m[1] = m._m[1];
		_m[2] = m._m[2];
		_m[3] = m._m[3];
	} 
}

//
//  --- Indexing Operator ---
//

vec4& mat4::operator [] ( int i ) { return _m[i]; }
const vec4& mat4::operator [] ( int i ) const { return _m[i]; }

//
//  --- (non-modifying) Arithematic Operators ---
//

mat4 mat4::operator + ( const mat4& m ) const
{ return mat4( _m[0]+m[0], _m[1]+m[1], _m[2]+m[2], _m[3]+m[3] ); }

mat4 mat4::operator - ( const mat4& m ) const
{ return mat4( _m[0]-m[0], _m[1]-m[1], _m[2]-m[2], _m[3]-m[3] ); }

mat4 mat4::operator * ( const GLfloat s ) const 
{ return mat4( s*_m[0], s*_m[1], s*_m[2], s*_m[3] ); }

mat4 mat4::operator / ( const GLfloat s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return mat4();
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
}

mat4 mat4::operator * ( const mat4& m ) const {
	mat4  a( 0.0 );
	
	for ( int i = 0; i < 4; ++i ) {
		for ( int j = 0; j < 4; ++j ) {
			for ( int k = 0; k < 4; ++k ) {
				a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	return a;
}

//
//  --- (modifying) Arithematic Operators ---
//

mat4& mat4::operator += ( const mat4& m ) {
	_m[0] += m[0];  _m[1] += m[1];  _m[2] += m[2];  _m[3] += m[3];
	return *this;
}

mat4& mat4::operator -= ( const mat4& m ) {
	_m[0] -= m[0];  _m[1] -= m[1];  _m[2] -= m[2];  _m[3] -= m[3];
	return *this;
}

mat4& mat4::operator *= ( const GLfloat s ) {
	_m[0] *= s;  _m[1] *= s;  _m[2] *= s;  _m[3] *= s;
	return *this;
}

mat4& mat4::operator *= ( const mat4& m ) {
	mat4  a( 0.0 );
	
	for ( int i = 0; i < 4; ++i ) {
		for ( int j = 0; j < 4; ++j ) {
			for ( int k = 0; k < 4; ++k ) {
				a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	return *this = a;
}

mat4& mat4::operator /= ( const GLfloat s ) {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		<< "Division by zero" << std::endl;
		return *this;
	}
#endif // DEBUG
	
	GLfloat r = GLfloat(1.0) / s;
	return *this *= r;
}

//
//  --- Matrix / Vector operators ---
//

vec4 mat4::operator * ( const vec4& v ) const {  // m * v
	return vec4( _m[0][0]*v.x + _m[0][1]*v.y + _m[0][2]*v.z + _m[0][3]*v.w,
				_m[1][0]*v.x + _m[1][1]*v.y + _m[1][2]*v.z + _m[1][3]*v.w,
				_m[2][0]*v.x + _m[2][1]*v.y + _m[2][2]*v.z + _m[2][3]*v.w,
				_m[3][0]*v.x + _m[3][1]*v.y + _m[3][2]*v.z + _m[3][3]*v.w
				);
}

//
//  --- Insertion and Extraction Operators ---
//

void mat4::print( std::ostream& os) const {
	os << std::endl 
	<< _m[0] << std::endl
	<< _m[1] << std::endl
	<< _m[2] << std::endl
	<< _m[3] << std::endl;
}

void mat4::read( std::istream& is) {
	is >> _m[0] >> _m[1] >> _m[2] >> _m[3];
}

//
//  --- Conversion Operators ---
//

mat4::operator const GLfloat* () const
{ return static_cast<const GLfloat*>( &_m[0].x ); }

mat4::operator GLfloat* ()
{ return static_cast<GLfloat*>( &_m[0].x ); }

