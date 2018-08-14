#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);

	soundStream.printDeviceList();

	int bufferSize = 256;

	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	// volHistory.assign(250, 0.0);

	bufferCounter = 0;
	drawCounter	= 0;
	smoothedVol = 0.0;
	scaledVol = 0.0;


	ofSoundStreamSettings settings;


	auto devices = soundStream.getMatchingDevices("default");
	if (!devices.empty()) {
		settings.setInDevice(devices[0]);
	}

	settings.setInListener(this);
	settings.sampleRate = 44100;
	settings.numOutputChannels = 0;
	settings.numInputChannels = 2;
	settings.bufferSize = bufferSize;
	soundStream.setup(settings);


	sampleText = "Ash nazg durbatulûk, ash nazg gimbatul, Ash nazg thrakatulûk agh burzum-ishi krimpatul.";

	sampleTextVector = ofSplitString( sampleText, " ");

	verdana30.load("verdana.ttf", 30, true, true);

	midiOut.listOutPorts();
	midiOut.openPort(0);

}






//--------------------------------------------------------------
void ofApp::update() {

	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	volHistory.push_back( scaledVol );

	if ( volHistory.size() >= 250 ) {
		volHistory.erase(volHistory.begin(), volHistory.begin() + 1);
	}

}





//--------------------------------------------------------------
void ofApp::draw() {

	information();

	stringView(scaleVolCounter(scaledVol));

	if (scaledVol > 0.2) {
		midiOut.sendControlChange(1, 20, ofMap(scaledVol, 0.2, 1, 0, 127));
	}

	// if (scaleVolThresholdOn(scaledVol)) {
	// 	oldNote = scaleVolCounter(scaledVol);
	// 	midiOut.sendNoteOn(1, scaleVolCounter(scaledVol) + 64,  100);
	// }
	// if (scaleVolThresholdOff(scaledVol)) {
	// 	midiOut.sendNoteOff(1, oldNote + 64,  100);
	// }


}



//--------------------------------------------------------------
bool ofApp::noteOff() {

	static int timer = 0;

	timer++;
	cout << timer << endl;
	if (timer > 120) {
		return true;
	}


}



//--------------------------------------------------------------
bool ofApp::scaleVolThresholdOn(float _scaledVol) {

	static bool th = false;

	if (_scaledVol > 0.8 && !th) {
		th = true;
		return true;
	}

	if (_scaledVol < 0.8) {
		th = false;
	}

	return false;

}



//--------------------------------------------------------------
bool ofApp::scaleVolThresholdOff(float _scaledVol) {

	static bool th = false;

	if (_scaledVol < 0.3 && !th) {
		th = true;
		return true;
	}

	if (_scaledVol > 0.8) {
		th = false;
	}

	return false;

}




//--------------------------------------------------------------
int ofApp::scaleVolCounter(float _scaledVol) {

	static int count = 0;
	static bool th = false;

	if (_scaledVol > 0.8 && th) {
		count += 1;
		th = false;
	}

	if (_scaledVol < 0.8) {
		th = true;
	}

	return count;

}


//--------------------------------------------------------------
void ofApp::stringView(int _index) {

	ofPushStyle();
	ofPushMatrix();

	ofTranslate(500, 50);

	ofDrawBitmapString("Sample Text", 0, 0);

	ofDrawBitmapString(sampleText, 0, 20);

	ofDrawBitmapString("Audio Trigger Sequence Text", 0, 55);

	verdana30.drawString(sampleTextVector[_index % sampleTextVector.size()], 0, 100);

	ofPushStyle();
	ofDrawBitmapString("/ Tex Count : " + ofToString(scaleVolCounter(scaledVol) % sampleTextVector.size(), 0), 100, 0);
	ofPopStyle();


	ofPopMatrix();
	ofPopStyle();

}


//--------------------------------------------------------------
void ofApp:: information() {

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(20, 20, 0);

	ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 0, 18);

	ofSetColor(255);
	ofFill();
	ofDrawRectangle(260, 150, 20, -scaledVol * 70.0f);

	ofBeginShape();
	for (unsigned int i = 0; i < volHistory.size(); i++) {
		if ( i == 0 ) ofVertex(i, 150);
		ofVertex(i, 150 - volHistory[i] * 70);

		if ( i == volHistory.size() - 1 ) ofVertex(i, 150);
	}
	ofEndShape(false);


	ofPopMatrix();
	ofPopStyle();

}




//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input) {

	float curVol = 0.0;

	int numCounted = 0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {
		left[i]		= input[i * 2] * 0.5;
		right[i]	= input[i * 2 + 1] * 0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted += 2;
	}

	curVol /= (float)numCounted;

	curVol = sqrt( curVol );

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

	bufferCounter++;

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key) {
	if ( key == 's' ) {
		soundStream.start();
	}

	if ( key == 'e' ) {
		soundStream.stop();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

