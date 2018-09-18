#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {

#ifdef DEBUG
#else
    ofSetDataPathRoot("../Resources/data");
#endif

    ofSetVerticalSync(true);


    cout << ofGetScreenWidth() << endl;

    if (ofGetScreenWidth() == 5120) {
        fullScreenRatio = 1;
        windowSize = ofVec2f(ofGetWidth(), ofGetHeight());
    } else {
        fullScreenRatio = 0.4 * 2; // 2 / 5
        windowSize = ofVec2f(ofGetWidth() * fullScreenRatio, ofGetHeight() * fullScreenRatio);
        ofSetWindowShape(windowSize.x, windowSize.y);
    }



    ofBackground(54);


    switchOn = false;

    textParticleLeftPos = ofVec3f(windowSize.x * 0.394, windowSize.y * (1 - 0.784), 0.0);
    textParticleRightPos = ofVec3f(windowSize.x * 0.611, windowSize.y * (1 - 0.784), 0.0);

    for (int i = 0; i < 10; i++) {
        ofTrueTypeFont _f;
        _f.load( "verdana.ttf", (int)ofRandom(7, 136), true, true, true );
        alphabetFont.push_back(_f);
        textParticles.push_back(TextParticle( textParticleLeftPos, _f ));
        textParticles.push_back(TextParticle( textParticleRightPos, _f ));
    }


    // palast_trans.load("palast_trans.png");
    palast_trans_all.load("palast_trans_all.png");
    palast_black_windows.load("palast_black_windows.png");
    palast_white_windows.load("palast_white_windows.png");
    palast_playzone.load("palast_playzone.png");
    playgroundImg.load("palast_playzone_all.png");


    int _stepP = 6;
    playgroundMeshPixel.setMode(OF_PRIMITIVE_POINTS);
    playgroundMeshPixel.enableColors();
    for (int w = 0; w < playgroundImg.getWidth(); w += _stepP) {
        for (int h = 0; h < playgroundImg.getHeight(); h += _stepP) {
            ofColor c = playgroundImg.getColor(w, h);
            if (c.a != 0) {
                ofVec3f pos(w * fullScreenRatio, windowSize.y - h * fullScreenRatio, 0.0);
                playgroundMeshPixel.addVertex(pos);
                playgroundMeshPixel.addColor(c);
            }
        }
    }
    playgroundMeshPixelBuff = playgroundMeshPixel;


    playgroundMeshTri.setMode(OF_PRIMITIVE_TRIANGLES);
    // playgroundMeshTri.enableIndices();
    int _stepT = 40;
    for (int h = 0; h < playgroundImg.getHeight(); h += _stepT) {
        for (int w = 0; w < playgroundImg.getWidth(); w += _stepT) {
            ofColor c = playgroundImg.getColor(w, h);
            ofVec3f pos(w * fullScreenRatio, windowSize.y - h * fullScreenRatio, 0.0);
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
                // seussLines.push_back(line);
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
    guiInfo->selectMidiName = selectMidiName;
    guiInfo->midiPort = midiPort;


    bufferCounter = 0;
    drawCounter = 0;
    smoothedVol = 0.0;
    scaledVol = 0.0;
    audioThreshold = 0.7;
    smoothedBaseVol = 0.0;
    scaledBaseVol = 0.0;


    setSoundStream(soundStream);


    // verdana30.load("verdana.ttf", 30, true, true);


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



    int _zLightDepth = 400;
    pointLightL.setDiffuseColor( ofColor(255, 255, 255));
    pointLightL.setSpecularColor( ofColor(0, 0, 255));
    pointLightL.setPosition(textParticleLeftPos.x, textParticleLeftPos.y, textParticleLeftPos.z + _zLightDepth);
    pointLightR.setDiffuseColor( ofColor(255, 255, 255));
    pointLightR.setSpecularColor( ofColor(0, 0, 255));
    pointLightR.setPosition(textParticleRightPos.x, textParticleRightPos.y, textParticleRightPos.z + _zLightDepth);

    material.setShininess( 64 );

    lightColor.setBrightness(180);
    lightColor.setSaturation(150);

    materialColor.setBrightness(250);
    materialColor.setSaturation(200);


    cam.setAutoDistance(false);
    cam.setPosition(ofVec3f(0, -ofGetHeight() * 0.5 + 150, 1330));
    // cam.setOrientation(ofVec3f(25, 0, 0));
    cam.setRelativeYAxis(true);
    cam.setLensOffset(ofVec2f(0, 0.8));
    // cam.lookAt(ofVec3f(0, -ofGetHeight() * 0.5, -700));


}





//--------------------------------------------------------------
void ofApp::setSoundStream(ofSoundStream & _s) {

    _s.printDeviceList();

    int _bufferSize = 512;
    left.assign(_bufferSize, 0.0);
    right.assign(_bufferSize, 0.0);
    // volHistory.assign(250, 0.0);

    ofSoundStreamSettings _settings;
    auto devices = _s.getMatchingDevices("default");
    if (!devices.empty()) {
        _settings.setInDevice(devices[0]);
    }

    _settings.setInListener(this);
    _settings.sampleRate = 44100;
    _settings.numOutputChannels = 0;
    _settings.numInputChannels = 2;
    _settings.bufferSize = _bufferSize;
    _s.setup(_settings);

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
bool shouldRemove(TextParticle & p) {
    if (p.position.y < 0 ) return true;
    else return false;
}



//--------------------------------------------------------------
void ofApp::update() {

    scaleVolChange();

    midiOutScaleChange();


    guiInfo->frameRate = ofToString(ofGetFrameRate(), 1);
    guiInfo->scaledVol = scaledVol;

    guiInfo->eqOutput = eqOutput;
    guiInfo->fftBinSize = fft->getBinSize();
    guiInfo->plotHeight = plotHeight;
    guiInfo->scaledBaseVol = scaledBaseVol;



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
                textParticles.push_back(TextParticle( textParticleLeftPos, alphabetFont[i] ));
            } else {
                textParticles.push_back(TextParticle( textParticleRightPos, alphabetFont[i] ));
            }
        }
    } else {
        // switchOn = false;
    }

    if (!scaleVolThresholdOn(scaledVol)) {
        switchOn = false;
    }


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

    if (guiInfo->pixelRandomOn) {
        updatePlaygroundMesh(playgroundMeshPixel, playgroundMeshPixelBuff, scaledVol, 400);
    }

    if (guiInfo->textureOn) {
        updatePlaygroundMesh(playgroundMeshTri, playgroundMeshTriBuff, scaledVol, 10);
    }




}





