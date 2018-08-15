#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);

    soundStream.printDeviceList();

    int bufferSize = 512;

    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    // volHistory.assign(250, 0.0);

    bufferCounter = 0;
    drawCounter = 0;
    smoothedVol = 0.0;
    scaledVol = 0.0;
    audioThreshold = 0.7;

    smoothedBaseVol = 0.0;
    scaledBaseVol = 0.0;


    midiOut.listOutPorts();
    midiOut.openPort("IAC Driver Bus 1");


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


    plotHeight = 128;
    bufferSize = 512;
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT);

    audioInput = new float[bufferSize];
    cout << fft->getBinSize() << endl;
    fftOutput = new float[fft->getBinSize()];
    eqFunction = new float[fft->getBinSize()];
    eqOutput = new float[fft->getBinSize()];
    ifftOutput = new float[bufferSize];

    // audioInput.assign(bufferSize, 0.0);
    // fftOutput.assign(fft->getBinSize(), 0.0);
    // eqFunction.assign(fft->getBinSize(), 0.0);
    // eqOutput.assign(fft->getBinSize(), 0.0);
    // ifftOutput.assign(bufferSize, 0.0);


    for (int i = 0; i < fft->getBinSize(); i++) {
        eqFunction[i] = (float) (fft->getBinSize() - i) / (float) fft->getBinSize();
    }

    appWidth = ofGetWidth();
    appHeight = ofGetHeight();



}






//--------------------------------------------------------------
void ofApp::update() {

    scaleVolChange();


}



//--------------------------------------------------------------
vector<float> & ofApp::volHistoryGenerator(float _h) {

    static vector<float> _fV;

    _fV.push_back( _h );

    if ( _fV.size() >= 250 ) {
        _fV.erase(_fV.begin(), _fV.begin() + 1);
    }

    return _fV;

}


//--------------------------------------------------------------
void ofApp::scaleVolChange() {

    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    scaledBaseVol = ofMap(smoothedBaseVol, 0.0, 0.17, 0.0, 1.0, true);
    scaledMiddleVol = ofMap(smoothedMiddleVol, 0.0, 0.17, 0.0, 1.0, true);
    scaledHighVol = ofMap(smoothedHighVol, 0.0, 0.17, 0.0, 1.0, true);

}




//--------------------------------------------------------------
void ofApp::midiOutScaleChange() {

    if (scaledVol > 0.2) {
        midiOut.sendControlChange(1, 20, ofMap(scaledVol, 0.0, 1, 0, 127));
    }
    if (scaledBaseVol > 0.2) {
        midiOut.sendControlChange(1, 21, ofMap(scaledBaseVol, 0.0, 1, 0, 127));
    }
    if (scaledMiddleVol > 0.2) {
        midiOut.sendControlChange(1, 22, ofMap(scaledMiddleVol, 0.0, 1, 0, 127));
    }
    if (scaledHighVol > 0.2) {
        midiOut.sendControlChange(1, 23, ofMap(scaledHighVol, 0.0, 1, 0, 127));
    }


}

//--------------------------------------------------------------
void ofApp::draw() {

    audioInputInfo(scaledVol, volHistoryGenerator(scaledVol));


    textView(scaleVolCounter(scaledVol));


    midiOutScaleChange();


    midiOutputInformation();


    drawEqPlot(eqOutput, fft->getBinSize(), -plotHeight, plotHeight / 2);


    // if (scaleVolThresholdOn(scaledVol)) {
    //  oldNote = scaleVolCounter(scaledVol);
    //  midiOut.sendNoteOn(1, scaleVolCounter(scaledVol) + 64,  100);
    // }
    // if (scaleVolThresholdOff(scaledVol)) {
    //  midiOut.sendNoteOff(1, oldNote + 64,  100);
    // }

}




//--------------------------------------------------------------
void ofApp::drawEqPlot(float* array, int length, float scale, float offset) {

    ofPushMatrix();

    ofTranslate( 20, 200 );

    // glTranslatef(fft->getBinSize(), 0, 0);
    // ofDrawBitmapString("EQd FFT Output", 0, 0);

    ofNoFill();
    ofDrawRectangle(0, 0, length, plotHeight);
    glPushMatrix();
    glTranslatef(0, plotHeight / 2 + offset, 0);
    ofBeginShape();
    for (int i = 0; i < length; i++) {
        ofVertex(i, array[i] * scale);
    }
    ofEndShape();
    glPopMatrix();

    ofTranslate( length + 10, 0 );

    ofDrawRectangle(0, plotHeight, 10, -scaledBaseVol * plotHeight);
    ofDrawRectangle(15, plotHeight, 10, -scaledMiddleVol * plotHeight);
    ofDrawRectangle(2 * 15, plotHeight, 10, -scaledHighVol * plotHeight);

    ofPopMatrix();

}



//--------------------------------------------------------------
bool ofApp::noteOff() {

    static int timer = 0;

    timer++;
    cout << timer << endl;
    if (timer > 120) {
        return true;
    }

    return false;

}



