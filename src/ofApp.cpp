#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

#ifdef DEBUG

#else
    ofSetDataPathRoot("../Resources/data");
#endif


    ofSetVerticalSync(true);
    // ofSetCircleResolution(80);


    float fullscreenWRatio = ofGetWidth() / 1920.0;
    float fullscreenHRatio = ofGetHeight() / 1080.0;



    ofBackground(54, 54, 54);

    setupGui();

    switchOn = false;

    for (int i = 0; i < 20; i++) {
        ofTrueTypeFont _f;
        _f.load( "verdana.ttf", (int)ofRandom(7, 136) );
        alphabetFont.push_back(_f);
        TextParticle _t = TextParticle( ofVec3f(ofGetWidth() * 0.44, ofGetWidth() * 0.427, 0.0), alphabetFont[i] );
        textParticles.push_back(_t);
        _t = TextParticle( ofVec3f(ofGetWidth() * 0.562, ofGetWidth() * 0.427, 0.0), alphabetFont[i] );
        textParticles.push_back(_t);
    }


    // palast_trans.load("palast_trans.png");
    palast_trans_all.load("palast_trans_all.png");
    palast_black_windows.load("palast_black_windows.png");
    palast_playzone.load("palast_playzone.png");
    playgroundImg.load("palast_playzone_all.png");


    int _stepP = 3;
    playgroundMeshPixel.setMode(OF_PRIMITIVE_POINTS);
    playgroundMeshPixel.enableColors();
    for (int w = 0; w < playgroundImg.getWidth(); w += _stepP) {
        for (int h = 0; h < playgroundImg.getHeight(); h += _stepP) {
            ofColor c = playgroundImg.getColor(w, h);
            if (c.a != 0) {
                ofVec3f pos(w * fullscreenWRatio, h * fullscreenHRatio, 0.0);
                playgroundMeshPixel.addVertex(pos);
                playgroundMeshPixel.addColor(c);
            }
        }
    }
    playgroundMeshPixelBuff = playgroundMeshPixel;


    playgroundMeshTri.setMode(OF_PRIMITIVE_TRIANGLES);
    // playgroundMeshTri.enableIndices();
    int _stepT = 20;
    for (int h = 0; h < playgroundImg.getHeight(); h += _stepT) {
        for (int w = 0; w < playgroundImg.getWidth(); w += _stepT) {
            ofColor c = playgroundImg.getColor(w, h);
            ofVec3f pos(w * fullscreenWRatio, h * fullscreenHRatio, 0.0);
            playgroundMeshTri.addVertex(pos);
            playgroundMeshTri.addColor(c);
        }
    }

    int _w = playgroundImg.getWidth() / _stepT;
    int _h = playgroundImg.getHeight() / _stepT;
    for (int i = 0; i < _w - 1; i += 1) {
        for (int j = 0; j < _h - 1; j += 1) {
            playgroundMeshTri.addIndex(i + j * _w);
            playgroundMeshTri.addIndex(i + 1 + j * _w);
            playgroundMeshTri.addIndex(i + j * _w + _w);

            playgroundMeshTri.addIndex(i + 1 + j * _w);
            playgroundMeshTri.addIndex(i + 1 + j * _w + _w);
            playgroundMeshTri.addIndex(i + j * _w + _w);
        }
    }
    playgroundMeshTriBuff = playgroundMeshTri;


    ofBuffer _buffer = ofBufferFromFile("DerBluePunktImAll.txt");
    if (_buffer.size()) {
        for (ofBuffer::Line it = _buffer.getLines().begin(), end = _buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            if (line.empty() == false) {
                seussLines.push_back(line);
            }
        }
    }

    textWords = getStringVector("DerBluePunktImAll.txt");


    midiOut.listOutPorts();
    midiPort = midiOut.getOutPortList();
    for (int i = 0; i < midiPort.size(); i++) {
        if (midiPort[i].at(0) == 'I' && midiPort[i].at(1) == 'A' && midiPort[i].at(2) == 'C') {
            selectMidiPort =  i;
        }
    }
    midiOut.openPort(0);
    selectMidiName = midiPort[selectMidiPort];



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


    verdana30.load("verdana.ttf", 30, true, true);


    plotHeight = 128;
    bufferSize = 512;
    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT);

    audioInput = new float[bufferSize];
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
vector<string> ofApp::getStringVector(string fileName) {

    vector<string> _result;

    ofBuffer _bufferBound = ofBufferFromFile(fileName);
    // if (_bufferBound.size()) {

    for (ofBuffer::Line it = _bufferBound.getLines().begin(), end = _bufferBound.getLines().end(); it != end; ++it) {
        string _s = *it;
        istringstream iss(_s);
        for (std::string _s; iss >> _s; ) {
            if (_s.at(0) != ',') {
                if (_s.at(_s.size() - 1) == '.' || _s.at(_s.size() - 1) == ',') {
                    _s.erase(_s.end() - 1);
                    _result.push_back(_s);
                } else {
                    _result.push_back(_s);
                }
            }
        }
    }

    // }

    return _result;

}




