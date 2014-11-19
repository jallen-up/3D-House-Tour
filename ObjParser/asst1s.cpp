// Starter file for CS 432, Assignment 1.
// *****THIS IS THE FILE THAT STUDENTS SHOULD EDIT*****

#include "cs432.h"
#include "vec.h"
#include "mat.h"
#include "matStack.h"
#include "ppm.h"
#include <fstream>
#include <sstream>
#include <istream>
#include <assert.h>
#include <stdio.h>
#include <ctime>
#include <vector>
using namespace std;

// The initial size and position of our window
#define INIT_WINDOW_XPOS 100
#define INIT_WINDOW_YPOS 100
#define INIT_WINDOW_WIDTH 500
#define INIT_WINDOW_HEIGHT 500

// The time between ticks, in milliseconds
#define TICK_INTERVAL 50

// The number of distance units that are in our viewport
#define VIEWSIZE 120

#define NUM_OBJECTS 5

// define our shader code
#define VERTEX_SHADER_CODE "\
attribute vec4 vPosition; \
attribute vec3 vNormal;\
attribute vec2 vTexCoord;\
attribute vec4 vColor;\
varying vec2 texCoord;\
varying vec4 color; \
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;\
uniform mat4 ModelView; \
uniform mat4 Projection; \
uniform mat4 CollisionProjection;\
uniform vec4 VariableColor; \
uniform vec4 LightPosition;\
uniform float Shininess;\
\
void main() \
{ \
texCoord = vTexCoord;\
vec3 pos = (ModelView * vPosition).xyz;\
vec3 L = normalize(LightPosition.xyz - pos);\
vec3 E = normalize(-pos);\
vec3 H = normalize(L + E);\
vec3 N = vNormal;\
vec4 ambient = AmbientProduct;\
float Kd = max(dot(L, N), 0.0);\
vec4  diffuse = Kd*DiffuseProduct;\
float Ks = pow(max(dot(N, H), 0.0), Shininess);\
vec4  specular = Ks * SpecularProduct;\
if (dot(L, N) < 0.0) {\
specular = vec4(0.0, 0.0, 0.0, 1.0);\
}\
color = ambient + diffuse + specular;\
color.a = 1.0;\
gl_Position = Projection*ModelView*vPosition; \
}  \
"
#define FRAGMENT_SHADER_CODE "\
varying vec2 texCoord;\
varying  vec4 color; \
uniform sampler2D texture;\
void main() \
{ \
gl_FragColor = color*texture2D(texture,texCoord); \
} \
"

// define typedefs for readability
typedef vec4 point4;
typedef vec4 color4;

// a large number, to ensure that there is enough room
const int NumVertices = 1000000; 

// arrays of points and colors, to send to the graphics card
static point4 points[NumVertices];
static color4 colors[NumVertices];
static vec3 normals[NumVertices];


const int  TextureSize = 1024;
const int trick = 128;
GLuint textures[2];

GLubyte image[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];

vec2    tex_coords[NumVertices];

// the amount of time in seconds since the the program started
GLfloat currentTime = 0.0;

// the current forward speed
static double currentSpeed = 0.0;

//----------------------------------------------------------------------------
// our matrix stack
static MatrixStack  mvstack;

// the model-view matrix, defining the current transformation
static mat4         model_view, model_view_start, copymv;

// GPU IDs for the projection and model-view matrices
static GLuint       ModelView, Projection, CollisionView, CollisionProjection;

//----------------------------------------------------------------------------
// the number of vertices we've loaded so far into the points and colors arrays
static int Index = 0;


