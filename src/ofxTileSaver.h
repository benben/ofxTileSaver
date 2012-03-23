/*
 OpenGL TileSaving C++ Library for OpenFrameworks
 http://www.openframeworks.cc

 Copyright (c) 2009 by Matthias Dörfelt & Jeremy Rotsztain

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _OFX_TILE_SAVER
#define _OFX_TILE_SAVER

#include "ofMain.h"
#include "ofxTileSaverImage.h"


struct ofxFrustumData{
	void set(float _left, float _right, float _bottom, float _top, float _near, float _far){
		left = _left;
		right = _right;
		bottom = _bottom;
		top = _top;
		near = _near;
		far = _far;
	}
	float left, right, bottom, top, near, far;
};

class ofxTileSaver{

public:

	ofxTileSaver(){}
	~ofxTileSaver(){
		cleanUp();
	}

	//Setup all settings needed
	void init(int _steps, int _border, bool _bBigImage){
		steps = _steps;
		currentRow = 0;
		currentCol = 0;
		border = _border;
		float width = ofGetWidth();
		float height = ofGetHeight();

		tileWidth = width;
		tileHeight = height;

		tileWidthNoBorder = width-border*2;
		tileHeightNoBorder = height-border*2;

		imageWidth = tileWidthNoBorder*steps;
		imageHeight = tileHeightNoBorder*steps;

		bGoTiling = false;
		bFlipY = false;
		bBigImage = _bBigImage;

		currentTile = 0;

		float halfFov, theTan, screenFov, aspect;
		screenFov 		= 60.0f;

		float eyeX 		= (float)width / 2.0;
		float eyeY 		= (float)height / 2.0;
		halfFov 		= PI * screenFov / 360.0;
		theTan 			= tanf(halfFov);
		float dist 		= eyeY / theTan;
		float nearDist 	= dist / 10.0;	// near / far clip plane
		float farDist 	= dist * 10.0;
		aspect 			= (float)width/(float)height;

		ofVec3f eyeHolder, posHolder, upHolder;
		posHolder.set(eyeX, eyeY, dist);
		eyeHolder.set(eyeX, eyeY, 0.0);
		upHolder.set(0,1,0);
		setPerspectiveData(screenFov, aspect, nearDist, farDist);
		setCameraData( posHolder, eyeHolder, upHolder);
	}

	//setup perspective and Camera, if done, safe Image
	void begin(){
		if(!bGoTiling)
			return;
	    if(currentRow == steps){

			if(bBigImage)
				final.done(fileName, bFlipY);
			setPerspective();
			positionCamera();
			bGoTiling = false;
			currentTile = 0;
			return;
		}

		setFrustum(currentCol, currentRow);
		positionCamera();
	}

	//grab FrameBuffer and ddd it to the Image or save the tile right away
	void end(){
		if(!bGoTiling)
			return;

		current.grabScreen(border, border, tileWidthNoBorder, tileHeightNoBorder);
		if(!bBigImage){
			current.saveImage(ofToString(currentTile)+".png");
		} else {
			final.addPixels(currentRow, currentCol, current.getPixels());
		}
		if(currentCol==steps-1){
			currentCol = 0;
			currentRow++;
		} else {
			currentCol++;
		}
		currentTile++;
	}

	//this starts the saving Process
	void finish(string _fileName, bool _bFlipY){
		if(bBigImage)
			final.init(steps, steps, tileWidthNoBorder, tileHeightNoBorder);
		bGoTiling = true;
		currentCol = 0;
		currentRow = 0;
		fileName = _fileName;
		bFlipY = _bFlipY;
	}

	//Camera Stuff
	void setPerspectiveData(float _fov, float _aspect, float _nearZ, float _farZ){
		fov = _fov;
		aspect = _aspect;
		nearZ = _nearZ;
		farZ = _farZ;
		float Top = nearZ * tan(fov * PI / 360.0);
     	float Bottom = -Top;

	    float Left = Bottom * aspect;
	    float Right = Top * aspect;
	    float Near = nearZ;
		float Far = farZ;

		curFrustum.set(Left, Right, Bottom, Top, Near, Far);
	}

	void setCameraData(ofVec3f _pos, ofVec3f _eye, ofVec3f _up){
		pos = _pos;
		eye = _eye;
		up = _up;
	}

	void setPerspective(){
		glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(fov, aspect, nearZ, farZ);
	}

	void positionCamera(){
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(pos.x, pos.y, pos.z,
				  eye.x, eye.y, eye.z,
				  up.x, up.y, up.z);


		//glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
		//glTranslatef(0, -height, 0);
	}

	//this is the important part, the Frustum gets setup correctly so we can tile perfectly
	void setFrustum(int _row, int _col){
		float Top = nearZ * tan(fov * PI / 360.0);
     	float Bottom = -Top;

	    float Left = Bottom * aspect;
	    float Right = Top * aspect;
	    float Near = nearZ;
		float Far = farZ;

	    float left   = Left + (((Right - Left) * (_col * tileWidthNoBorder-border)) / imageWidth);
	    float right  = left + (((Right - Left) * tileWidth) / imageWidth);
	    float bottom = Bottom + (Top - Bottom) * ((steps-1-_row)* tileHeightNoBorder-border) / imageHeight;
	    float top    = bottom + (((Top - Bottom) * tileHeight) / imageHeight);

		glViewport(0, 0, tileWidth, tileHeight);

	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();

	    // this is strange but we need it
	    float diff = 0;

		curFrustum.set(left, right, bottom+diff, top+diff, Near, Far);

	    glFrustum(left, right, bottom+diff, top+diff, Near, Far);
	}

	ofxFrustumData getCurFrustum(){
		return curFrustum;
	}

	int getCurCol(){
		return currentCol;
	}

	int getCurRow(){
		return currentRow;
	}

	//free memory
	void cleanUp(){
		final.clear();
		current.clear();
	}

	float aspect, fov, nearZ, farZ, imageWidth, imageHeight, tileHeight, tileWidth, tileWidthNoBorder, tileHeightNoBorder;
	int steps, currentRow, currentCol, border, currentTile;

	ofxTileSaverImage final;
	ofImage current;

	ofxFrustumData curFrustum;

	string fileName;

	bool bGoTiling, bFlipY, bBigImage;
	ofVec3f pos, eye, up;
};

#endif
