// Starter file for CS 432, Assignment 2; Fall 2014
// 
// THIS PROGRAM RESPONDS TO KEYBOARD EVENTS!
// The the following keys provide the following functions:
//		'8': Figure 8 pattern
//		'c': Circle pattern
//		'h': Helix pattern
//		's': Toggle rotation around axis of movement
//		']': Add a link to the end of the chain (maximum of 15)
//		'[': Removes the last link in the chain
//		't': Toggle rave mode. WARNING- Do not toggle rave mode if you are prone to epileptic seizures

#include "cs432.h"
#include "vec.h"
#include "mat.h"
#include "matStack.h"
#include "torus.h"
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
#define INIT_WINDOW_YPOS 30
#define INIT_WINDOW_WIDTH 900
#define INIT_WINDOW_HEIGHT 900

// The time between ticks, in milliseconds
#define TICK_INTERVAL 50

// Define the various shape functions
#define CIRCLE   0
#define FIGURE_8 1
#define HELIX    2

// number of vertices our array can store
const int NumVertices = 30000; 
const int MaxLinks = 20;

// These allow the user to toggle various aspects of the functionality
int NumLinks    = 10; //Assign this to a big value for added trippyness
int ShapeFunc   = 1;
bool isSpinning = true;
bool trippy     = false;

// The number of distance units that are in our viewport
#define H_VIEWSIZE 200
#define V_VIEWSIZE 200
#define D_VIEWSIZE 200

// typdefs for readability
typedef vec4 point4;
typedef vec4 color4;

// the amount of time in seconds since the the program started
GLfloat currentTime = 0.0;

static point4 points[NumVertices];
static color4 colors[NumVertices];
static color4 randColors[MaxLinks];

// define our shader code
#define VERTEX_SHADER_CODE "\
attribute  vec4 vPosition; \
attribute  vec4 vColor; \
varying vec4 color; \
\
uniform mat4 ModelView; \
uniform mat4 Projection; \
uniform vec4 VariableColor; \
\
void main() \
{ \
  color = vColor; \
  if ( vColor[3] < 0.0 ) { \
   color = VariableColor; \
} \
  gl_Position = Projection*ModelView*vPosition; \
}  \
"
;
#define FRAGMENT_SHADER_CODE "\
varying  vec4 color; \
void main() \
{ \
gl_FragColor = color; \
} \
"

//----------------------------------------------------------------------------
// our matrix stack
static MatrixStack  mvstack;

// the model-view matrix, defining the current transformation
static mat4         model_view;

// GPU IDs for the projection and model-view matrices
static GLuint       ModelView, Projection, VariableColor;

//----------------------------------------------------------------------------
// the number of vertices we've loaded so far into the points and colors arrays
static int Index[2] = {0, NumVertices};

//----------------------------------------------------------------------------
// a "color" that tells the shader to use the variable color
const color4 colorIsVarying(0.0,0.0,0.0,-1.0);

//----------------------------------------------------------------------------
// sets the shader's variable color
static void setVariableColor(color4 col) {
	glUniform4fv(VariableColor, 1, col);
}

// The current spinning angle
static GLfloat spinningAngle = 0.0;

// boolean that tells whether we are spinning (initially false)
static int spinning = 0;


ObjRef genObject(std::string path, int* idxVar, point4* pointsArray, color4* colorsArray){

	std::vector<point4> vertList;

	cout << "hello";
	// create a file-reading object
	ifstream objFile("monkey.obj");
	if (!objFile.good())
		return 1; // exit if file not found

	string line;


	int startIdx = *idxVar;
	
	int numvs = 0;

	while (getline(objFile,line))
	{
		if (line[0] == 'v' && line[1] == ' '){
			point4 tempv;
			string thisLine = line.substr(2);
			float val;

			string first = thisLine.substr(0, thisLine.find(' '));
			val = (float)atof(first.c_str());
			tempv.x = val;

			string second = thisLine.substr(first.length() + 1, thisLine.find(' '));
			val = (float)atof(second.c_str());
			tempv.y = val;

			string third = thisLine.substr(second.length() + 1, thisLine.find(' '));
			val = (float)atof(third.c_str());
			tempv.z = val;

			std::cout << tempv.x << " " << tempv.y << " " << tempv.z << " " << numvs << endl;
			numvs++;
			vertList.push_back(tempv);
		}

		else if (line[0] == 'f' && line[1] == ' '){
			string thisLine = line.substr(2);
			int val;
			string first = thisLine.substr(0, thisLine.find(' '));
			val = (int)atof(first.c_str());
			pointsArray[*idxVar] = vertList.at(val-1);
			colorsArray[*idxVar] = color4(1.0,0.0,0.0,1.0);
			++*idxVar;

			string second = thisLine.substr(first.length() + 1, thisLine.find(' '));
			val = (float)atof(second.c_str());
			pointsArray[*idxVar] = vertList.at(val-1);
			colorsArray[*idxVar] = color4(1.0, 0.0, 0.0, 1.0);
			++*idxVar;

			string third = thisLine.substr(second.length() + 1, thisLine.find(' '));
			val = (float)atof(third.c_str());
			pointsArray[*idxVar] = vertList.at(val-1);
			colorsArray[*idxVar] = color4(1.0, 0.0, 0.0, 1.0);
			++*idxVar;
		}

	}//End of file reached

	return(startIdx, *idxVar);
}//GenObject




