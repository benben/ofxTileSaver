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
#ifndef _OFX_TILE_SAVER_IMAGE
#define _OFX_TILE_SAVER_IMAGE


#include "ofMain.h"
#include "ofImage.h"


class ofxTileSaverImage : public ofImage {

public:

	ofxTileSaverImage(){};

	void init(int _rows, int _cols, int _colWidth, int _rowHeight){

		rows = _rows;
		columns = _cols;

		colWidth = _colWidth;
		rowHeight = _rowHeight;

		width = colWidth * rows;
		height = rowHeight * columns;

		bigImagePixels =  new unsigned char[width*height*3];
		allocate(width, height, OF_IMAGE_COLOR);

		bUseTexture = false;
	};


	void addPixels(int x_, int y_, unsigned char * screenPixels){

		int startX = x_ * colWidth;
		int startY = y_ * rowHeight;

		for(int x=0; x<colWidth; x++){
			for(int y=0; y<rowHeight; y++){

				long srcPos = ((y * colWidth) + x) * 3;
				long dstPos = (((y + startY) * width) + x + startX) * 3;

				bigImagePixels[dstPos] = screenPixels[srcPos];
				bigImagePixels[dstPos+1] = screenPixels[srcPos+1];
				bigImagePixels[dstPos+2] = screenPixels[srcPos+2];
			}
		}
	};

	void done(string fileName, bool _flipY){
		setFromPixels(bigImagePixels, width, height, OF_IMAGE_COLOR);
		if(_flipY)
		     flipY();
		saveImage(fileName);
		delete [] bigImagePixels;
        clear();
	};

	void flipY(){
        mirror(true, false);
	};



	int rows, columns;
	int colWidth, rowHeight;
	int width, height;

	unsigned char * bigImagePixels;

};

#endif

