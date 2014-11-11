//////////////////////////////////////////////////////////////////////////////
//
//  --- mat.h ---
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __MAT_H__
#define __MAT_H__

#include "vec.h"
#include "mat.h"

#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif
#ifndef DegreesToRadians
#  define DegreesToRadians (M_PI / 180.0)
#endif

//  Defined constant for when numbers are too small to be used in the
//    denominator of a division operation.  This is only used if the
//    DEBUG macro is defined.
const GLfloat  DivideByZeroTolerance = GLfloat(1.0e-07);

//----------------------------------------------------------------------------
//
//  mat2 - 2D square matrix
//

class mat2 {
	
    vec2  _m[2];
	
public:
    //
    //  --- Constructors and Destructors ---
    //
	
    mat2( const GLfloat d = GLfloat(1.0) );
	
    mat2( const vec2& a, const vec2& b );
	
    mat2( GLfloat m00, GLfloat m10, GLfloat m01, GLfloat m11 );
	
    mat2( const mat2& m );
    //
    //  --- Indexing Operator ---
    //
	
    vec2& operator [] ( int i );
    const vec2& operator [] ( int i ) const;
	
    //
    //  --- (non-modifying) Arithmatic Operators ---
    //
	
    mat2 operator + ( const mat2& m ) const;
	
    mat2 operator - ( const mat2& m ) const;
	
    mat2 operator * ( const GLfloat s ) const ;
	
    mat2 operator / ( const GLfloat s ) const;
	
    mat2 operator * ( const mat2& m ) const;
	
    //
    //  --- (modifying) Arithmetic Operators ---
    //
	
    mat2& operator += ( const mat2& m );
	
    mat2& operator -= ( const mat2& m );
	
    mat2& operator *= ( const GLfloat s );
	
    mat2& operator *= ( const mat2& m );
    
	mat2& operator /= ( const GLfloat s );
	
    //
    //  --- Matrix / Vector operators ---
    //
	
    vec2 operator * ( const vec2& v ) const;
	
    //
    //  --- Insertion and Extraction Operators ---
    //
	
    void print(std::ostream& os) const;
	
    void read(std::istream& is);
	
    //
    //  --- Conversion Operators ---
    //
	
    operator const GLfloat* () const;
	
    operator GLfloat* ();
};

//
//  --- Non-class mat2 Methods ---
//

inline
mat2 matrixCompMult( const mat2& A, const mat2& B ) {
    return mat2( A[0][0]*B[0][0], A[0][1]*B[0][1],
				A[1][0]*B[1][0], A[1][1]*B[1][1] );
}

inline
mat2 transpose( const mat2& A ) {
    return mat2( A[0][0], A[1][0],
				A[0][1], A[1][1] );
}

inline std::ostream& operator << ( std::ostream& os, const mat2& m ) {
	m.print(os);
	return os;
}

inline std::istream& operator >> ( std::istream& is, mat2& m ) {
	m.read(is);
	return is;
}

inline mat2 operator * ( const GLfloat s, const mat2& m )
{ return m * s; }

//----------------------------------------------------------------------------
//
//  mat3 - 3D square matrix 
//

class mat3 {
	
    vec3  _m[3];
	
public:
    //
    //  --- Constructors and Destructors ---
    //
	
    mat3( const GLfloat d = GLfloat(1.0) );  // Create a diagional matrix
	
    mat3( const vec3& a, const vec3& b, const vec3& c );
	
    mat3( GLfloat m00, GLfloat m10, GLfloat m20,
		 GLfloat m01, GLfloat m11, GLfloat m21,
		 GLfloat m02, GLfloat m12, GLfloat m22 ) ;
	
    mat3( const mat3& m );
	
    //
    //  --- Indexing Operator ---
    //
	
    vec3& operator [] ( int i );
	
    const vec3& operator [] ( int i ) const;
	
    //
    //  --- (non-modifying) Arithmatic Operators ---
    //
	
    mat3 operator + ( const mat3& m ) const;
	
    mat3 operator - ( const mat3& m ) const;
	
    mat3 operator * ( const GLfloat s ) const;
	
    mat3 operator / ( const GLfloat s ) const ;
	
    mat3 operator * ( const mat3& m ) const;
	
    //
    //  --- (modifying) Arithmetic Operators ---
    //
	
    mat3& operator += ( const mat3& m );
	
    mat3& operator -= ( const mat3& m );
	