// Performs a rotation along the x- and y-axes based on the time.
// The intent is that this be used to rotate the entire scene.
static void rotate() {
	model_view *= RotateX(spinningAngle);
	model_view *= RotateY(spinningAngle*0.324);
}

//----------------------------------------------------------------------------
// reference to our torus object
static ObjRef torus;

static ObjRef obj;

// returns the x value of the desired figure 8 pattern at a given time
static GLfloat fig8x(GLfloat currentTime){
	GLfloat cycle = fmod(currentTime, 4 * M_PI);
	if (cycle >= 0 && cycle < 2 * M_PI){
		return cos(currentTime) - 1;
	}
	else {
		return -cos(currentTime) + 1;
	}
}

// returns the y value of the desired figure 8 pattern at a given time
static GLfloat fig8y(GLfloat currentTime){
	GLfloat cycle = fmod(currentTime, 4 * M_PI);
	if (cycle >= 0 && cycle < 2 * M_PI){
		return sin(currentTime);
	}
	else {
		return sin(currentTime);
	}
}

// returns the angle at which perpindicular rings should rotate at a given time
static GLfloat fig8rotation(GLfloat currentTime){
	GLfloat cycle = fmod(currentTime, 4 * M_PI);
	if (cycle >= 0 && cycle < 2 * M_PI){
		return cycle*(180 / M_PI);
	}
	else {
		return -cycle*(180 / M_PI);
	}
}

// generates a random color
color4 randColor(){
	
	GLfloat r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	GLfloat g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	GLfloat b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	color4 randCol = color4(r, g, b, 1.0);
	return randCol;
}

//----------------------------------------------------------------------------
// draws our scene
static void drawScene() {
	
	// clear scene, handle depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	// enter domain for this scene's transformations
    mvstack.push(model_view);
	
	// rotate the scene based on the current spinning angle
	model_view *= RotateX(spinningAngle*0.4);
	model_view *= RotateY(spinningAngle*0.75);
	
	// scale the scene so that it fits nicely in the window
	model_view *= Scale(50,50,50);

	// set the spacing of the rings using a small time delay
	float spacing = 0.4;
	if (ShapeFunc == 2){
		// a smaller time delay is more aesthetically pleasing for the helix
		spacing = 0.3;
	}

	/********************************Figure 8*********************************/
	if(ShapeFunc == FIGURE_8){
		// indicates if a ring is perpindicular
		bool perp = false;

		int i = 0;
		for (i = 0; i < NumLinks; i++){
			// enter local transformation domain for this ring
			mvstack.push(model_view);

			GLfloat t = currentTime - spacing*i; // improves readability

			// moves ring to proper place on figure 8
			model_view *= Translate(1.5*fig8x(t), 1.5*fig8y(t), 0);

			if (perp) {
				// rotate perpindicular rings to proper angle
				model_view *= RotateZ(fig8rotation(t));
				model_view *= RotateY(90);

				if (isSpinning){
					// rotate around axis of movement
					model_view *= RotateY(t*(180 / M_PI));
				}
			}
			else {
				if (isSpinning){
					// rotate around axis of movement
					model_view *= RotateZ(fig8rotation(t));
					model_view *= RotateY(t*(180 / M_PI));
				}
			}

			// send the transformation matrix to the GPU
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

			// set the torus' color
			if (trippy){
				setVariableColor(randColor());
			} else{
				setVariableColor(randColors[i]);
			}
			
			// draw the torus
			glDrawArrays(GL_TRIANGLES, obj.getStartIdx(), obj.getCount());
			model_view = mvstack.pop(); // exit local transformations domain

			perp = !perp;
		}
	}
	/********************************Circle*********************************/
	else if(ShapeFunc == CIRCLE) {
		// indicates if a ring is perpindicular
		bool perp = false;

		int i = 0;
		for (i = 0; i < NumLinks; i++){
			// enter local transformation domain for this ring
			mvstack.push(model_view);

			GLfloat t = currentTime - spacing*i; // improves readability

			// moves ring to proper location on circle
			model_view *= Translate(1.5*cos(t), 1.5*sin(t), 0);

			if (perp) {
				// rotate perpindicular rings to proper angle
				model_view *= RotateZ(t*(180 / M_PI));
				model_view *= RotateY(90);

				if (isSpinning){
					// rotate around axis of movement
					model_view *= RotateY(t*(180 / M_PI));
				}
			}
			else {
				if (isSpinning){
					// rotate around axis of movement
					model_view *= RotateZ(t*(180 / M_PI));
					model_view *= RotateY(t*(180 / M_PI));
				}
			}

			// send the transformation matrix to the GPU
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
			// set the torus' color
			if (trippy){
				setVariableColor(randColor());
			}
			else{
				setVariableColor(randColors[i]);
			}
			// draw the torus
			glDrawArrays(GL_TRIANGLES, torus.getStartIdx(), torus.getCount());
			model_view = mvstack.pop(); // exit local transformations domain

			perp = !perp;
		}
	}
	/********************************Helix*********************************/
	else if(ShapeFunc == HELIX){
		int i = 0;
		bool perp = false;
		for (i = 0; i < NumLinks; i++){
			mvstack.push(model_view);
			GLfloat t = currentTime - spacing*i;

			// Wow! Such a cool semi-helical pattern can be represented by such
			// a simple parametic function? Nice one, math!
			model_view *= Translate(2*cos(t), 3*sin(0.2*t), 2*sin(t));

			if (perp) {
				model_view *= RotateY(-t*(180 / M_PI));
				// proof that I still remember how to math
				// arctan of the derivitive of the y component give the angle at which perpindicular rings ascend and descend
				model_view *= RotateX(-atan(0.2*cos(0.2*t))*(180 / M_PI));
				model_view *= RotateX(90);

				if (isSpinning){
					// rotate around axis of movement
					model_view *= RotateY(t*(180 / M_PI));
				}	
			}
			else {
				model_view *= RotateY(90);
				model_view *= RotateY(-t*(180 / M_PI));
				
				if (isSpinning){
					// rotate around axis of movement
					model_view *= RotateX(-t*(180 / M_PI));
				}
			}

			// send the transformation matrix to the GPU
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
			// set the torus' color
			if (trippy){
				setVariableColor(randColor());
			}
			else{
				setVariableColor(randColors[i]);
			}
			// draw the torus
			glDrawArrays(GL_TRIANGLES, torus.getStartIdx(), torus.getCount());
			model_view = mvstack.pop(); // exit local transformations domain

			perp = !perp;
		}
	}
	
	model_view = mvstack.pop();
	// swap buffers so that just-drawn image is displayed
	glutSwapBuffers();
}


