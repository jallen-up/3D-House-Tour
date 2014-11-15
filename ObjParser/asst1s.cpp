// Starter file for CS 432, Assignment 1.
// *****THIS IS THE FILE THAT STUDENTS SHOULD EDIT*****

#include "cs432.h"
#include "vec.h"
#include "mat.h"
#include "matStack.h"
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

// The current spinning angle of our five objects
static GLfloat globalAngle = 0.0;

// boolean that tells whether we are spinning (initially true)
static int spinning = true;

// define typedefs for readability
typedef vec4 point4;
typedef vec4 color4;

// a large number, to ensure that there is enough room
const int NumVertices = 100000; 

// arrays of points and colors, to send to the graphics card
static point4 points[NumVertices];
static color4 colors[NumVertices];
static vec3 normals[NumVertices];


const int  TextureSize = 64;
GLuint textures[2];

GLubyte image[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];

vec2    tex_coords[NumVertices];

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLuint   theta;


//----------------------------------------------------------------------------
// our matrix stack
static MatrixStack  mvstack;

// the model-view matrix, defining the current transformation
static mat4         model_view;

// GPU IDs for the projection and model-view matrices
static GLuint       ModelView, Projection;

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
			string thisLine = line.substr(2);
			float val;

			string first = thisLine.substr(0, thisLine.find(' '));
			val = (float)atof(first.c_str());
			tempt.x = val;

			string second = thisLine.substr(thisLine.find_last_of(' ') + 1);
			val = (float)atof(second.c_str());
			tempt.y = val;

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
			
			string firstt = firstel.substr(firstv.length() + 1, firstel.find('/'));
			val = (int)atof(firstt.c_str());
			texArray[*idxVar] = texList.at(val - 1);

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

			string secondt = secondel.substr(secondv.length() + 1, secondel.find('/'));
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

			string thirdt = thirdel.substr(thirdv.length() + 1, thirdel.find('/'));
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
// creates a skinny triangle
//  - this function is for demo purposes
static ObjRef genSkinnyTriangle(color4 theColor, GLfloat zVal, int *idxVar, point4 *pointsArray, color4 *colorArray) {
	
	// save start index, as it will be part of our return value
	int startIdx = *idxVar;
	
	// add the first vertex to the array(s)
	pointsArray[*idxVar] = point4(-0.1, -0.5, zVal, 1.0);
	colorArray[*idxVar] = theColor;
	(*idxVar)++;
	
	// add second vertex
	pointsArray[*idxVar] = point4(0.1, -0.5, zVal, 1.0);
	colorArray[*idxVar] = theColor;
	(*idxVar)++;
	
	// add third vertex
	pointsArray[*idxVar] = point4(0.0, 0.5, zVal, 1.0);
	colorArray[*idxVar] = theColor;
	(*idxVar)++;
	
	// return the object reference (first and last point)
	return ObjRef(startIdx, *idxVar);
}

//----------------------------------------------------------------------------
// This function should generate an equlateral triangle, centered approximately at (0,0,0)
static ObjRef genShape0(int *idxVar, point4 *pointsArray, color4 *colorArray) {
	
	// save start index, as it will be part of our return value
	int startIdx = *idxVar;
	
	// generate a white triangle
	genSkinnyTriangle(color4(1.0,1.0,1.0,1.0), 0.0, idxVar, pointsArray, colorArray);	
	
	// return the object reference (first and last point)
	return ObjRef(startIdx, *idxVar);
}

// This function should generate a polygon of at least 5 sides, and at most 20 sides,
// centered approximately at (0,0,0).
static ObjRef genShape1(int *idxVar, point4 *pointsArray, color4 *colorArray) {
	
	// save start index, as it will be part of our return value
	int startIdx = *idxVar;
	
	// generate a red triangle
	genSkinnyTriangle(color4(1.0,0.0,0.0,1.0), 0.0, idxVar, pointsArray, colorArray);	
	
	// return the object reference (first and last point)
	return ObjRef(startIdx, *idxVar);
	
}

// This function should generate a 1x1x1 cube, centered at (0,0,0).
static ObjRef genShape2(int *idxVar, point4 *pointsArray, color4 *colorArray) {
	
	// save start index, as it will be part of our return value
	int startIdx = *idxVar;
	
	// generate a medium green triangle
	genSkinnyTriangle(color4(0.0,0.75,0.0,1.0), 0.0, idxVar, pointsArray, colorArray);	
	
	// return the object reference (first and last point)
	return ObjRef(startIdx, *idxVar);
}

