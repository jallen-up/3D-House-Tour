// Starter file for CS 432, Assignment 5.

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
#include "picking.h"
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

#define NUM_OBJECTS 14

// define our shader code
#define VERTEX_SHADER_CODE "\
attribute vec4 vPosition; \
attribute vec3 vNormal;\
attribute vec2 vTexCoord;\
attribute vec4 vColor;\
varying vec2 texCoord;\
varying vec4 color; \
varying vec4 initPick; \
uniform vec4 LightPositions[7];\
uniform int LightStates[7];\
uniform int LightIntensities[7];\
uniform vec4 SceneAmbient;\
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;\
uniform mat4 ModelView; \
uniform mat4 Projection; \
uniform mat4 CollisionProjection;\
uniform vec4 VariableColor; \
uniform vec4 LightPosition;\
uniform vec4 PickColor;\
uniform float Shininess;\
\
void main() \
{ \
initPick = PickColor;\
if (PickColor.a >= 0.0) { \
color = PickColor; \
}\
else {  \
texCoord = vTexCoord;\
vec3 pos = (ModelView * vPosition).xyz;\
vec3 test = (ModelView * LightPosition).xyz;\
vec3 L = normalize(test.xyz - pos);\
vec3 E = normalize(-pos);\
vec3 H = normalize(L + E);\
vec3 N = vNormal;\
vec4 ambient = SceneAmbient;\
vec4 diffuse = vec4(0.0,0.0,0.0,1.0);\
vec4 specular = vec4(0.0,0.0,0.0,1.0);\
int i = 0;\
for(i = 0; i < 7; i++){\
	if( LightStates[i] == 0 ){\
		ambient += vec4(0.0,0.0,0.0,1.0);\
		diffuse += vec4(0.0,0.0,0.0,1.0);\
		specular += vec4(0.0,0.0,0.0,1.0);\
	} else { \
		vec3 L = normalize(LightPositions[i].xyz - vPosition.xyz); \
		vec3 H = normalize(L + E);\
		float Kd = max(dot(L, N), 0.0);\
		diffuse += LightIntensities[i] *(Kd*DiffuseProduct) / distance(LightPositions[i].xyz,vPosition.xyz);\
		float Ks = pow(max(dot(N, H), 0.0), Shininess);\
		vec4 specular_i = LightIntensities[i]* (Ks * SpecularProduct)/distance(LightPositions[i].xyz,vPosition.xyz);\
		if (dot(L, N) < 0.0) {\
			specular += vec4(0.0, 0.0, 0.0, 1.0);\
		} else {\
			specular += specular_i;\
		}\
	}\
}\
	color = ambient + diffuse + specular;\
}\
gl_Position = Projection*ModelView*vPosition; \
}  \
"
#define FRAGMENT_SHADER_CODE "\
varying vec2 texCoord;\
varying  vec4 color; \
varying vec4 initPick; \
uniform sampler2D texture;\
void main() \
{ \
if (initPick.a >= 0.0) {\
gl_FragColor = color; \
}\
else { \
gl_FragColor = color*texture2D(texture,texCoord); \
} \
} \
"

// define typedefs for readability
typedef vec4 point4;
typedef vec4 color4;

// a large number, to ensure that there is enough room
const int NumVertices = 10000000; 

// arrays of points and colors, to send to the graphics card
static point4 points[NumVertices];
static color4 colors[NumVertices];
static vec3 normals[NumVertices];


const int  TextureSize = 1024;
const int trick = 128;
GLuint textures[14];
bool mapTextures;


GLubyte image[TextureSize][TextureSize][3];
GLubyte image1[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];
GLubyte image3[TextureSize][TextureSize][3];
GLubyte image4[TextureSize][TextureSize][3];
GLubyte image5[TextureSize][TextureSize][3];
GLubyte image6[TextureSize][TextureSize][3];
GLubyte image7[TextureSize][TextureSize][3];
GLubyte image8[TextureSize][TextureSize][3];
GLubyte image9[TextureSize][TextureSize][3];
GLubyte image10[TextureSize][TextureSize][3];
GLubyte image11[TextureSize][TextureSize][3];
GLubyte image12[TextureSize][TextureSize][3];
GLubyte image13[TextureSize][TextureSize][3];


GLubyte houseImage[4096][4096][3];

vec2    tex_coords[NumVertices];

// the amount of time in seconds since the the program started
GLfloat currentTime = 0.0;

// door variables
static int CLOSED = 0;
static int OTC = 1; //Open to Close
static int CTO = 2; //Close to Open
static int OPEN = 3;
static const int numDoors = 8;
int doorStates[numDoors];
int rotateDoor[numDoors];

