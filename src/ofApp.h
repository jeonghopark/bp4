#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxFft.h"


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
	// vector <float> volHistory;

	int bufferCounter;
	int drawCounter;

	float smoothedVol;
	float scaledVol;

	float smoothedBaseVol;
	float scaledBaseVol;
	float smoothedMiddleVol;
	float scaledMiddleVol;
	float smoothedHighVol;
	float scaledHighVol;


	ofSoundStream soundStream;

	string sampleText;
	vector<string> sampleTextVector;
	void information(float _h, vector<float> & _v);	
	vector<float> & volHistoryGenerator(float _h);

	ofTrueTypeFont verdana30;

	int scaleVolCounter(float _scaledVol);
	bool scaleVolThresholdOn(float _scaledVol);
	bool scaleVolThresholdOff(float _scaledVol);
	void textView(int _index);

	bool noteOff();

	ofxMidiOut midiOut;
	int channel;
	
	unsigned int currentPgm;
	int note, velocity;
	int pan, bend, touch, polytouch;

	int oldNote;

	float audioThreshold;


	void midiOutputInformation();


	int plotHeight, bufferSize;

	ofxFft* fft;

	float* audioInput;
	float* fftOutput;
	float* eqFunction;
	float* eqOutput;
	float* ifftOutput;

	// vector<float> audioInput;
	// vector<float> fftOutput;
	// vector<float> eqFunction;
	// vector<float> eqOutput;
	// vector<float> ifftOutput;


	float appWidth;
	float appHeight;

	void plot(float* array, int length, float scale, float offset);

};