//--------------------------------------------------------------
bool ofApp::scaleVolThresholdOn(float _scaledVol) {

    static bool th = false;

    if (_scaledVol > audioThreshold && !th) {
        th = true;
        return true;
    }

    if (_scaledVol < audioThreshold) {
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

    if (_scaledVol > audioThreshold) {
        th = false;
    }

    return false;

}




//--------------------------------------------------------------
int ofApp::scaleVolCounter(float _scaledVol) {

    static int count = 0;
    static bool th = false;

    if (_scaledVol > audioThreshold && th) {
        count += 1;
        th = false;
    }

    if (_scaledVol < audioThreshold) {
        th = true;
    }

    return count;

}




//--------------------------------------------------------------
void ofApp::midiOutputInformation() {

    ofPushStyle();
    ofPushMatrix();

    ofTranslate(500, 170);

    string _v = ofToString(ofMap(scaledVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Main Volume / MIDI CH : 1 / Ctrl Nr : 20 / " + _v, 0, 0);

    ofTranslate(0, 160);

    string _vB = ofToString(ofMap(scaledBaseVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Base Volume / MIDI CH : 1 / Ctrl Nr : 21 / " + _vB, 0, -40);

    string _vM = ofToString(ofMap(scaledMiddleVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Mid Volume  / MIDI CH : 1 / Ctrl Nr : 22 / " + _vM, 0, -20);

    string _vH = ofToString(ofMap(scaledHighVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("High Volume / MIDI CH : 1 / Ctrl Nr : 23 / " + _vH, 0, 0);

    ofPopMatrix();
    ofPopStyle();

}



//--------------------------------------------------------------
void ofApp::textView(int _index) {

    ofPushStyle();
    ofPushMatrix();

    ofTranslate(500, ofGetHeight() - 150);

    ofDrawBitmapString("Sample Text", 0, 0);
    ofDrawBitmapString(sampleText, 0, 20);

    ofDrawBitmapString("Audio Trigger Sequence Text", 0, 55);
    verdana30.drawString(sampleTextVector[_index % sampleTextVector.size()], 0, 100);

    ofPopMatrix();
    ofPopStyle();

    ofPushStyle();
    ofDrawBitmapString("/ Tex Count : " + ofToString(scaleVolCounter(scaledVol) % sampleTextVector.size(), 0), 100, 0);
    ofPopStyle();

}




//--------------------------------------------------------------
void ofApp:: audioInputInfo(float _h, vector<float> & _v) {

    ofPushStyle();
    ofPushMatrix();
    ofTranslate(20, 20, 0);

    ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(_h * 100.0, 0), 0, 18);

    ofSetColor(255);
    ofFill();
    ofDrawRectangle(260, 150, 20, -_h * 70);
    ofNoFill();
    ofDrawRectangle(260, 150, 20, -70);
    ofSetColor(255, 0, 0);
    ofDrawLine(260, 150 - 70 * audioThreshold, 280, 150 - 70 * audioThreshold);


    ofFill();
    ofSetColor(255);
    ofBeginShape();
    for (unsigned int i = 0; i < _v.size(); i++) {
        if ( i == 0 ) ofVertex(i, 150);
        ofVertex(i, 150 - _v[i] * 70);

        if ( i == _v.size() - 1 ) ofVertex(i, 150);
    }
    ofEndShape(false);


    ofDrawBitmapString(ofToString(ofGetFrameRate(), 1), 0, 0);

    ofPopMatrix();
    ofPopStyle();

}




//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input) {

    float curVol = 0.0;

    int numCounted = 0;

    for (size_t i = 0; i < input.getNumFrames(); i++) {
        left[i]     = input[i * 2] * 0.5;
        right[i]    = input[i * 2 + 1] * 0.5;

        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted += 2;
    }

    curVol /= (float)numCounted;
    curVol = sqrt( curVol );
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;

    bufferCounter++;

    // memcpy(&audioInput, &left, sizeof(float) * bufferSize);
    audioInput = &left[0];
    // audioInput = left;
    fft->setSignal(audioInput);
    memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());

    for (int i = 0; i < fft->getBinSize(); i++) {
        eqOutput[i] = fftOutput[i] * eqFunction[i];
    }



    smoothedBaseVol *= 0.93;
    smoothedBaseVol += 0.07 * getSmoothedVol(eqOutput, 0, 10);

    smoothedMiddleVol *= 0.93;
    smoothedMiddleVol += 0.07 * getSmoothedVol(eqOutput, 20, 80);

    smoothedHighVol *= 0.93;
    smoothedHighVol += 0.07 * getSmoothedVol(eqOutput, 80, 257);


    fft->setPolar(eqOutput, fft->getPhase());

    fft->clampSignal();
    memcpy(ifftOutput, fft->getSignal(), sizeof(float) * fft->getSignalSize());

}




//--------------------------------------------------------------
float ofApp::getSmoothedVol(float * _in, int _s, int _e) {

    float _allMiddleVol = 0;
    int _numMiddleCounted = 0;
    for (int i = _s; i < _e; i++) {
        _allMiddleVol += _in[i];
        _numMiddleCounted += 1;
    }
    _allMiddleVol /= (float)_numMiddleCounted;
    _allMiddleVol = sqrt( _allMiddleVol );
    return _allMiddleVol;

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