    mat3& operator *= ( const GLfloat s );
	
    mat3& operator *= ( const mat3& m ) ;
	
    mat3& operator /= ( const GLfloat s );
	
    //
    //  --- Matrix / Vector operators ---
    //
	
    vec3 operator * ( const vec3& v ) const;
	
    //
    //  --- Insertion and Extraction Operators ---
    //
	
    void print( std::ostream& os) const;
	
    void read(std::istream& is);	
	
    //
    //  --- Conversion Operators ---
    //
	
    operator const GLfloat* () const;
	
    operator GLfloat* ();
};

//
//  --- Non-class mat3 Methods ---
//

inline mat3 operator * ( const GLfloat s, const mat3& m )
{ return m * s; }

inline std::ostream& operator << ( std::ostream& os, const mat3& m ) {
	m.print(os);
	return os;
}

inline std::istream& operator >> ( std::istream& is, mat3& m ) { 
	m.read(is);
	return is;
}

inline
mat3 matrixCompMult( const mat3& A, const mat3& B ) {
    return mat3( A[0][0]*B[0][0], A[0][1]*B[0][1], A[0][2]*B[0][2],
				A[1][0]*B[1][0], A[1][1]*B[1][1], A[1][2]*B[1][2],
				A[2][0]*B[2][0], A[2][1]*B[2][1], A[2][2]*B[2][2] );
}

inline
mat3 transpose( const mat3& A ) {
    return mat3( A[0][0], A[1][0], A[2][0],
				A[0][1], A[1][1], A[2][1],
				A[0][2], A[1][2], A[2][2] );
}

//----------------------------------------------------------------------------
//
//  mat4.h - 4D square matrix
//

class mat4 {
	
    vec4  _m[4];
	
public:
    //
    //  --- Constructors and Destructors ---
    //
	
    mat4( const GLfloat d = GLfloat(1.0) ); // Create a diagional matrix
	
    mat4( const vec4& a, const vec4& b, const vec4& c, const vec4& d );
	
    mat4( GLfloat m00, GLfloat m10, GLfloat m20, GLfloat m30,
		 GLfloat m01, GLfloat m11, GLfloat m21, GLfloat m31,
		 GLfloat m02, GLfloat m12, GLfloat m22, GLfloat m32,
		 GLfloat m03, GLfloat m13, GLfloat m23, GLfloat m33 );
	
    mat4( const mat4& m );
	
    //
    //  --- Indexing Operator ---
    //
	
    vec4& operator [] ( int i );
    const vec4& operator [] ( int i ) const;
	
    //
    //  --- (non-modifying) Arithematic Operators ---
    //
	
    mat4 operator + ( const mat4& m ) const;
	
    mat4 operator - ( const mat4& m ) const;
	
    mat4 operator * ( const GLfloat s ) const;
	
    mat4 operator / ( const GLfloat s ) const ;
	
    mat4 operator * ( const mat4& m ) const ;
	
    //
    //  --- (modifying) Arithematic Operators ---
    //
	
    mat4& operator += ( const mat4& m );
	
    mat4& operator -= ( const mat4& m );
	
    mat4& operator *= ( const GLfloat s );
	
    mat4& operator *= ( const mat4& m ) ;
	
    mat4& operator /= ( const GLfloat s ) ;
	
    //
    //  --- Matrix / Vector operators ---
    //
	
    vec4 operator * ( const vec4& v ) const;
	
    //
    //  --- Insertion and Extraction Operators ---
    //
	
    void print ( std::ostream& os) const ;
	
    void read( std::istream& is);
	
    //
    //  --- Conversion Operators ---
    //
	
    operator const GLfloat* () const;
	
    operator GLfloat* ();
};

//
//  --- Non-class mat4 Methods ---
//



inline std::ostream& operator << ( std::ostream& os, const mat4& m ) {
	m.print(os);
	return os;
}

inline std::istream& operator >> ( std::istream& is, mat4& m ) { 
	m.read(is);
	return is;
}

inline mat4 operator * ( const GLfloat s, const mat4& m )
{ return m * s; }

inline
mat4 matrixCompMult( const mat4& A, const mat4& B ) {
    return mat4(
				A[0][0]*B[0][0], A[0][1]*B[0][1], A[0][2]*B[0][2], A[0][3]*B[0][3],
				A[1][0]*B[1][0], A[1][1]*B[1][1], A[1][2]*B[1][2], A[1][3]*B[1][3],
				A[2][0]*B[2][0], A[2][1]*B[2][1], A[2][2]*B[2][2], A[2][3]*B[2][3],
				A[3][0]*B[3][0], A[3][1]*B[3][1], A[3][2]*B[3][2], A[3][3]*B[3][3] );
}

