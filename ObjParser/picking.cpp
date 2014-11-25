/*
 *  picking.cpp
 *  picking
 *
 *  Created by vegdahl on 9/10/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "cs432.h"
#include "picking.h"
#include "vec.h"

using namespace std;

static pickCallbackFunction* callback = NULL;
static int gpuPickColorId;
static int xVal;
static int yVal;
static GLfloat savedClearColor[4];

static bool doShowPickColors = false;

void showPickColors(bool b) {
	doShowPickColors = b;
	if (b) glClearColor(0,0,0,1);
}

void setGpuPickColorId(int n) {
	gpuPickColorId = n;
	glUniform4fv(gpuPickColorId, 1, vec4(0,0,0,-1));
	if (doShowPickColors) {
		glClearColor(0,0,0,1); //  background
	}
}
void startPicking(pickCallbackFunction* fcn, int x, int y) {
	if (fcn != NULL && callback == NULL) {
		callback = fcn;		
		
		glGetFloatv(GL_COLOR_CLEAR_VALUE, savedClearColor);
		glClearColor(0.0,0.0,0.0,1.0); //  background
		GLint viewportInfo[4];
		glGetIntegerv(GL_VIEWPORT, viewportInfo);
		yVal = viewportInfo[3]-y; // invert y with respect to pixel coordinates
		xVal = x;
		
		glutPostRedisplay();
	}
}

void endPicking(void) {
	if (callback == NULL) return;
	pickCallbackFunction* tempFcn = callback;
	callback = NULL;
	glDisableVertexAttribArray(0);
	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned int data;
	glReadPixels(xVal,yVal,1,1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
	data &= 0xffffff;
	glEnableVertexAttribArray(0);
	glUniform4fv(gpuPickColorId, 1, vec4(0,0,0,-1));
	if (!doShowPickColors) {
		glClearColor(savedClearColor[0],savedClearColor[1],savedClearColor[2],savedClearColor[3]); //  background
	}
	tempFcn(data);
}
bool inPickingMode(void) {
	return callback != NULL;
}

void clearPickId(void) {
	if (doShowPickColors || callback == NULL) return;
	// transparent color, so that object is ignored
	glUniform4fv(gpuPickColorId, 1, vec4(0,0,0,0));
}

void setPickId(int n) {
	if (!doShowPickColors && callback == NULL) return;
	int blue = (n / 256 / 256) & 0xff;
	int green = (n / 256) & 0xff;
	int red = n & 0xff;
	vec4 thisColor(red/255.0 + 0.0001, green/255.0 + 0.0001, blue/255.0 + 0.0001, 1.0);
	glUniform4fv(gpuPickColorId, 1, thisColor);
}

