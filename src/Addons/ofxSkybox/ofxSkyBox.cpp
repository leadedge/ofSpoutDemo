//
// ofxSkyBox.cpp
//
// Created by kalwalt alias Walter Perdan on 27/12/11
// Copyright 2011 http://www.kalwaltart.it/ All rights reserved.
//


// LJ revisions
// C++ code analysis warning 26812 for VS2022
// Class enums have cascading effects.
// Many other libraries do not use them at this time.
#pragma warning(disable : 26812)
// To avoid Openframeworks warning
// Boost.Config is older than your compiler version
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
//
// 23.11.23 ofxSkyBox  - add loadShaders()
// 25.11.23 add shader source to ofxSkybox.h and load shaders from string
//

#include "ofxSkyBox.h"

ofxSkyBox::ofxSkyBox(){

}

void ofxSkyBox::load(){

    cubeshader.load("build/shaders/skybox");

	// Not used
	cubeMap.loadImages(
		"build/images/spacescape_purple_nebula_1024RT.jpg",  // Right - positive x
		"build/images/spacescape_purple_nebula_1024UP.jpg",  // Top   - positive y
		"build/images/spacescape_purple_nebula_1024FT.jpg",  // Front - positive z
		"build/images/spacescape_purple_nebula_1024LF.jpg",  // Left  - negative x
		"build/images/spacescape_purple_nebula_1024DN.jpg",  // Down  - negative y
		"build/images/spacescape_purple_nebula_1024BK.jpg"); // Back  - negative z

}

void ofxSkyBox::loadShaders() {
	cubeshader.setupShaderFromSource(GL_VERTEX_SHADER, skyboxvert.c_str());
	cubeshader.setupShaderFromSource(GL_FRAGMENT_SHADER, skyboxfrag.c_str());
	cubeshader.linkProgram();
	// cubeshader.load("build/shaders/skybox");
}

void ofxSkyBox::draw() {

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	
	cubeshader.begin();
	//glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	cubeMap.bind();

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    cubeshader.setUniform1i("EnvMap", 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

	Draw_Skybox(0, 0, 0, 100, 100, 100);

	cubeshader.end();

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glDisable(GL_DEPTH_TEST);
   	glPopMatrix();


}

void ofxSkyBox::Draw_Skybox(float x, float y, float z, float width, float height, float length){

    // Center the Skybox around the given x,y,z position
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Draw Front side
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();

	// Draw Back side
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	// Draw Left side
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	// Draw Right side
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();

	// Draw Up side
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();

	// Draw Down side
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y,		z);
	glEnd();

}