inline
mat4 transpose( const mat4& A ) {
    return mat4( A[0][0], A[1][0], A[2][0], A[3][0],
				A[0][1], A[1][1], A[2][1], A[3][1],
				A[0][2], A[1][2], A[2][2], A[3][2],
				A[0][3], A[1][3], A[2][3], A[3][3] );
}

//////////////////////////////////////////////////////////////////////////////
//
//  Helpful Matrix Methods
//
//////////////////////////////////////////////////////////////////////////////

#define Error( str ) do { std::cerr << "[" __FILE__ ":" << __LINE__ << "] " \
<< str << std::endl; } while(0)

inline
vec4 mvmult( const mat4& a, const vec4& b )
{
    Error( "replace with vector matrix multiplcation operator" );
	
    vec4 c;
    int i, j;
    for(i=0; i<4; i++) {
		c[i] =0.0;
		for(j=0;j<4;j++) c[i]+=a[i][j]*b[j];
    }
    return c;
}

//----------------------------------------------------------------------------
//
//  Rotation matrix generators
//

inline
mat4 RotateX( const GLfloat theta )
{
    GLfloat angle = DegreesToRadians * theta;
	
    mat4 c;
    c[2][2] = c[1][1] = cos(angle);
    c[2][1] = sin(angle);
    c[1][2] = -c[2][1];
    return c;
}

inline
mat4 RotateY( const GLfloat theta )
{
    GLfloat angle = DegreesToRadians * theta;
	
    mat4 c;
    c[2][2] = c[0][0] = cos(angle);
    c[0][2] = sin(angle);
    c[2][0] = -c[0][2];
    return c;
}

inline
mat4 RotateZ( const GLfloat theta )
{
    GLfloat angle = DegreesToRadians * theta;
	
    mat4 c;
    c[0][0] = c[1][1] = cos(angle);
    c[1][0] = sin(angle);
    c[0][1] = -c[1][0];
    return c;
}

//----------------------------------------------------------------------------
//
//  Translation matrix generators
//

inline
mat4 Translate( const GLfloat x, const GLfloat y, const GLfloat z )
{
    mat4 c;
    c[0][3] = x;
    c[1][3] = y;
    c[2][3] = z;
    return c;
}

inline
mat4 Translate( const vec3& v )
{
    return Translate( v.x, v.y, v.z );
}

inline
mat4 Translate( const vec4& v )
{
    return Translate( v.x, v.y, v.z );
}

//----------------------------------------------------------------------------
//
//  Scale matrix generators
//

inline
mat4 Scale( const GLfloat x, const GLfloat y, const GLfloat z )
{
    mat4 c;
    c[0][0] = x;
    c[1][1] = y;
    c[2][2] = z;
    return c;
}

inline
mat4 Scale( const vec3& v )
{
    return Scale( v.x, v.y, v.z );
}

//----------------------------------------------------------------------------
//
//  Projection transformation matrix geneartors
//
//    Note: Microsoft Windows (r) defines the keyword "far" in C/C++.  In
//          order to avoid any name conflicts, we use the variable names
//          "zNear" to reprsent "near", and "zFar" to reprsent "far".
//



inline
mat4 Ortho( const GLfloat left, const GLfloat right,
		   const GLfloat bottom, const GLfloat top,
		   const GLfloat zNear, const GLfloat zFar )
{
    mat4 c;
    c[0][0] = 2.0/(right - left);
    c[1][1] = 2.0/(top - bottom);
    c[2][2] = 2.0/(zNear - zFar);
    c[3][3] = 1.0;
    c[0][3] = -(right + left)/(right - left);
    c[1][3] = -(top + bottom)/(top - bottom);
    c[2][3] = -(zFar + zNear)/(zFar - zNear);
    return c;
}

inline
mat4 Ortho2D( const GLfloat left, const GLfloat right,
			 const GLfloat bottom, const GLfloat top )
{
    return Ortho( left, right, bottom, top, -1.0, 1.0 );
}

