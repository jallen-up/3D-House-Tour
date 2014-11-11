/*
 *  torus.h
 *  garland
 *
 *  Created by vegdahl on 8/7/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef TORUS_H
#define TORUS_H
#include "cs432.h"
#include "vec.h"
typedef vec4 point4;
typedef vec4 color4;

extern ObjRef genTorus(color4 color, int hSlices, int vSlices, GLfloat holeDiam, int* idxSpot,
	point4* pointArray, color4* colorArray);
#endif

