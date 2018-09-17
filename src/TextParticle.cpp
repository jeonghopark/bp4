#include "TextParticle.h"


TextParticle::TextParticle(ofVec3f _v, ofTrueTypeFont _f) {

    position = _v;


    float _xRandom;
    if (ofRandom(1) < 0.5) {
        _xRandom = ofRandom(-0.25, -0.75);
    } else {
        _xRandom = ofRandom(0.25, 0.75);
    }

    float _yRandom;
    if (ofRandom(1) < 0.5) {
        _yRandom = ofRandom(-0.15, -2);
    } else {
        _yRandom = ofRandom(0.15, 0.75);
    }


    speed = ofVec3f(_xRandom, _yRandom, 0);

    alphabetString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    stringIndex = (int)ofRandom(26);

    // size = (int)ofRandom(7, 46);
    c.setHsb( ofRandom(255), 255, 255 );

    alphabet = _f;
    rotationZ = 0;
    rotationSpeed = ofRandom(-4, 4);

    font3d = extrude_mesh_from_text(ofToString(alphabetString[stringIndex]), alphabet, 30);

}


//------------------------------------------
void TextParticle::update() {

    position += speed;
    rotationZ += rotationSpeed;
}


//------------------------------------------
void TextParticle::draw() {

    ofPushMatrix();
    ofPushStyle();
    ofSetColor(c);
    ofTranslate(position.x, position.y);
    ofRotateZDeg(rotationZ);

    // char _s = alphabetString[stringIndex];
    // alphabet.drawString(ofToString(_s), 0, 0);

    for (int m = 0; m < font3d.size(); m++) {
        font3d.at(m).draw();
    }


    ofPopStyle();
    ofPopMatrix();

}