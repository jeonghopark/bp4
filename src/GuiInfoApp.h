#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class GuiInfoApp: public ofBaseApp {
public:
	void setup();
	void update();
	void draw();


    ofxPanel gui;
    ofxLabel frameRate;
    ofxFloatSlider volumeInput;
    ofxFloatSlider audioThresholdLevel;
    ofxToggle pixelRandomOn;
    ofxToggle textOn;
    ofxToggle textureOn;


    float scaledVol;
    float smoothedBaseVol;
    float scaledBaseVol;
    float smoothedMiddleVol;
    float scaledMiddleVol;
    float smoothedHighVol;
    float scaledHighVol;


    void audioInputInfo(float _h, vector<float> & _v);
    vector<float> & volHistoryGenerator(float _h);


    float * eqOutput;
    float plotHeight;
    int fftBinSize;
    void drawEqPlot(float* array, int length, float scale, float offset);


    vector<string> midiPort;
    int selectMidiPort;
    string selectMidiName;
    void midiOutputInformation();

};

