/*
 *  torus.cpp
 *  garland
 *
 *  Created by vegdahl on 8/7/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "cs432.h"
#include "torus.h"

static void genVertex(color4 color, point4 p, int *idxSpot, point4* pointArray, color4* colorArray) {
	if (pointArray != NULL) {
		pointArray[*idxSpot] = p;
		colorArray[*idxSpot] = color;
	}
	++*idxSpot;
}

static void genQuad(color4 color, point4 p1, point4 p2, point4 p3, point4 p4, int *idxSpot,
					point4* pointArray, color4* colorArray) {
	if (idxSpot[0] + 6 > idxSpot[1]) {
		*idxSpot += 6;
	}
	else {
		genVertex(color, p1, idxSpot, pointArray, colorArray);
		genVertex(color, p2, idxSpot, pointArray, colorArray);
		genVertex(color, p3, idxSpot, pointArray, colorArray);
		genVertex(color, p1, idxSpot, pointArray, colorArray);
		genVertex(color, p3, idxSpot, pointArray, colorArray);
		genVertex(color, p4, idxSpot, pointArray, colorArray);
	}
}


extern ObjRef genTorus(color4 color, int hSlices, int vSlices, GLfloat holeDiam, int* idxSpot,
					   point4* pointArray, color4* colorArray) {
	int indexStart = *idxSpot;
	GLfloat centerRadius = 0.25+(0.25*holeDiam) ;
	GLfloat sliceRadius = 0.25-(0.25*holeDiam);
	GLfloat vAngleDelta = 2*M_PI/vSlices;
	GLfloat hAngleDelta = 2*M_PI/hSlices;
    for (int h = 0; h < hSlices; h++) {
		for(int v = 0; v < vSlices; v++) {
			point4 points[2][2];
			for (int i = 0; i <= 1; i++) {
				for (int j = 0; j <= 1; j++) {
					GLfloat hAngle = (h+i)*hAngleDelta;
					GLfloat vAngle = (v+j)*vAngleDelta;
					GLfloat zValue = sliceRadius*sin(vAngle);
					GLfloat baseXValue = sliceRadius*cos(vAngle);
					GLfloat xValue = (centerRadius+baseXValue)*cos(hAngle);
					GLfloat yValue = (centerRadius+baseXValue)*sin(hAngle);
					points[i][j] = point4(xValue, yValue, zValue, 1.0);
				}
			}
			genQuad(color, points[0][0], points[0][1],points[1][1],points[1][0], idxSpot,
					pointArray, colorArray);
		}
	}
	return ObjRef(indexStart, *idxSpot);
}
