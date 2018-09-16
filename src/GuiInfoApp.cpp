/*
 * GuiInfoApp.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: arturo
 */

#include "GuiInfoApp.h"

void GuiInfoApp::setup(){
	
    ofBackground(10);
	ofSetVerticalSync(false);

    gui.setup();
    gui.setPosition(10, 10);

    gui.add(frameRate.setup("FPS", " "));
    gui.add(volumeInput.setup("Input Gain", 0.75, 0, 1.0));
    gui.add(audioThresholdLevel.setup("Audio Threshold", 0.5, 0.01, 1.0));

    gui.add(textOn.setup("Text OnOff", true));
    gui.add(pixelRandomOn.setup("Pixel OnOff", false));
    gui.add(textureOn.setup("Texture OnOff", false));
    gui.add(windowsOn.setup("Windows OnOff", false));

}

//--------------------------------------------------------------
void GuiInfoApp::update(){

}

//--------------------------------------------------------------
void GuiInfoApp::draw(){

    gui.draw();

    ofTranslate( 20, 0 );
    ofTranslate( 0, gui.getHeight() + 20, 0);
    audioInputInfo(scaledVol, volHistoryGenerator(scaledVol));

    ofTranslate( 0, 200 );
    drawEqPlot(eqOutput, fftBinSize, -plotHeight, plotHeight / 2);

    ofTranslate( 0, 200 );
    midiOutputInformation();

}



//--------------------------------------------------------------
void GuiInfoApp:: audioInputInfo(float _h, vector<float> & _v) {

    ofPushStyle();
    ofPushMatrix();

    ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(_h * 100.0, 0), 0, 18);

    ofSetColor(255);
    ofFill();
    ofDrawRectangle(260, 150, 20, -_h * 70);
    ofNoFill();
    ofDrawRectangle(260, 150, 20, -70);
    ofSetColor(255, 0, 0);
    ofDrawLine(260, 150 - 70 * audioThresholdLevel, 280, 150 - 70 * audioThresholdLevel);


    ofFill();
    ofSetColor(255);
    ofBeginShape();
    for (unsigned int i = 0; i < _v.size(); i++) {
        if ( i == 0 ) ofVertex(i, 150);
        ofVertex(i, 150 - _v[i] * 70);

        if ( i == _v.size() - 1 ) ofVertex(i, 150);
    }
    ofEndShape(false);


    // ofDrawBitmapString(ofToString(ofGetFrameRate(), 1), 0, 0);

    ofPopMatrix();
    ofPopStyle();

}


//--------------------------------------------------------------
vector<float> & GuiInfoApp::volHistoryGenerator(float _h) {

    static vector<float> _fV;

    _fV.push_back( _h );

    if ( _fV.size() >= 250 ) {
        _fV.erase(_fV.begin(), _fV.begin() + 1);
    }

    return _fV;

}


//--------------------------------------------------------------
void GuiInfoApp::drawEqPlot(float* array, int length, float scale, float offset) {

    ofPushMatrix();
    ofPushStyle();


    // glTranslatef(fft->getBinSize(), 0, 0);
    // ofDrawBitmapString("EQd FFT Output", 0, 0);

    ofNoFill();
    ofDrawRectangle(0, 0, length, plotHeight);

    // glPushMatrix();
    ofFill();
    ofTranslate(0, plotHeight / 2 + offset, 0);
    ofBeginShape();
    ofVertex(0, 0);
    for (int i = 0; i < length; i++) {
        ofVertex(i, array[i] * scale);
    }
    ofVertex(length, 0);
    ofEndShape();
    // glPopMatrix();

    ofTranslate( length + 10, 0 );

    ofDrawRectangle(0, 0, 10, -scaledBaseVol * plotHeight);
    ofDrawRectangle(15, 0, 10, -scaledMiddleVol * plotHeight);
    ofDrawRectangle(2 * 15, 0, 10, -scaledHighVol * plotHeight);

    ofPopStyle();
    ofPopMatrix();
}




//--------------------------------------------------------------
void GuiInfoApp::midiOutputInformation() {

    ofPushStyle();
    ofPushMatrix();

    string _v = ofToString(ofMap(scaledVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Main Volume / CH : 1 / Ctrl Nr : 20 / " + _v, 0, 0);

    string _vB = ofToString(ofMap(scaledBaseVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Base Volume / CH : 1 / Ctrl Nr : 21 / " + _vB, 0, 20);

    string _vM = ofToString(ofMap(scaledMiddleVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Mid Volume  / CH : 1 / Ctrl Nr : 22 / " + _vM, 0, 40);

    string _vH = ofToString(ofMap(scaledHighVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("High Volume / CH : 1 / Ctrl Nr : 23 / " + _vH, 0, 60);

    ofDrawBitmapString("Select Midi Port: " + selectMidiName, 0, 80);

    for (int i = 0; i < midiPort.size(); i++) {
        ofDrawBitmapString("All Midi Port: " + midiPort[i], 0, i * 20 + 100);
    }


    ofPopMatrix();
    ofPopStyle();

}