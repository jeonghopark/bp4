#pragma once

#include "ofMain.h"

#include "ofxMidi.h"


class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void audioIn(ofSoundBuffer & input);

	vector <float> left;
	vector <float> right;
	vector <float> volHistory;

	int bufferCounter;
	int drawCounter;

	float smoothedVol;
	float scaledVol;

	ofSoundStream soundStream;

	string sampleText;
	vector<string> sampleTextVector;
	void information();	

	ofTrueTypeFont verdana30;

	int scaleVolCounter(float _scaledVol);
	bool scaleVolThresholdOn(float _scaledVol);
	bool scaleVolThresholdOff(float _scaledVol);
	void stringView(int _index);

	bool noteOff();

	ofxMidiOut midiOut;
	int channel;
	
	unsigned int currentPgm;
	int note, velocity;
	int pan, bend, touch, polytouch;

	int oldNote;


};
