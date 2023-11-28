//
// ofxSkyBox.cpp
//
// Created by kalwalt alias Walter Perdan on 27/12/11
// Copyright 2011 http://www.kalwaltart.it/ All rights reserved.
//

#ifndef	SKY_BOX_H
#define SKY_BOX_H

#include "ofMain.h"
#include "ofxCubeMap.h"

class ofxSkyBox
{

public:

ofxSkyBox();

    void load();
    void loadShaders();
	void draw();
    void Draw_Skybox(float x, float y, float z, float width, float height, float length);

    ofxCubeMap cubeMap{};
	ofShader cubeshader{};

    // Shader source
    std::string skyboxfrag="uniform samplerCube EnvMap;\n"
    "varying vec3  texcoord;\n"
    "void main (void)\n"
    "{\n"
    "    vec3 envColor = vec3(textureCube(EnvMap, texcoord));\n"
    "    gl_FragColor = vec4 (envColor, 1.0);\n"
    "}\n";

    std::string skyboxvert="varying vec3 texcoord;\n"
    "void main (void)\n"
    "{\n"
    "    vec4 texcoord0 = gl_ModelViewMatrix*gl_Vertex;\n"
    "    texcoord = normalize(gl_Vertex.xyz);\n"
    "    gl_Position = ftransform();\n"
    "}\n";

};

#endif
