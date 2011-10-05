#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	
	//numSteps (the outputImage will be n-Times bigger than the windowsize), border(most times zero), save as one big image?
	//-> depends on your RAM, if you want to go bigger than 12k*12k you should propably set this to false and stich the tiles
	// offline.
	saver.init(10, 20, true);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	//use this is you have custom camera setup:
	
	//saver.setCameraData(pos, eye, up);
	//saver.setPerspectiveData(fov, aspect, zNear, zFar);
	
	saver.begin();
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(ofGetWidth()/2.0, ofGetHeight()/2.0, 0);
	glRotatef(90.0f, 1,0,0);
	glutSolidCone(100, 200, 40,40);
	glPopMatrix();
	saver.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	//fileName (supports all image types ofImage does), flipY?
	if(key=='f') saver.finish("high.png", false);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