// Light Switch Variables
static int OFF= 0;
static int ON = 1;
static const int numLights = 7;
int lightStates[numLights];
int lightIntensities[numLights];
// TV variable
static bool tv1IsOn = false;
static bool tv2IsOn = false;

//Scene ambient
vec4 sceneAmbient;
bool collisionView = false;
bool normalView = true;
bool gravityView = false;

color4 backgroundColor;
int x_rotation = 0;
GLfloat personHeight = 100.0;

GLuint program;
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

//-----------------------------------------------------------------------------
//dominoFall-callback
static void activateCallback(int code) {
	cout << "pickID: " << code << endl;
	if ((code != 0) && (doorStates[code - 1] == CLOSED)){
		doorStates[code - 1] = CTO;
	}
	if (code == 13) {
		lightStates[1] = ON;
		glUniform1i(glGetUniformLocation(program, "LightStates[1]"), lightStates[1]);
	}
	if (code == 14) {
		lightStates[2] = ON;
		glUniform1i(glGetUniformLocation(program, "LightStates[2]"), lightStates[2]);
	}
	if (code == 15) {
		lightStates[3] = ON;
		glUniform1i(glGetUniformLocation(program, "LightStates[3]"), lightStates[3]);
	}
	if (code == 16){
		lightStates[4] = ON;
		glUniform1i(glGetUniformLocation(program, "LightStates[4]"), lightStates[4]);
	}
	if (code == 17){
		lightStates[5] = ON;
		glUniform1i(glGetUniformLocation(program, "LightStates[5]"), lightStates[5]);
	}
	if (code == 18){
		lightStates[6] = ON;
		glUniform1i(glGetUniformLocation(program, "LightStates[6]"), lightStates[6]);
	}
	if (code == 20) tv1IsOn = true;
	if (code == 21) tv2IsOn = true;
}