//--------------------------------------------------------------
bool shouldRemove(TextParticle &p) {
    if (p.position.y < 0 ) return true;
    else return false;
}



//--------------------------------------------------------------
void ofApp::update() {

    scaleVolChange();

    frameRate = ofToString(ofGetFrameRate(), 1);


    for (int i = 0; i < textParticles.size(); ++i) {
        textParticles[i].update();
    }

    ofRemove(textParticles, shouldRemove);

    if (lineMovingOnOff) {
        lineMoving += 7;
    }

    if (scaleVolThresholdOn(scaledVol) && switchOn == false) {
        switchOn = true;
        for (int i = 0; i < 10; i++) {
            if (ofRandom(1) < 0.5) {
                TextParticle _t = TextParticle( ofVec3f(ofGetWidth() * 0.44, ofGetWidth() * 0.427, 0.0), alphabetFont[i] );
                textParticles.push_back(_t);
            } else {
                TextParticle _t = TextParticle( ofVec3f(ofGetWidth() * 0.562, ofGetWidth() * 0.427, 0.0), alphabetFont[i] );
                textParticles.push_back(_t);
            }
        }
    } else {
        // switchOn = false;
    }

    if (!scaleVolThresholdOn(scaledVol)) {
        switchOn = false;
    }

    // cout << textParticles.size() << endl;

    // if (lineMoving > 282) {
    //     lineMovingOnOff = false;
    //     lineMoving = 0;

    //     for (int i = 0; i < 30; i++) {
    //         // alphabetFont.load( "verdana.ttf", (int)ofRandom(7, 24) );
    //         if (ofRandom(1) < 0.5) {
    //             TextParticle _t = TextParticle( ofVec3f(621, 556, 0.0), alphabetFont[i] );
    //             textParticles.push_back(_t);
    //         } else {
    //             TextParticle _t = TextParticle( ofVec3f(778, 556, 0.0), alphabetFont[i] );
    //             textParticles.push_back(_t);
    //         }
    //     }

    // }

    if (pixelRandomOn) {
        updatePlaygroundMeshPixelPos(scaledVol);
    }

    if (textureOn) {
        updatePlaygroundMeshTriPos(scaledVol);
    }

}





//--------------------------------------------------------------
void ofApp::updatePlaygroundMeshPixelPos(float audioVol) {

    ofPushStyle();

    float _randomRatio = ofMap(audioVol, 0, 1, 0, 400);
    glPointSize(ofMap(audioVol, 0, 1, 2, 30));
    if (audioVol > 0.1) {
        for (int i = 0; i < playgroundMeshPixel.getNumVertices(); i++) {
            ofVec3f _randomVec = ofVec3f( ofRandom(-_randomRatio, _randomRatio), ofRandom(-_randomRatio, _randomRatio), 0 );
            ofVec3f _updateVec = playgroundMeshPixel.getVertex(i) + _randomVec;
            playgroundMeshPixel.setVertex(i, _updateVec);
        }
    } else {
        for (int i = 0; i < playgroundMeshPixel.getNumVertices(); i++) {
            ofVec3f _resetVec = playgroundMeshPixelBuff.getVertex(i);
            playgroundMeshPixel.setVertex(i, _resetVec);
        }
    }

    ofPopStyle();

}



