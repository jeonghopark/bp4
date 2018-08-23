#pragma once

#include "ofMain.h"


class TextParticle{

public:

    TextParticle(ofVec3f _v, ofTrueTypeFont _t);

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

};