static void deactivateCallback(int code){
	cout << "pickID: " << code << endl;
	if ((code != 0) && (doorStates[code - 1] == OPEN)){
		doorStates[code - 1] = OTC;
	}
	if (code == 13){
		lightStates[1] = 0;
		glUniform1i(glGetUniformLocation(program, "LightStates[1]"), lightStates[1]);
	}
	if (code == 14) {
		lightStates[2] = 0;
		glUniform1i(glGetUniformLocation(program, "LightStates[2]"), lightStates[2]);
	}
	if (code == 15) {
		lightStates[3] = 0;
		glUniform1i(glGetUniformLocation(program, "LightStates[3]"), lightStates[3]);
	}
	if (code == 16){
		lightStates[4] = 0;
		glUniform1i(glGetUniformLocation(program, "LightStates[4]"), lightStates[4]);
	}
	if (code == 17){
		lightStates[5] = 0;
		glUniform1i(glGetUniformLocation(program, "LightStates[5]"), lightStates[5]);
	}
	if (code == 18){
		lightStates[6] = 0;
		glUniform1i(glGetUniformLocation(program, "LightStates[6]"), lightStates[6]);
	}
	if (code == 20) tv1IsOn = false;
	if (code == 21) tv2IsOn = false;
}



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

	
			string firstv = firstel.substr(0, firstel.find('/'));
			val = (int)atof(firstv.c_str());
			pointsArray[*idxVar] = vertList.at(val - 1);

			string temp = firstel.substr(firstv.length()+1);
			string firstt = temp.substr(0, temp.find('/'));
			
			
			val = (int)atof(firstt.c_str());
			texArray[*idxVar] = texList.at(val - 1);
			

			string firstn = firstel.substr(firstel.find_last_of('/')+1);
			val = (int)atof(firstn.c_str());
			vec4 test = normList.at(val - 1);
		
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
// draws our scene
//   CS 432 students should NOT modlfy this function
static void drawScene() {
	// clear scene, handle depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	model_view = model_view_start;

	model_view = RotateX(x_rotation)*model_view;
	// enter domain for local transformations
    mvstack.push(model_view);
	
	GLfloat s = 40;
	// scale the scene so that it fits nicely in the window
	model_view *= Scale(s, s, s);
	
	//Doors
	for (int i = 0; i < numDoors; i++){
		mvstack.push( model_view );
		//initial door transformations
		switch (i) {
		case 0:
			model_view *= Translate( 1.5, 1, 10);
			break;
		case 1:
			model_view *= RotateY(90);
			model_view *= Translate(14, 1, 5);
			break;
		case 2:
			model_view *= RotateY(90);
			model_view *= Translate(14, 1, -5);
			break;
		case 3:
			model_view *= RotateY(90);
			model_view *= Translate(-5.5, 1, 5);
			break;
		case 4:
			model_view *= RotateY(90);
			model_view *= Translate(-5.5, 1, -5);
			break;
		case 5:
			model_view *= RotateY(90);
			model_view *= Translate(14, 7, -5);
			break;
		case 6:
			model_view *= RotateY(90);
			model_view *= Translate(10.5, 7, 10);
			break;
		case 7:
			model_view *= RotateY(90);
			model_view *= Translate(-2, 7, 10);
			break;
		}

		// ------ Door States -------
		// Closed to Open Movement
		if (doorStates[i] == CTO){
			rotateDoor[i] -= 5.0;
			model_view *= RotateY(rotateDoor[i]);

			if (rotateDoor[i] <= -90){
				doorStates[i] = OPEN;
			}
		}

		// Open to Closed Movement
		if (doorStates[i] == OTC){
			rotateDoor[i] += 5.0;
			model_view *= RotateY(rotateDoor[i]);

			if (rotateDoor[i] >= 0){
				doorStates[i] = CLOSED;
			}
		}

		// Open Door
		if (doorStates[i] == OPEN){
			model_view *= RotateY(rotateDoor[i]);
		}


		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(i + 1);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[1]);
		// draw the (transformed) door objects
		glDrawArrays(GL_TRIANGLES, objects[0].getStartIdx(), objects[0].getCount());

		clearPickId();

		model_view = mvstack.pop();
	}

		//House
		mvstack.push( model_view ); 
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(1 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[0]);
		glDrawArrays(GL_TRIANGLES, objects[1].getStartIdx(), objects[1].getCount());
		clearPickId();
		model_view = mvstack.pop(); 

		//Couch
		mvstack.push(model_view);
		model_view *= Translate(18, 1, -2);
		model_view *= RotateY(90);
		model_view *= Scale(1.3, 1.3, 1.3);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(2 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[2]);
		glDrawArrays(GL_TRIANGLES, objects[2].getStartIdx(), objects[2].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//bookshelf
		mvstack.push(model_view);
		model_view *= Translate(11.5, 1, -14);
		model_view *= RotateY(180);
		model_view *= Scale(1.3, 1.3, 1.3);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(3 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[3]);
		glDrawArrays(GL_TRIANGLES, objects[3].getStartIdx(), objects[3].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//end_table
		mvstack.push(model_view);
		model_view *= Translate(18, 0, 8);
		model_view *= RotateY(45);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(4 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[4]);
		glDrawArrays(GL_TRIANGLES, objects[4].getStartIdx(), objects[4].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//Living room lamp
		mvstack.push(model_view);
		model_view *= Translate(18, 1, -12);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(5 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[13]);
		glDrawArrays(GL_TRIANGLES, objects[5].getStartIdx(), objects[5].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//Dining room lamp
		mvstack.push(model_view);
		model_view *= Translate(-18, 1, -12);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(6 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[13]);
		glDrawArrays(GL_TRIANGLES, objects[5].getStartIdx(), objects[5].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//Master Bedroom lamp
		mvstack.push(model_view);
		model_view *= Translate(-18, 7, -12);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(7 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[13]);
		glDrawArrays(GL_TRIANGLES, objects[5].getStartIdx(), objects[5].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//bathroom lamp
		mvstack.push(model_view);
		model_view *= Translate(18, 7, -12);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(8 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[13]);
		glDrawArrays(GL_TRIANGLES, objects[5].getStartIdx(), objects[5].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//Medium bedroom lamp
		mvstack.push(model_view);
		model_view *= Translate(18, 7, 8);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(9 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[13]);
		glDrawArrays(GL_TRIANGLES, objects[5].getStartIdx(), objects[5].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//Foyer lamp
		mvstack.push(model_view);
		model_view *= Translate(9, 7, -2);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(9 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[13]);
		glDrawArrays(GL_TRIANGLES, objects[5].getStartIdx(), objects[5].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//bed
		mvstack.push(model_view);
		model_view *= Translate(15, 7, 0);
		model_view *= Scale(1.2, 1, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(10 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[5]);
		glDrawArrays(GL_TRIANGLES, objects[6].getStartIdx(), objects[6].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//bed
		mvstack.push(model_view);
		model_view *= Translate(-11.5, 7, 9);
		model_view *= RotateY(180);
		model_view *= Scale(2, 1, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(11 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[5]);
		glDrawArrays(GL_TRIANGLES, objects[6].getStartIdx(), objects[6].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//tv1
		mvstack.push(model_view);
		model_view *= Translate(6, 2, -2);
		model_view *= RotateY(90);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(12 + numDoors);
		if (tv1IsOn){
			if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[7]);
		}
		else{
			if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[6]);
		}
		glDrawArrays(GL_TRIANGLES, objects[7].getStartIdx(), objects[7].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//tv2
		mvstack.push(model_view);
		model_view *= Translate(-6, 8, -2);
		model_view *= RotateY(-90);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(13 + numDoors);
		if (tv2IsOn){
			if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[7]);
		}
		else{
			if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[6]);
		}
		glDrawArrays(GL_TRIANGLES, objects[7].getStartIdx(), objects[7].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//left tree
		mvstack.push(model_view);
		model_view *= Translate(-6, 0, 15);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(14 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[8]);
		glDrawArrays(GL_TRIANGLES, objects[8].getStartIdx(), objects[8].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//right tree
		mvstack.push(model_view);
		model_view *= Translate(6, 0, 15);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(15 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[8]);
		glDrawArrays(GL_TRIANGLES, objects[8].getStartIdx(), objects[8].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//bathtub
		mvstack.push(model_view);
		model_view *= Translate(18, 7, -3);
		model_view *= RotateY(90);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(16 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[9]);
		glDrawArrays(GL_TRIANGLES, objects[9].getStartIdx(), objects[9].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//table
		mvstack.push(model_view);
		model_view *= Translate(-15, 1, 2);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(17 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[10]);
		glDrawArrays(GL_TRIANGLES, objects[10].getStartIdx(), objects[10].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//chair1
		mvstack.push(model_view);
		model_view *= Translate(-15, 1, -0.5);
		model_view *= Scale(0.8, 0.8, 0.8);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(18 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[4]);
		glDrawArrays(GL_TRIANGLES, objects[11].getStartIdx(), objects[11].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//chair2
		mvstack.push(model_view);
		model_view *= Translate(-15, 1, 4.5);
		model_view *= RotateY(180);
		model_view *= Scale(0.8, 0.8, 0.8);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(19 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[4]);
		glDrawArrays(GL_TRIANGLES, objects[11].getStartIdx(), objects[11].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//chair2
		mvstack.push(model_view);
		model_view *= Translate(-14, 1, 2.25);
		model_view *= RotateY(-90);
		model_view *= Scale(0.8, 0.8, 0.8);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(20 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[4]);
		glDrawArrays(GL_TRIANGLES, objects[11].getStartIdx(), objects[11].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//chair4
		mvstack.push(model_view);
		model_view *= Translate(-16, 1, 2.25);
		model_view *= RotateY(90);
		model_view *= Scale(0.8, 0.8, 0.8);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(21 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[4]);
		glDrawArrays(GL_TRIANGLES, objects[11].getStartIdx(), objects[11].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//toilet
		mvstack.push(model_view);
		model_view *= Translate(12, 7, -12);
		model_view *= RotateY(90);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(22 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[11]);
		glDrawArrays(GL_TRIANGLES, objects[12].getStartIdx(), objects[12].getCount());
		clearPickId();
		model_view = mvstack.pop();

		//fridge
		mvstack.push(model_view);
		model_view *= Translate(-6, 1, -8);
		model_view *= RotateY(-90);
		model_view *= Scale(1, 0.8, 1);
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		setPickId(23 + numDoors);
		if (mapTextures) glBindTexture(GL_TEXTURE_2D, textures[12]);
		glDrawArrays(GL_TRIANGLES, objects[13].getStartIdx(), objects[13].getCount());
		clearPickId();
		model_view = mvstack.pop();
	
	model_view = mvstack.pop(); // undo transformations for this entire draw
}

//----------------------------------------------------------------------------
// draws our scene
//   Collission detection for the mouse
static bool detectMouseCollisions() {

	glClearColor(1.0, 0.0, 1.0, 1.0);

	GLfloat left = -100.0, right = 100.0;
	GLfloat bottom = -100.0, top = 100.0;
	GLfloat zNear = -100.0, zFar = 100.0;

	GLfloat aspect = GLfloat(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

	if (aspect > 1.0) {
		left *= aspect;
		right *= aspect;
	}
	else {
		bottom /= aspect;
		top /= aspect;
	}

	mat4 collision = Ortho(left / 2, right / 2, bottom, top, zNear / 2, zFar / 2);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, collision);

	//draw the scene in ortho
	drawScene();
	int collisionWidth = glutGet(GLUT_WINDOW_WIDTH) / 20;
	int collisionHeight = glutGet(GLUT_WINDOW_HEIGHT) / 20;
	int centerPixelX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int centerPixelY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	bool foundObject = false;
	bool foundObject1 = false;
	int i, j;

	for (i = 0; i < collisionWidth; i++){
		for (j = 0; j < collisionHeight; j++){
			unsigned int data;
			glReadPixels(centerPixelX - (collisionWidth / 2) + i, centerPixelY - (collisionHeight / 2) + j, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			data &= 0xffffff;
			//if we hit something, return true
			if (data != 0xff00ff){
				foundObject = true;
			}

		}
	}

	if (collisionView) glutSwapBuffers();

	mat4 projection1 = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
	projection1 = Perspective(50, 1, 1.0, 20000);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection1);

	glClearColor(0.529, 0.808, 0.980, 1.0);

	//if we hit something return true
	if (foundObject){
		return true;
	}
	else{
		return false;
	}
}


//----------------------------------------------------------------------------
// Draws the scene in ortho view and writes to the frame buffer.   The frame 
// Buffer is then read to look for a color other than megenta,  If it finds a color
// other than the background color(magenta), it returns true, there has been a collision. 
//  
static bool detectCollisions(unsigned char key) {

	glClearColor(1.0, 0.0, 1.0, 1.0);

	GLfloat left = -100.0, right = 100.0;
	GLfloat bottom = -100.0, top = 100.0;
	GLfloat zNear = -100.0, zFar = 100.0;

	GLfloat aspect = GLfloat(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

	if (aspect > 1.0) {
		left *= aspect;
		right *= aspect;
	}
	else {
		bottom /= aspect;
		top /= aspect;
	}

	mat4 collision = Ortho(left/2, right/2, bottom, top, zNear/2, zFar/2);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, collision);

	//draw the scene in ortho
	drawScene();
	int collisionWidth = glutGet(GLUT_WINDOW_WIDTH) / 20;
	int collisionHeight = glutGet(GLUT_WINDOW_HEIGHT) / 20;
	int centerPixelX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int centerPixelY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	bool foundObject = false;
	bool foundObject1 = false;
	int i,j;

	for (i = 0; i < collisionWidth; i++){
		for (j = 0; j < collisionHeight; j++){
			unsigned int data;
			glReadPixels(centerPixelX - (collisionWidth / 2) + i, centerPixelY - (collisionHeight / 2) + j , 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			data &= 0xffffff;
			//if we hit something, return true
			if (data != 0xff00ff){
				foundObject = true;
			}
			
		}
	}

	for (int k = 0; k < 10; k++){
		unsigned int data;
		glReadPixels(centerPixelX, k, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
		data &= 0xffffff;
		//if we hit something, return true
		if (data != 0xff00ff){
			foundObject1 = true;
		}
	}
	
	//stair finder
	if ((foundObject1) && (!foundObject)){
		switch (key){
		case 'w': case 'W':
			model_view_start = Translate(0, -25, 2)*model_view_start;
			break;
		case 's': case 'S':
			model_view_start = Translate(0, -25, -2)*model_view_start;
			break;
		case 'a': case 'A':
			model_view_start = Translate(2, -25, 0)*model_view_start;
			break;
		case 'd': case 'D':
			model_view_start = Translate(-2, -25, 0)*model_view_start;
			break;
		}


	}

	if (collisionView) glutSwapBuffers();

	mat4 projection1 = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
	projection1 = Perspective(50, 1, 1.0, 20000);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection1);

	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
	
	//if we hit something return true
	if (foundObject){
		return true;
	}
	else{
		return false;
	}
}

//----------------------------------------------------------------------------
// gravity function
static void gravity(){
	glClearColor(1.0, 0.0, 1.0, 1.0);

	GLfloat left = -100.0, right = 100.0;
	GLfloat bottom = -100.0, top = 100.0;
	GLfloat zNear = -100.0, zFar = 100.0;

	GLfloat aspect = GLfloat(glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT));

	int centerPixelX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int centerPixelY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	if (aspect > 1.0) {
		left *= aspect;
		right *= aspect;
	}
	else {
		bottom /= aspect;
		top /= aspect;
	}

	mat4 collision = Ortho(left / 2, right / 2, bottom / 2, top / 2, zNear / 2, zFar / 2);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, collision);
	//model_view_start = Translate(0, 80, 0)*model_view_start;
	model_view_start = RotateX(90)*model_view_start;

	//draw the scene in ortho
	drawScene();
	
	while (true){
		//bottom
		unsigned int data;
		glReadPixels(centerPixelX, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
		data &= 0xffffff;
		if (data != 0xff00ff) break;

		//top
		glReadPixels(centerPixelX, GLUT_WINDOW_HEIGHT, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
		data &= 0xffffff;
		if (data != 0xff00ff) break;

		model_view_start = Translate(0, 0, 5)*model_view_start;

		drawScene();
	}
			
	model_view_start = RotateX(-90)*model_view_start;
	model_view_start = Translate(0, -80, 0)*model_view_start;
	
	if (gravityView) glutSwapBuffers();

	mat4 projection1 = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
	projection1 = Perspective(50, 1, 1.0, 20000);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection1);

	glClearColor(0.529, 0.808, 0.980, 1.0);

}

//----------------------------------------------------------------------------
// callback function: handles a mouse-press
static void mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		cout << "mouse: " << x << ", " << y << endl;
		startPicking(activateCallback, x, y);
	}

	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		cout << "mouse: " << x << ", " << y << endl;
		startPicking(deactivateCallback, x, y);
	}
}

static void mouseMove(int x, int y){
	int screen_width = glutGet(GLUT_WINDOW_WIDTH);
	int screen_height = glutGet(GLUT_WINDOW_HEIGHT);

	int x_center = screen_width / 2; 
	int y_center = screen_height / 2;

	if (x - x_center > 10){
		copymv = model_view_start;
		model_view_start = RotateY(5) * model_view_start;
		//insert dummy key
		if (detectCollisions('m')) model_view_start = copymv;
		glutWarpPointer(x_center, y_center);
		return;
	}
	if (x - x_center < -10){
		copymv = model_view_start;
		model_view_start = RotateY(-5) * model_view_start;
		//insert dummy key
		if (detectCollisions('n')) model_view_start = copymv;
		glutWarpPointer(x_center, y_center);
		return;
	}
	if (y - y_center > 10){
		x_rotation += 5;
		glutWarpPointer(x_center, y_center);
		return;
	}
	if (y - y_center < -10){
		x_rotation -= 5;
		glutWarpPointer(x_center, y_center);
		return;
	}
}

//---------------------------------------------------------------------------
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
	mat4 projection = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
	projection = Perspective(50, 1, 1.0, 20000);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	// define the model-view matrix so that it initially does no transformations
    model_view = mat4( 1.0 );   // An Identity matrix
}

//----------------------------------------------------------------------------
// initialize the world
//----------------------------------------------------------------------------
static void init(void) {

	// Load shaders and use the resulting shader program
	program = InitShader2(VERTEX_SHADER_CODE, FRAGMENT_SHADER_CODE);
	
	char* objFiles[NUM_OBJECTS] = { "door.obj", "house.obj", "couch.obj", "bookshelf.obj", "end_table.obj", "lamp.obj", "bed.obj", "tv.obj",
									"tree.obj", "bathtub.obj", "table.obj", "chair.obj", "toilet.obj", "fridge.obj"};

	//parse .obj files
	for (int i = 0; i < NUM_OBJECTS; i++){
		objects[i] = genObject(objFiles[i], &Index, points, colors, normals, tex_coords);
	}

	static GLfloat pic[1024][1024][3];
	static GLfloat pic2[4096][4096][3];

	// Initialize texture objects
	glGenTextures(14, textures);

	

	readPpmImage("house.ppm", (GLfloat*)pic2, 0, 0, 4096, 4096);
	gluScaleImage(GL_RGB, 4096, 4096, GL_FLOAT, pic2, 4096, 4096, GL_BYTE, houseImage);

	//Read .ppm texture image
	readPpmImage("door.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image);

	readPpmImage("couch.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image1);
	readPpmImage("bookshelf.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image2);
	readPpmImage("bed.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image4);
	readPpmImage("tv.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image5);
	readPpmImage("tv_on.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image6);
	readPpmImage("tree.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image7);
	readPpmImage("bathtub.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image8);
	readPpmImage("toilet.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image11);
	readPpmImage("fridge.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image12);
	readPpmImage("lamp.ppm", (GLfloat*)pic, 0, 0, TextureSize, TextureSize);
	gluScaleImage(GL_RGB, 1024, 1024, GL_FLOAT, pic, 1024, 1024, GL_BYTE, image13);


	// Initialize texture objects
	glGenTextures(14, textures);
	
	//house
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4096, 4096, 0, GL_RGB, GL_UNSIGNED_BYTE, houseImage);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//door
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//couch
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//bookshelf
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//endtable
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//bed
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//tv
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//tvon
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image6);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//tree
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image7);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//bathtub
	glBindTexture(GL_TEXTURE_2D, textures[9]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image8);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//table
	glBindTexture(GL_TEXTURE_2D, textures[10]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//toilet
	glBindTexture(GL_TEXTURE_2D, textures[11]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image11);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//fridge
	glBindTexture(GL_TEXTURE_2D, textures[12]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image12);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//lamp
	glBindTexture(GL_TEXTURE_2D, textures[13]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image13);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glActiveTexture(GL_TEXTURE0);

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
		BUFFER_OFFSET(sizeof(points) + sizeof(normals)));

	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	//Material properties
	color4 material_ambient(0.0, 0.5, 0.5, 1.0);
	color4 material_diffuse(0.5, 0.5, 0.5, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float  material_shininess = 1.0;

	//Light Properties
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(0.5, 0.5, 0.5, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);

	point4 light_position(2000.0, 0.0, 0.0, 0.0); //Sun
	point4 light_position1(17.0, 5.0, -14, 0.0); //Living room
	point4 light_position2(-17.0, 5.0, -14, 0.0); //Kitchen
	point4 light_position3(-17.0, 10.0, -14, 0.0); //Master Bedroom
	point4 light_position4(17.0, 10.0, -14, 0.0); //Bathroom
	point4 light_position5(17.0, 10.0, 6, 0.0); //Small Bedroom	
	point4 light_position6(6.0, 10.0, -2.0, 0.0); //Foyer

	glUniform4fv(glGetUniformLocation(program, "LightPositions[0]"), 1, light_position);
	glUniform4fv(glGetUniformLocation(program, "LightPositions[1]"), 1, light_position1);
	glUniform4fv(glGetUniformLocation(program, "LightPositions[2]"), 1, light_position2);
	glUniform4fv(glGetUniformLocation(program, "LightPositions[3]"), 1, light_position3);
	glUniform4fv(glGetUniformLocation(program, "LightPositions[4]"), 1, light_position4);
	glUniform4fv(glGetUniformLocation(program, "LightPositions[5]"), 1, light_position5);
	glUniform4fv(glGetUniformLocation(program, "LightPositions[6]"), 1, light_position6);

	//Initialize the ambient lighting for the scene
	sceneAmbient = vec4(0.5, 0.5, 0.5, 1.0);
	glUniform4fv(glGetUniformLocation(program, "SceneAmbient"), 1, sceneAmbient);

	for (int i = 0; i < 7; i++){
		lightStates[i] = 0;
	}
	lightStates[0] = 1;

	//SEND LIGHT STATES
	glUniform1i(glGetUniformLocation(program, "LightStates[0]"), lightStates[0]);
	glUniform1i(glGetUniformLocation(program, "LightStates[1]"), lightStates[1]);
	glUniform1i(glGetUniformLocation(program, "LightStates[2]"), lightStates[2]);
	glUniform1i(glGetUniformLocation(program, "LightStates[3]"), lightStates[3]);
	glUniform1i(glGetUniformLocation(program, "LightStates[4]"), lightStates[4]);
	glUniform1i(glGetUniformLocation(program, "LightStates[5]"), lightStates[5]);
	glUniform1i(glGetUniformLocation(program, "LightStates[6]"), lightStates[6]);

	for (int i = 0; i < 7; i++){
		lightIntensities[i] = 10;
	}
	lightIntensities[0] = 200;

	glUniform1i(glGetUniformLocation(program, "LightIntensities[0]"), lightIntensities[0]);
	glUniform1i(glGetUniformLocation(program, "LightIntensities[1]"), lightIntensities[1]);
	glUniform1i(glGetUniformLocation(program, "LightIntensities[2]"), lightIntensities[2]);
	glUniform1i(glGetUniformLocation(program, "LightIntensities[3]"), lightIntensities[3]);
	glUniform1i(glGetUniformLocation(program, "LightIntensities[4]"), lightIntensities[4]);
	glUniform1i(glGetUniformLocation(program, "LightIntensities[5]"), lightIntensities[5]);
	glUniform1i(glGetUniformLocation(program, "LightIntensities[6]"), lightIntensities[6]);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	CollisionView = glGetUniformLocation(program, "CollisionView");
	CollisionProjection = glGetUniformLocation(program, "CollisionProjection");
	setGpuPickColorId(glGetUniformLocation(program, "PickColor"));

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	backgroundColor = color4(0.529, 0.808, 0.980, 1.0);
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0); /* light blue background*/
	glShadeModel(GL_SMOOTH);

	// Starting position for the camera	
	model_view_start = Translate(0, -140, -1000);

}


//----------------------------------------------------------------------------
// callback function: occurs every time the clock ticks: update the angles and redraws the scene
static void tick(int n) {
	// set up next "tick"
	glutTimerFunc(n, tick, n);

	// advance the clock
	currentTime += TICK_INTERVAL / 1000.0;

	glUniform1i(glGetUniformLocation(program, "LightStates[0]"), lightStates[0]);
	glUniform1i(glGetUniformLocation(program, "LightStates[1]"), lightStates[1]);
	glUniform1i(glGetUniformLocation(program, "LightStates[2]"), lightStates[2]);
	glUniform1i(glGetUniformLocation(program, "LightStates[3]"), lightStates[3]);
	glUniform1i(glGetUniformLocation(program, "LightStates[4]"), lightStates[4]);
	glUniform1i(glGetUniformLocation(program, "LightStates[5]"), lightStates[5]);
	glUniform1i(glGetUniformLocation(program, "LightStates[6]"), lightStates[6]);

	// draw the new scene
	glutPostRedisplay();


	if (inPickingMode()) {

		GLfloat left = -100.0, right = 100.0;
		GLfloat bottom = -100.0, top = 100.0;
		GLfloat zNear = -100.0, zFar = 100.0;

		mat4 DistanceProjection = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
		DistanceProjection = Perspective(50, 1, 1.0, 300);
		glUniformMatrix4fv(Projection, 1, GL_TRUE, DistanceProjection);

		//draw the scene with smaller perspective
		drawScene();
		endPicking();

		mat4 projection1 = Frustum(left, right, bottom, top, 1.0, zNear + zFar);
		projection1 = Perspective(50, 1, 1.0, 20000);
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection1);

	}
	else {
		// draw the new scene
		mapTextures = true;
		drawScene();
		mapTextures = false;

		//swap the buffers
		if (normalView) glutSwapBuffers();
	}
}

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
		if (detectCollisions(key)) model_view_start = copymv;
		break;
	case 's': case 'S':
		copymv = model_view_start;
		model_view_start = Translate(0, 0, -6.0*factor)*model_view_start;
		if (detectCollisions(key)) model_view_start = copymv;
		break;
	case 'a': case 'A':
		copymv = model_view_start;
		model_view_start = Translate(6.0*factor, 0, 0)*model_view_start;
		model_view_start = RotateY(-90)*model_view_start;
		if (detectCollisions(key)) {
			model_view_start = copymv;
		}
		else{
			model_view_start = RotateY(90)*model_view_start;
		}
		break;
	case 'd': case 'D':
		copymv = model_view_start;
		model_view_start = Translate(-6.0*factor, 0, 0)*model_view_start;
		model_view_start = RotateY(90)*model_view_start;
		if (detectCollisions(key)) {
			model_view_start = copymv;
		}
		else{
			model_view_start = RotateY(-90)*model_view_start;
		}
		break;
	case 'r': case 'R':
		copymv = model_view_start;
		model_view_start = Translate(0, -6.0*factor, 0)*model_view_start;
		model_view_start = RotateX(-90)*model_view_start;
		if (detectCollisions(key)) {
			model_view_start = copymv;
		}
		else{
			model_view_start = RotateX(90)*model_view_start;
		}
		break;
	case 'f': case 'F':
		copymv = model_view_start;
		model_view_start = Translate(0, 6.0*factor, 0)*model_view_start;
		model_view_start = RotateX(90)*model_view_start;
		if (detectCollisions(key)) {
			model_view_start = copymv;
		}
		else{
			model_view_start = RotateX(-90)*model_view_start;
		}
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
	case '[':
		backgroundColor = color4(0.0, 0.0, 0.14, 1.0);
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
		sceneAmbient = vec4(0.1, 0.1, 0.1, 1.0);
		glUniform4fv(glGetUniformLocation(program, "SceneAmbient"), 1, sceneAmbient);
		break;
	case ']':
		backgroundColor = color4(0.529, 0.808, 0.980, 1.0);
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
		sceneAmbient = vec4(0.6, 0.6, 0.6, 1.0);
		glUniform4fv(glGetUniformLocation(program, "SceneAmbient"), 1, sceneAmbient);
		break;
	case '1'://collision view
		collisionView = true;
		gravityView = false;
		normalView = false;
		break;
	case '2'://gravity view
		collisionView = false;
		gravityView = true;
		normalView = false;
		break;
	case '3'://normal view
		collisionView = false;
		gravityView = false;
		normalView = true;
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
	glutCreateWindow("CS 432, House of Awesomenessssss");
	//glutFullScreen();
	glewInit();

	// call the initializer function
    init();
	
	// set up callback functions
	glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
	glutTimerFunc(TICK_INTERVAL, tick, TICK_INTERVAL); // timer callback
    glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouseMove);
	glutSetCursor(GLUT_CURSOR_NONE);

	// start processing
    glutMainLoop();
	
	// (we should never get here)
    return 0;
}