//Parses a .obj file
//currently only supports vertices
ObjRef genObject(string path, int* idxVar, point4* pointsArray, color4* colorsArray, vec3* normalArray, vec2*texArray){
	//store the starting index of the object
	int startIdx = *idxVar;

	//A list of vertices in the object
	std::vector<point4> vertList;
	std::vector<vec3> normList;
	std::vector<vec2> texList;
	//Open the file
	ifstream objFile(path);
	if (!objFile.good())
		return 1; 

	string line;
	while (getline(objFile, line))
	{
		//PARSE VERTICES
		if (line[0] == 'v' && line[1] == ' '){
			point4 tempv;
			string thisLine = line.substr(2);
			float val;

			//Get the first value after the 'v'
			string first = thisLine.substr(0, thisLine.find(' '));
			val = (float)atof(first.c_str());
			tempv.x = val;

			//Get the second value after the 'v'
			string second = thisLine.substr(first.length() + 1, thisLine.find(' '));
			val = (float)atof(second.c_str());
			tempv.y = val;

			//Get the third value after the 'v'
			string third = thisLine.substr(thisLine.find_last_of(' ') + 1);
			val = (float)atof(third.c_str());
			tempv.z = val;
			tempv.w = 1.0;

			//store the vertex
			vertList.push_back(tempv);
		}
		//PARSE TEXTURES
		else if (line[0] == 'v' && line[1] == 't'){
			vec2 tempt;
			string thisLine = line.substr(3);
			float val;

			string first = thisLine.substr(0, thisLine.find(' '));
			val = (float)atof(first.c_str());
			tempt.x = val;

			string second = thisLine.substr(thisLine.find_last_of(' ') + 1);
			val = (float)atof(second.c_str());
			tempt.y = val;

			cout << tempt.x << " " << tempt.y << endl;
			texList.push_back(tempt);
		}
		//PARSE NORMALS
		else if (line[0] == 'v' && line[1] == 'n'){
			vec3 tempn;
			string thisLine = line.substr(3);
			float val;

			//Get the first value after the 'vn'
			string first = thisLine.substr(0, thisLine.find(' '));
			val = (float)atof(first.c_str());
			tempn.x = val;

			//Get the second value after the 'vn'
			string second = thisLine.substr(first.length() + 1, thisLine.find(' '));
			val = (float)atof(second.c_str());
			tempn.y = val;

			//Get the third value after the 'vn'
			string third = thisLine.substr(thisLine.find_last_of(' ') + 1);
			val = (float)atof(third.c_str());
			tempn.z = val;

			//store the vertex
			normList.push_back(tempn);
		}
		//PARSE FACES
		else if (line[0] == 'f' && line[1] == ' '){
			string thisLine = line.substr(2);
			int val;

			string firstel = thisLine.substr(0, thisLine.find(' '));
			string jjj = thisLine.substr(firstel.length()+1);
			string secondel = jjj.substr(0, jjj.find(' '));
			string thirdel = thisLine.substr(thisLine.find_last_of(' '));

			//cout << firstel << " " << secondel << " " << thirdel << endl;
			string firstv = firstel.substr(0, firstel.find('/'));
			val = (int)atof(firstv.c_str());
			pointsArray[*idxVar] = vertList.at(val - 1);

			string temp = firstel.substr(firstv.length()+1);
			string firstt = temp.substr(0, temp.find('/'));
			
			//std::cout << firstt << endl;
			val = (int)atof(firstt.c_str());
			texArray[*idxVar] = texList.at(val - 1);
			//std::cout << texList.at(val - 1).x << " " << texList.at(val - 1).y << endl;

			string firstn = firstel.substr(firstel.find_last_of('/')+1);
			val = (int)atof(firstn.c_str());
			vec4 test = normList.at(val - 1);
			//cout << test;
			normalArray[*idxVar] = normList.at(val - 1);
		
			colorsArray[*idxVar] = color4(1.0, 0.0, 0.0, 1.0);
			(*idxVar)++;



			string secondv = secondel.substr(0, secondel.find('/'));
			val = (int)atof(secondv.c_str());
			pointsArray[*idxVar] = vertList.at(val - 1);

			temp = secondel.substr(secondv.length() + 1);
			string secondt = temp.substr(0, temp.find('/'));
			val = (int)atof(secondt.c_str());
			texArray[*idxVar] = texList.at(val - 1);

			string secondn = secondel.substr(secondel.find_last_of('/')+1);
			val = (int)atof(secondn.c_str());
			normalArray[*idxVar] = normList.at(val - 1);

			colorsArray[*idxVar] = color4(1.0, 0.0, 0.0, 1.0);
			(*idxVar)++;



			string thirdv = thirdel.substr(0, thirdel.find('/'));
			val = (int)atof(thirdv.c_str());
			pointsArray[*idxVar] = vertList.at(val - 1);

			temp = thirdel.substr(thirdv.length() + 1);
			string thirdt = temp.substr(0, temp.find('/'));
			val = (int)atof(thirdt.c_str());
			texArray[*idxVar] = texList.at(val - 1);

			string thirdn = thirdel.substr(thirdel.find_last_of('/')+1);
			val = (int)atof(thirdn.c_str());
			normalArray[*idxVar] = normList.at(val - 1);

			colorsArray[*idxVar] = color4(1.0, 0.0, 0.0, 1.0);
			(*idxVar)++;
	
		}

	}//End of file reached
	return ObjRef(startIdx, *idxVar);
}//GenObject


