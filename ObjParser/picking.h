/*
 *  picking.h
 *  picking
 *
 *  Created by vegdahl on 9/10/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __PICKING_H__
#define __PICKING_H__

typedef void pickCallbackFunction(int code);

void showPickColors(bool b);

void setGpuPickColorId(int n);

void startPicking(pickCallbackFunction* fcn, int x, int y);

void endPicking(void);

bool inPickingMode(void);

void clearPickId(void) ;

void setPickId(int n);

#endif
