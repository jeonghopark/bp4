#pragma once

#include "ofMain.h"

#include "vv_extrudeFont.h"


class TextParticle : public ofBaseApp {

public:

    TextParticle(ofVec3f _v, ofTrueTypeFont _f);

    void update();
    void draw();

    ofVec3f position;
    ofVec3f speed;
    string alphabetString;

    int stringIndex;
    int size;

    ofColor c;

    ofTrueTypeFont alphabet;

    int rotationZ;
    float rotationSpeed;

    vector<ofVboMesh> font3d;


};