//--------------------------------------------------------------
void ofApp::updatePlaygroundMesh(ofMesh & m, ofMesh & mBuff, float audioVol, float ratio) {

    ofPushStyle();

    float _randomRatio = ofMap(audioVol, 0, 1, 0, ratio);
    // glPointSize(ofMap(audioVol, 0, 1, 2, ratio));
    if (audioVol > 0.1) {
        for (int i = 0; i < m.getNumVertices(); i++) {
            ofVec3f _randomVec = ofVec3f( ofRandom(-_randomRatio, _randomRatio), ofRandom(-_randomRatio, _randomRatio), 0 );
            ofVec3f _updateVec = m.getVertex(i) + _randomVec;
            m.setVertex(i, _updateVec);
        }
    } else {
        for (int i = 0; i < m.getNumVertices(); i++) {
            ofVec3f _resetVec = mBuff.getVertex(i);
            m.setVertex(i, _resetVec);
        }
    }

    ofPopStyle();

}




//--------------------------------------------------------------
void ofApp::scaleVolChange() {

    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    scaledBaseVol = ofMap(smoothedBaseVol, 0.0, 0.17, 0.0, 1.0, true);
    scaledMiddleVol = ofMap(smoothedMiddleVol, 0.0, 0.17, 0.0, 1.0, true);
    scaledHighVol = ofMap(smoothedHighVol, 0.0, 0.17, 0.0, 1.0, true);


    guiInfo->scaledVol = scaledVol;
    guiInfo->scaledBaseVol = scaledBaseVol;
    guiInfo->scaledMiddleVol = scaledMiddleVol;
    guiInfo->scaledHighVol = scaledHighVol;

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



    cam.begin();
    // pointLight.enable();

    ofDisableDepthTest();

    ofTranslate(-ofGetWidth() * 0.5, -ofGetHeight() * 0.5);
    ofPushStyle();
    ofSetColor(255);
    palast_playzone.draw(0, 0, palast_playzone.getWidth() * fullScreenRatio, palast_playzone.getHeight() * fullScreenRatio);
    ofPopStyle();

    ofEnableDepthTest();


    if (guiInfo->textOn) {
        ofPushMatrix();

        ofEnableDepthTest();
        ofEnableLighting();

        pointLightL.enable();
        pointLightR.enable();

        ofTranslate(0, 0, -30);
        for (int i = 0; i < textParticles.size(); ++i) {
            textParticles[i].draw();
        }

        pointLightL.disable();
        pointLightR.disable();

        ofDisableLighting();
        ofDisableDepthTest();

        pointLightL.draw();
        pointLightR.draw();

        ofPopMatrix();
    }


    if (guiInfo->pixelRandomOn) {
        glPointSize(ofMap(scaledVol, 0, 1, 3, 30));
        playgroundMeshPixel.drawFaces();
    } else {
        glPointSize(1);
    }


    if (guiInfo->textureOn) {
        ofPushMatrix();
        ofPushStyle();
        // ofSetColor(255);
        playgroundMeshTri.drawFaces();

        // ofSetColor(0, 180);
        // ofDrawRectangle(0, 0, ofGetWidth() * fullScreenRatio, ofGetHeight() * fullScreenRatio);
        // playgroundMeshTri.drawWireframe();

        for (int i = 0; i < playgroundMeshTri.getNumVertices() - 1; i += 1) {
            ofDrawLine(playgroundMeshTri.getVertex(i), playgroundMeshTri.getVertex(i + 1));
        }

        ofPopStyle();
        ofPopMatrix();
    }


    ofDisableDepthTest();

    // palast_trans.draw(0, 0, ofGetWidth() * fullScreenRatio, ofGetHeight() * fullScreenRatio);
    palast_trans_all.draw(0, 0, palast_trans_all.getWidth() * fullScreenRatio, palast_trans_all.getHeight() * fullScreenRatio);

    ofPushStyle();
    ofSetColor(0);
    palast_black_windows.draw(0, 0, palast_black_windows.getWidth() * fullScreenRatio, palast_black_windows.getHeight() * fullScreenRatio);
    ofPopStyle();


    if (guiInfo->windowsOn) {
        ofPushStyle();
        ofSetColor(255);
        palast_white_windows.draw(0, 0, palast_white_windows.getWidth() * fullScreenRatio, palast_white_windows.getHeight() * fullScreenRatio);
        ofPopStyle();
    }

    ofEnableDepthTest();



    cam.end();
    // pointLight.disable();



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


}





//--------------------------------------------------------------
bool ofApp::noteOff() {

    static int _timer = 0;
    _timer++;
    if (_timer > 120) {
        return true;
    }
    return false;

}



//--------------------------------------------------------------
bool ofApp::scaleVolThresholdOn(float _scaledVol) {

    if (_scaledVol > guiInfo->audioThresholdLevel) {
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

    if (_scaledVol > guiInfo->audioThresholdLevel && th) {
        count += 1;
        th = false;
    }

    if (_scaledVol < guiInfo->audioThresholdLevel) {
        th = true;
    }

    return count;

}






//--------------------------------------------------------------
void ofApp::textView(int _index) {

    ofPushStyle();
    ofPushMatrix();

    ofTranslate(windowSize.x * 0.347, windowSize.y * fullScreenRatio - 250);


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
void ofApp::audioIn(ofSoundBuffer & input) {

    float curVol = 0.0;

    int numCounted = 0;

    for (size_t i = 0; i < input.getNumFrames(); i++) {
        left[i]     = input[i * 2] * guiInfo->volumeInput;
        right[i]    = input[i * 2 + 1] * guiInfo->volumeInput;

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

