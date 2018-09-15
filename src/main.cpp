#include "ofMain.h"
#include "ofApp.h"
#include "GuiInfoApp.h"
#include "ofAppGLFWWindow.h"


//========================================================================
int main( ){
    ofGLFWWindowSettings settings;

    settings.setSize(864, 864); // 1080 * 4 / 5 , 0.8
    settings.setPosition(ofVec2f(500,0));
    settings.resizable = false;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    settings.setSize(480, 864);
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = false;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

    shared_ptr<ofApp> mainApp(new ofApp);
    shared_ptr<GuiInfoApp> guiInfoApp(new GuiInfoApp);
    mainApp->guiInfo = guiInfoApp;

    ofRunApp(guiWindow, guiInfoApp);
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();


	// ofSetupOpenGL(1440, 810, OF_WINDOW); // 1440, 810 / 1920, 1080

	// // this kicks off the running of my app
	// // can be OF_WINDOW or OF_FULLSCREEN
	// // pass in width and height too:
	// ofRunApp( new ofApp());

}