// This function should generate a tetrahedron or a pyramid, centered approximately
// at (0,0,0).
static ObjRef genShape3(int *idxVar, point4 *pointsArray, color4 *colorArray) {
	
	// save start index, as it will be part of our return value
	int startIdx = *idxVar;
	
	// generate a blue triangle
	genSkinnyTriangle(color4(0.0,0.0,1.0,1.0), 0.0, idxVar, pointsArray, colorArray);	
	
	// return the object reference (first and last point)
	return ObjRef(startIdx, *idxVar);
	
}

// This function can be left as a right triangle, or it can generate a polyhedra with more than
// six sides.
static ObjRef genShape4(int *idxVar, point4 *pointsArray, color4 *colorArray) {
	
	// save start index, as it will be part of our return value
	int startIdx = *idxVar;
	
	// generate a yellow triangle
	genSkinnyTriangle(color4(1.0,1.0,0.0,1.0), 0.0, idxVar, pointsArray, colorArray);	
	
	// return the object reference (first and last point)
	return ObjRef(startIdx, *idxVar);
	
}

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
// Updates both the global rotation of the objects around the center (globalAngle),
// but only if 'spinning' is true.
static void updateAngles() {
	if (spinning) { // only apply transforms if we're spinning
		// modify each local angle entry by a small, relatively uniform amount
		for (int i = 0; i < sizeof localAngles/sizeof *localAngles; i++) {
			localAngles[i][0] += 5.0+randUniform();
			localAngles[i][1] += 3.0+randUniform();
		}
		
		// modify the global angle by two degrees
		globalAngle += 2.0;
	}
}

//----------------------------------------------------------------------------
// draws our scene
//   CS 432 students should NOT modlfy this function
static void drawScene() {
	// clear scene, handle depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// enter domain for local transformations
    mvstack.push(model_view);
	
	float s = 2.2;
	// scale the scene so that it fits nicely in the window
	model_view *= Scale(s, s, s);
	
	// rotate the entire scene by the global angle
    model_view *= RotateZ(globalAngle);
	
	// rotate each individual piece
	for (int i = 0; i < NUM_OBJECTS; i++) {
		mvstack.push( model_view ); // enter local transformation domain
		model_view *= Translate(30, 0, 0); // move 30 away from center in (logical) x-direction
		model_view *= Scale(20, 20, 20); // make it larger by a factor of 20
		model_view *= RotateX(localAngles[i][0]); // rotate by current x-rotation angle
		model_view *= RotateY(localAngles[i][1]); // rotate by current y-rotation angle
		
		// send the transformation matrix to the GPU
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		
		glUniform3fv(theta, 1, Theta);

		// draw the (transformed) object
		glDrawArrays(GL_TRIANGLES, objects[i].getStartIdx(), objects[i].getCount());
		
		model_view = mvstack.pop(); // undo transformations for the just-drawn object
		
		// rotate by 1/Nth of circle to go to position for next object
		model_view *= RotateZ(360.0/NUM_OBJECTS);
	}
	
	model_view = mvstack.pop(); // undo transformations for this entire draw
	
	// swap buffers so that just-drawn image is displayed
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
// callback function: handles a mouse-press
static void mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// toggle the "spinning" boolean if the left mouse is down
		spinning = !spinning;
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
    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
	
	// define the model-view matrix so that it initially does no transformations
    model_view = mat4( 1.0 );   // An Identity matrix
}

//----------------------------------------------------------------------------
// initialize the world
//----------------------------------------------------------------------------
static void init(void) {

	// generate the objects, storing a reference to each in the 'objects' array
	objects[0] = genObject("monkey.obj", &Index, points, colors, normals,tex_coords);
	//objects[1] = genObject("cube.obj", &Index, points, colors, normals, tex_coords);
	//objects[2] = genShape2(&Index, points, colors);
	//objects[3] = genShape3(&Index, points, colors);
	//objects[4] = genShape4(&Index, points, colors);

	// Create a checkerboard pattern
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			image[i][j][0] = c;
			image[i][j][1] = c;
			image[i][j][2] = c;
			image2[i][j][0] = c;
			image2[i][j][1] = 0;
			image2[i][j][2] = c;
		}
	}


	// Initialize texture objects
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image);
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
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
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
	point4 light_position(1.0, 1.0, 1.0, 0.0);
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

	glEnable(GL_DEPTH_TEST);

	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
	glShadeModel(GL_SMOOTH);
}


//----------------------------------------------------------------------------
// callback function: occurs every time the clock ticks: update the angles and redraws the scene
static void tick(int n) {
	// set up next "tick"
	glutTimerFunc(n, tick, n);
	
	// update our angles
	updateAngles();
	
	// draw the new scene
	drawScene();
}

//----------------------------------------------------------------------------
// callback function, responding to a key-press. Program will terminate if the escape key
// or upper- or lower-case 'Q' is pressed.
static void keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
		case 033: // Escape Key
		case 'q': case 'Q':
			exit( EXIT_SUCCESS );
			break;
    }
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

