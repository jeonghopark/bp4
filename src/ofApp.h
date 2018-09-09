#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxFft.h"
#include "ofxGui.h"

#include "TextParticle.h"


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
    float getSmoothedVol(float * _in, int _s, int _e);

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

    void audioInputInfo(float _h, vector<float> & _v);
    vector<float> & volHistoryGenerator(float _h);

    ofTrueTypeFont verdana30;

    int scaleVolCounter(float _scaledVol);

    bool scaleVolThresholdOn(float _scaledVol);
    bool switchOn;

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
    vector<string> midiPort;
    int selectMidiPort;
    string selectMidiName;

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

    void drawEqPlot(float* array, int length, float scale, float offset);

    void scaleVolChange();
    void midiOutScaleChange();


    vector<string> getStringVector(string fileName);
    vector<string> seussLines;
    vector<string> textWords;


    void setupGui();
    ofxPanel gui;
    ofxLabel frameRate;
    ofxFloatSlider volumeInput;
    ofxFloatSlider audioThresholdLevel;
    ofxToggle pixelRandomOn;
    ofxToggle textOn;
    ofxToggle textureOn;


    float testMidiSignal;


    ofImage palast_trans;
    ofImage palast_trans_all;
    ofImage palast_black_windows;
    ofImage palast_playzone;


    vector<TextParticle> textParticles;
    vector<ofTrueTypeFont> alphabetFont;
    vector<ofTrueTypeFont> alphabetFonts;
    string alphabet;



    int lineMoving;
    bool lineMovingOnOff;
    ofColor lineColor;



    ofImage playgroundImg;
    ofMesh playgroundMeshPixel;
    ofMesh playgroundMeshTri;
    ofMesh playgroundMeshPixelBuff;
    ofMesh playgroundMeshTriBuff;

    void updatePlaygroundMeshPixelPos(float audioVol);
    void updatePlaygroundMeshTriPos(float audioVol);


};