//----------------------------------------------------------------------------
// a helper-function that returns a random number in the range [0.0,1.0)
static GLfloat randUniform() {
	return rand()/(GLfloat)RAND_MAX;
}

//----------------------------------------------------------------------------
// array of GPU object references
static ObjRef objects[NUM_OBJECTS];

//----------------------------------------------------------------------------
// The current orientation of each of our objects.  The first element of each pair
// specifies an x-axis rotation; the second specifies a y-axis rotation
static GLfloat localAngles[NUM_OBJECTS][2] = {
{23.0, 45.0},
{34.0, 91.0},
{264.0, 7.0},
{67.0, 334.0},
{179.0, 146.0},
};

//----------------------------------------------------------------------------
// draws our scene
//   CS 432 students should NOT modlfy this function
static void drawScene() {
	// clear scene, handle depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	model_view = model_view_start;

	// enter domain for local transformations
    mvstack.push(model_view);
	
	float s = 2.2;
	// scale the scene so that it fits nicely in the window
	model_view *= Scale(s, s, s);
		
	// rotate each individual piece
	for (int i = 0; i < NUM_OBJECTS; i++) {
		mvstack.push( model_view ); // enter local transformation domain
		//model_view *= Translate(30, 0, 0); // move 30 away from center in (logical) x-direction
		model_view *= Scale(20, 20, 20); // make it larger by a factor of 20
		
		// send the transformation matrix to the GPU
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

		// draw the (transformed) object
		glDrawArrays(GL_TRIANGLES, objects[i].getStartIdx(), objects[i].getCount());
		
		model_view = mvstack.pop(); // undo transformations for the just-drawn object
		
	}
	
	model_view = mvstack.pop(); // undo transformations for this entire draw
}

//----------------------------------------------------------------------------
// Draws the scene in ortho view and writes to the frame buffer.   The frame 
// Buffer is then read to look for a color other than megenta,  If it finds a color
// other than the background color(magenta), it returns true, there has been a collision. 
//  
static bool detectCollisions() {

	glClearColor(1.0, 0.0, 1.0, 1.0);

	GLfloat left = -50.0, right = 50.0;
	GLfloat bottom = -100.0, top = 100.0;
	GLfloat zNear = -100.0, zFar = 100.0;

	mat4 collision = Ortho(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, collision);

	//draw the scene in ortho
	drawScene();
	
	unsigned int data;
	glReadPixels(1, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
	data &= 0xffffff;

	mat4 projection1 = Frustum(left*2, right*2, bottom, top, 1.0, zNear + zFar);
	projection1 = Perspective(50, 1, 1.0, 20000);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection1);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	//if we hit something, return true
	if (data != 0xff00ff){
		return true;
	}
	//if we dont hit anything, return false
	return false;
}

//----------------------------------------------------------------------------
// callback function: handles a mouse-press
static void mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	
	}
}