inline
mat4 Frustum( const GLfloat left, const GLfloat right,
			 const GLfloat bottom, const GLfloat top,
			 const GLfloat zNear, const GLfloat zFar )
{
    mat4 c;
    c[0][0] = 2.0*zNear/(right - left);
    c[0][2] = (right + left)/(right - left);
    c[1][1] = 2.0*zNear/(top - bottom);
    c[1][2] = (top + bottom)/(top - bottom);
    c[2][2] = -(zFar + zNear)/(zFar - zNear);
    c[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    return c;
}

inline
mat4 Perspective( const GLfloat fovy, const GLfloat aspect,
				 const GLfloat zNear, const GLfloat zFar)
{
    GLfloat top   = tan(fovy*DegreesToRadians/2) * zNear;
    GLfloat right = top * aspect;
	
    mat4 c;
    c[0][0] = zNear/right;
    c[1][1] = zNear/top;
    c[2][2] = -(zFar + zNear)/(zFar - zNear);
    c[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    return c;
}

//----------------------------------------------------------------------------
//
//  Viewing transformation matrix generation (vector parameters)
//
inline
mat4 LookAt( const vec4& eye, const vec4& at, const vec4& up )
{
    vec4 n = normalize(eye - at);
    vec4 u = vec4(normalize(cross(up,n)), 0.0);
    vec4 v = vec4(normalize(cross(n,u)), 0.0);
    vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 c = mat4(u, v, n, t);
    return c * Translate( -eye );
}

//----------------------------------------------------------------------------
//
//  Viewing transformation matrix generation (vertex parameters)
//
inline
mat4 LookAt(GLfloat xEye, GLfloat yEye, GLfloat zEye,
			GLfloat xAt, GLfloat yAt, GLfloat zAt,
			GLfloat xUp, GLfloat yUp, GLfloat zUp)
{
    vec4 eye(xEye,  yEye, zEye, 1.0);
	vec4 at(xAt, yAt, zAt, 1.0);
	vec4 up(xUp, yUp, xUp, 1.0);
	return LookAt(eye, at, up);
}

//----------------------------------------------------------------------------
//
// Generates a Normal Matrix
//
inline
mat3 Normal( const mat4& c)
{
	mat3 d;
	GLfloat det;
	
	det = c[0][0]*c[1][1]*c[2][2]+c[0][1]*c[1][2]*c[2][1]+c[0][2]*c[1][0]*c[2][1]
	-c[2][0]*c[1][1]*c[0][2]-c[1][0]*c[0][1]*c[2][2]-c[0][0]*c[1][2]*c[2][1];
	
	d[0][0] = (c[1][1]*c[2][2]-c[1][2]*c[2][1])/det;
	d[0][1] = -(c[1][0]*c[2][2]-c[1][2]*c[2][0])/det;
	d[0][2] =  (c[1][0]*c[2][1]-c[1][1]*c[2][0])/det;
	d[1][0] = -(c[0][1]*c[2][2]-c[0][2]*c[2][1])/det;
	d[1][1] = (c[0][0]*c[2][2]-c[0][2]*c[2][0])/det;
	d[1][2] = -(c[0][0]*c[2][1]-c[0][1]*c[2][0])/det;
	d[2][0] =  (c[0][1]*c[1][2]-c[0][2]*c[1][1])/det;
	d[2][1] = -(c[0][0]*c[1][2]-c[0][2]*c[1][0])/det;
	d[2][2] = (c[0][0]*c[1][1]-c[1][0]*c[0][1])/det;
	
	return d;
}

//----------------------------------------------------------------------------

inline
vec4 minus(const vec4& a, const vec4&  b )
{
    Error( "replace with vector subtraction" );
    return vec4(a[0]-b[0], a[1]-b[1], a[2]-b[2], 0.0);
}

inline
void printv(const vec4& a )
{
    Error( "replace with vector insertion operator" );
    printf("%f %f %f %f \n\n", a[0], a[1], a[2], a[3]);
}

inline
void printm(const mat4 a)
{
    Error( "replace with matrix insertion operator" );
    for(int i=0; i<4; i++) printf("%f %f %f %f \n", a[i][0], a[i][1], a[i][2], a[i][3]);
    printf("\n");
}

inline
mat4 identity()
{
    Error( "replace with either a matrix constructor or identity method" );
    mat4 c;
    for(int i=0; i<4; i++) for(int j=0; j<4; j++) c[i][j]=0.0;
    for(int i=0; i<4; i++) c[i][i] = 1.0;
    return c;
}

#endif // __MAT_H__