//--------------------------------------------------------------
void ofApp::updatePlaygroundMeshTriPos(float audioVol) {

    ofPushStyle();

    float _randomRatio = ofMap(audioVol, 0, 1, 0, 10);
    if (audioVol > 0.1) {
        for (int i = 0; i < playgroundMeshTri.getNumVertices(); i++) {
            ofVec3f _randomVec = ofVec3f( ofRandom(-_randomRatio, _randomRatio), ofRandom(-_randomRatio, _randomRatio), 0 );
            ofVec3f _updateVec = playgroundMeshTri.getVertex(i) + _randomVec;
            playgroundMeshTri.setVertex(i, _updateVec);
        }
    } else {
        for (int i = 0; i < playgroundMeshTri.getNumVertices(); i++) {
            ofVec3f _resetVec = playgroundMeshTriBuff.getVertex(i);
            playgroundMeshTri.setVertex(i, _resetVec);
        }
    }

    ofPopStyle();

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

    if (scaledVol > 0.1) {
        midiOut.sendControlChange(1, 20, int(ofMap(scaledVol, 0.1, 1, 0, 127)));
    }
    if (scaledBaseVol > 0.1) {
        midiOut.sendControlChange(1, 21, ofMap(scaledBaseVol, 0.1, 1, 0, 127));
    }
    if (scaledMiddleVol > 0.1) {
        midiOut.sendControlChange(1, 22, ofMap(scaledMiddleVol, 0.1, 1, 0, 127));
    }
    if (scaledHighVol > 0.1) {
        midiOut.sendControlChange(1, 23, ofMap(scaledHighVol, 0.1, 1, 0, 127));
    }


}




//--------------------------------------------------------------
void ofApp::draw() {

    ofPushStyle();
    ofSetColor(180);
    palast_playzone.draw(0, 0, ofGetWidth(), ofGetHeight());
    ofPopStyle();

    if (textOn) {
        for (int i = 0; i < textParticles.size(); ++i) {
            textParticles[i].draw();
        }
    }

    if (pixelRandomOn) {
        playgroundMeshPixel.drawFaces();
    }


    if (textureOn) {
        ofPushMatrix();
        ofPushStyle();
        // ofSetColor(255);
        playgroundMeshTri.drawFaces();

        // ofSetColor(0, 180);
        // ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        // playgroundMeshTri.drawWireframe();

        for (int i=0; i<playgroundMeshTri.getNumVertices()-1; i+=1) {
            ofDrawLine(playgroundMeshTri.getVertex(i), playgroundMeshTri.getVertex(i + 1));
        }

        ofPopStyle();
        ofPopMatrix();
    }




// palast_trans.draw(0, 0, ofGetWidth(), ofGetHeight());
    palast_trans_all.draw(0, 0, ofGetWidth(), ofGetHeight());

    ofPushStyle();
    ofSetColor(0);
    palast_black_windows.draw(0, 0, ofGetWidth(), ofGetHeight());
    ofPopStyle();


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


// testMidiSignal += 0.1;
// float _midiS = ofMap(int(testMidiSignal) % 127, 0, 127, 0, 7);
// midiOut.sendControlChange(1, 7, int(_midiS));
// ofDrawBitmapString(int(_midiS), 700, 20);

    gui.draw();

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

    if (_scaledVol > audioThresholdLevel) {
        return true;
    }

    return false;

}



//--------------------------------------------------------------
bool ofApp::scaleVolThresholdOff(float _scaledVol) {

    // static bool th = false;

    // if (_scaledVol < 0.3 && !th) {
    //     th = true;
    //     return true;
    // }

    // if (_scaledVol > audioThreshold) {
    //     th = false;
    // }

    return false;

}




//--------------------------------------------------------------
int ofApp::scaleVolCounter(float _scaledVol) {

    static int count = 0;
    static bool th = false;

    if (_scaledVol > audioThresholdLevel && th) {
        count += 1;
        th = false;
    }

    if (_scaledVol < audioThresholdLevel) {
        th = true;
    }

    return count;

}