//----------------------------------------------------------------------------
// callback function: handles a window-resizing
static void reshape(int width, int height) {
    glViewport(0, 0, width, height);
	
	// define the viewport to be 200x200x200, centered at origin
	GLfloat left = -100.0, right = 100.0;
    GLfloat bottom = -100.0, top = 100.0;
	GLfloat zNear = -100.0, zFar = 100.0;
	
	// scale everything by the smallest of the two window-dimensions, so that
	// everything is visible. The larger dimension will "see" more
    GLfloat aspect = GLfloat( width ) / height;
	
    if ( aspect > 1.0 ) {
        left *= aspect;
        right *= aspect;
    }
    else {
        bottom /= aspect;
        top /= aspect;
    }
	
	// define the projection matrix, based on the computed dimensions;
	// send the matrix to the GPU
    mat4 collision = Ortho( left, right, bottom, top, zNear, zFar );
	mat4 projection = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
	projection = Perspective(50, 1, 1.0, 20000);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	glUniformMatrix4fv( CollisionProjection, 1, GL_TRUE, collision );
	// define the model-view matrix so that it initially does no transformations
    model_view = mat4( 1.0 );   // An Identity matrix
}

//----------------------------------------------------------------------------
// initialize the world
//----------------------------------------------------------------------------
static void init(void) {

	// generate the objects, storing a reference to each in the 'objects' array
	objects[0] = genObject("testroom.obj", &Index, points, colors, normals,tex_coords);
	//objects[1] = genObject("cube.obj", &Index, points, colors, normals, tex_coords);
	//objects[2] = genShape2(&Index, points, colors);
	//objects[3] = genShape3(&Index, points, colors);
	//objects[4] = genShape4(&Index, points, colors);

	 //Create a checkerboard pattern
	/*for (int i = 0; i < TextureSize; i++) {
		for (int j = 0; j < TextureSize; j++) {
			GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			image[i][j][0] = c;
			image[i][j][1] = c;
			image[i][j][2] = c;
			image2[i][j][0] = c;
			image2[i][j][1] = 0;
			image2[i][j][2] = c;
		}
	}*/

	static GLfloat pic[1024][1024][3];

	//readPpmImage("monkey_lowpoly_ascii.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	//readPpmImage("monkey_lowpoly_ascii.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	readPpmImage("monkey_lowpoly_ascii.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(
		GL_RGB, // as in GL_RGB
		1024, // width of existing image, in pixels
		1024, // height of existing image, in pixels
		GL_FLOAT, // type of data in existing image, as in GL_FLOAT
		pic, // pointer to first element of existing image
		1024, // width of new image
		1024, // height of new image
		GL_BYTE, // type of new image, as in GL_FLOAT
		image); // pointer to buffer for holding new image

	gluScaleImage(
		GL_RGB, // as in GL_RGB
		1024, // width of existing image, in pixels
		1024, // height of existing image, in pixels
		GL_FLOAT, // type of data in existing image, as in GL_FLOAT
		pic, // pointer to first element of existing image
		1024, // width of new image
		1024, // height of new image
		GL_BYTE, // type of new image, as in GL_FLOAT
		image2); // pointer to buffer for holding new image

	//readPpmImage("monkey_lowpoly_ascii.ppm", (GLubyte*)image2, 0, 0, 2048, 2048);
	// Initialize texture objects
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(tex_coords),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points),
		sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(tex_coords),tex_coords);

	

	// Load shaders and use the resulting shader program
	//GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	GLuint program = InitShader2(VERTEX_SHADER_CODE, FRAGMENT_SHADER_CODE);

	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)+sizeof(normals)));

	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	// Initialize shader lighting parameters
	point4 light_position(1.0, 1.0, -1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(0.0, 0.5, 0.5, 1.0);
	color4 material_diffuse(0.5, 0.5, 0.5, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float  material_shininess = 1.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	CollisionView = glGetUniformLocation(program, "CollisionView");
	CollisionProjection = glGetUniformLocation(program, "CollisionProjection");
	

	glEnable(GL_DEPTH_TEST);

	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
	glShadeModel(GL_SMOOTH);

	// Starting position for the camera
	
	model_view_start = Translate(0, -200, -1000);
	model_view_start *= RotateY(-90);
	//model_view_start *= RotateX(50);
	//model_view_start *= RotateZ(10);
}


//----------------------------------------------------------------------------
// callback function: occurs every time the clock ticks: update the angles and redraws the scene
static void tick(int n) {
	// set up next "tick"
	glutTimerFunc(n, tick, n);

	// advance the clock
	currentTime += TICK_INTERVAL / 1000.0;

	// position based on speed
	//model_view_start = Translate(0, 0, currentSpeed)*model_view_start;

	// draw the new scene
	glutPostRedisplay();

	// draw the new scene
	drawScene();

	//swap the buffers
	glutSwapBuffers();
}

int toggler = 0;
//----------------------------------------------------------------------------
// callback function, responding to a key-press. Program will terminate if the escape key
// or upper- or lower-case 'Q' is pressed.
static void keyboard( unsigned char key, int x, int y )
{
	
	int factor = (key <= 'Z') ? 6.0 : 1.0;
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w': case 'W':
		copymv = model_view_start;
		model_view_start = Translate(0, 0, 6.0*factor)*model_view_start;

		if (detectCollisions()){
			model_view_start = copymv;
		}
		break;
	case 's': case 'S':
		copymv = model_view_start;
		model_view_start = Translate(0, 0, -6.0*factor)*model_view_start;

		if (detectCollisions()){
			model_view_start = copymv;
		}
		break;
	case 'x': case 'X':
		currentSpeed = 0.0;
		break;
	case 'a': case 'A':
		model_view_start = Translate(1.0*factor, 0, 0)*model_view_start;
		break;
	case 'd': case 'D':
		model_view_start = Translate(-1.0*factor, 0, 0)*model_view_start;
		break;
	case 'r': case 'R':
		model_view_start = Translate(0, -1.0*factor, 0)*model_view_start;
		break;
	case 'f': case 'F':
		model_view_start = Translate(0, 1.0*factor, 0)*model_view_start;
		break;
	case 'j': case 'J':
		model_view_start = RotateY(-0.5*factor)*model_view_start;
		break;
	case 'l': case 'L':
		model_view_start = RotateY(0.5*factor)*model_view_start;
		break;
	case 'u': case 'U':
		model_view_start = RotateZ(-1.5*factor)*model_view_start;
		break;
	case 'o': case 'O':
		model_view_start = RotateZ(1.5*factor)*model_view_start;
		break;
	case 'i': case 'I':
		model_view_start = RotateX(-1.5*factor)*model_view_start;
		break;
	case 'k': case 'K':
		model_view_start = RotateX(1.5*factor)*model_view_start;
		break;
	case 'b':
		
		/*GLfloat left = -100.0, right = 100.0;
		GLfloat bottom = -100.0, top = 100.0;
		GLfloat zNear = -100.0, zFar = 100.0;
		
		mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
		
		
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);*/
		break;
	case 'g':
		/*GLfloat left1 = -100.0, right1 = 100.0;
		GLfloat bottom1 = -100.0, top1 = 100.0;
		GLfloat zNear1 = -100.0, zFar1 = 100.0;

		mat4 projection1 = Frustum(left1, right1, bottom1, top1, 1.0, zNear1 + zFar1);
		projection1 = Perspective(50, 1, 1.0, 20000);
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection1);*/
		break;

	}//NAV SWITCH
}

//----------------------------------------------------------------------------
// main program
int main( int argc, char **argv ) {
	
	// set up the window
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowPosition(INIT_WINDOW_XPOS, INIT_WINDOW_YPOS);
	glutInitWindowSize(INIT_WINDOW_WIDTH,INIT_WINDOW_HEIGHT);
	glutCreateWindow("CS 432, Assignment 1");
	///glutFullScreen();
	glewInit();

	// call the initializer function
    init();
	
	// set up callback functions
	glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
	glutTimerFunc(TICK_INTERVAL, tick, TICK_INTERVAL); // timer callback
    glutMouseFunc(mouse);
	
	// start processing
    glutMainLoop();
	
	// (we should never get here)
    return 0;
}