//----------------------------------------------------------------------------
// callback function: handles a mouse-press
static void mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// toggle the "pause time" boolean if the left mouse is down
		spinning = !spinning;
	}
}

//----------------------------------------------------------------------------
// callback function: handles a window-resizing
static void reshape(int width, int height) {
    glViewport(0, 0, width, height);
	
	// define the viewport to be 200x200x200, centered at origin
	GLfloat left = -H_VIEWSIZE, right = H_VIEWSIZE;
    GLfloat bottom = -V_VIEWSIZE, top = V_VIEWSIZE;
	GLfloat zNear = -D_VIEWSIZE, zFar = D_VIEWSIZE;
	
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
	
	obj = genObject("blah",Index, points, colors);
	// generate the torus' vertices, with a color of "varying"
	torus = genTorus(colorIsVarying, 20, 20, 0.65, Index, points, colors);	

	//Seed random number generator
	srand(static_cast <unsigned> (time(0)));

	//Assign each ring a random color
	for (color4 &c : randColors){
		c = randColor();
	}
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
	
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
				 NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
					colors );
	
    // Load shaders and use the resulting shader program
    GLuint program = InitShader2(VERTEX_SHADER_CODE, FRAGMENT_SHADER_CODE);
    glUseProgram( program );
	
	// get (and enable) GPU ID for the vertex position
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
						  BUFFER_OFFSET(0) );
	
	// get (and enable) the GPU ID for the vertex color
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
						  BUFFER_OFFSET(sizeof(points)) );
	
	// get the GPU IDs for the two transformation matrices
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
	VariableColor = glGetUniformLocation( program, "VariableColor" );
	
	// enable the depth test so that an objects in front will appear
	// rather than objects that are behind it
	glEnable(GL_DEPTH_TEST);	
	
	// drawing should be of filled-in triangles, not of outlines or points;
	// both backs and fronts should be visible
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// set the background to light gray
    glClearColor(0.7,0.7,0.7,1.0);

}

//----------------------------------------------------------------------------
// callback function: occurs every time the clock ticks: update the angles and redraws the scene
static void tick(int n) {
	// set up next "tick"
	glutTimerFunc(n, tick, n);
	
	// advance the clock
	currentTime += TICK_INTERVAL/1000.0;
	
	if (spinning) {
		spinningAngle += 4;
	}
	
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
		case 'c':
			ShapeFunc = CIRCLE;
			break;
		case '8':
			ShapeFunc = FIGURE_8;
			break;
		case 'h':
			ShapeFunc = HELIX;
			break;
		case 's':
			isSpinning = !isSpinning;
			break;
		case ']':
			if (NumLinks < 15){
				NumLinks++;
			}
			break;
		case '[':
			if (NumLinks > 0){
				NumLinks--;
			}
			break;
		case 't':
			trippy = !trippy;
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
	glutCreateWindow("CS 432, Assignment 2");
	
	// initialize GLEW, if needed
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