//--------------------------------------------------------------
void ofApp::midiOutputInformation() {

    ofPushStyle();
    ofPushMatrix();

    ofTranslate(20, 190);

    string _v = ofToString(ofMap(scaledVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Main Volume / CH : 1 / Ctrl Nr : 20 / " + _v, 0, 0);



    string _vB = ofToString(ofMap(scaledBaseVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Base Volume / CH : 1 / Ctrl Nr : 21 / " + _vB, 0, 190);

    string _vM = ofToString(ofMap(scaledMiddleVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("Mid Volume  / CH : 1 / Ctrl Nr : 22 / " + _vM, 0, 210);

    string _vH = ofToString(ofMap(scaledHighVol, 0.0, 1, 0, 127), 0);
    ofDrawBitmapString("High Volume / CH : 1 / Ctrl Nr : 23 / " + _vH, 0, 230);



    ofDrawBitmapString("Select Midi Port: " + selectMidiName, 0, 260);

    for (int i = 0; i < midiPort.size(); i++) {
        ofDrawBitmapString("All Midi Port: " + midiPort[i], 0, i * 20 + 280);
    }


    ofPopMatrix();
    ofPopStyle();

}



//--------------------------------------------------------------
void ofApp::textView(int _index) {

    ofPushStyle();
    ofPushMatrix();

    ofTranslate(ofGetWidth() * 0.347, ofGetHeight() - 250);


    // ofDrawBitmapString("Audio Trigger Sequence Text", 0, 55);
    // verdana30.drawString(textWords[_index % textWords.size()], 0, 100);

    ofPopMatrix();
    ofPopStyle();

    ofPushStyle();
    // ofDrawBitmapString("/ Tex Count : " + ofToString(scaleVolCounter(scaledVol) % textWords.size(), 0), 100, 0);
    ofPopStyle();


    // for (int i = 0; i < seussLines.size(); i++) {
    //     string typedLine = seussLines[i];
    //     ofSetColor(255);
    //     ofDrawBitmapString(typedLine, 10, i * 20 + 600);
    // }


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
void ofApp::drawEqPlot(float* array, int length, float scale, float offset) {

    ofPushMatrix();
    ofPushStyle();

    ofTranslate( 20, 220 );

    // glTranslatef(fft->getBinSize(), 0, 0);
    // ofDrawBitmapString("EQd FFT Output", 0, 0);

    ofNoFill();
    ofDrawRectangle(0, 0, length, plotHeight);

    // glPushMatrix();
    ofTranslate(0, plotHeight / 2 + offset, 0);
    ofBeginShape();
    for (int i = 0; i < length; i++) {
        ofVertex(i, array[i] * scale);
    }
    ofEndShape();
    // glPopMatrix();

    ofTranslate( length + 10, 0 );

    ofDrawRectangle(0, plotHeight, 10, -scaledBaseVol * plotHeight);
    ofDrawRectangle(15, plotHeight, 10, -scaledMiddleVol * plotHeight);
    ofDrawRectangle(2 * 15, plotHeight, 10, -scaledHighVol * plotHeight);

    ofPopStyle();
    ofPopMatrix();
}



//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input) {

    float curVol = 0.0;

    int numCounted = 0;

    for (size_t i = 0; i < input.getNumFrames(); i++) {
        left[i]     = input[i * 2] * volumeInput;
        right[i]    = input[i * 2 + 1] * volumeInput;

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
    smoothedBaseVol += 0.07 * getSmoothedVol(eqOutput, 0, 4);

    smoothedMiddleVol *= 0.93;
    smoothedMiddleVol += 0.07 * getSmoothedVol(eqOutput, 4, 10);

    smoothedHighVol *= 0.93;
    smoothedHighVol += 0.07 * getSmoothedVol(eqOutput, 10, 40);




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
    } else if ( key == 'e' ) {
        soundStream.stop();
    } else if (key == ' ') {
        lineMovingOnOff = true;
        lineColor.setHsb( ofRandom(255), 255, 255 );
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

    // float volume = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 127);
    // midiOut.sendControlChange(1, 7, volume);

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



//--------------------------------------------------------------
void ofApp::setupGui() {

    gui.setup();
    gui.setPosition(ofGetWidth() - 250, 10);

    gui.add(frameRate.setup("FPS", " "));
    gui.add(volumeInput.setup("Input Gain", 0.75, 0, 1.0));
    gui.add(audioThresholdLevel.setup("Audio Threshold", 0.5, 0.01, 1.0));

    gui.add(textOn.setup("Text OnOff", false));
    gui.add(pixelRandomOn.setup("Pixel OnOff", false));
    gui.add(textureOn.setup("Texture OnOff", false));

}