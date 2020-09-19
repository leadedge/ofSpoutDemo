/*

    Spout OpenFrameworks Demo program

    Spout 2.007
    OpenFrameworks 11
    Visual Studio 2017

    Copyright (C) 2020 Lynn Jarvis.

    This program can be used to test Spout development code.
    It requires the files from the Spout SDK "develop" branch as detailed below.

    Uses the ofxSkybox addon
    https://github.com/kalwalt/ofxSkyBox
    Created by kalwalt alias Walter Perdan on 27/12/11
    Copyright 2011 http://www.kalwaltart.it/ All rights reserved.

    Uses the ofxWinMenu addon
    https://github.com/leadedge/ofxWinMenu

    The source for both of these is modified and included with the project.

    Setup required for For Visual Studio 2017 and Openframeworks 11.

    1) Download Openframeworks for Windows and Visual Studio
	   https://openframeworks.cc/download/
    2) Unzip the files to get a folder containing Openframeworks
       e.g. "of_v0.11.0_vs2017_release"
    3) Copy the folder to a convenient location and rename it as you wish
       e.g. somefolder\OPENFRAMEWORKS11
    4) Download the "develop" branch of the Spout SDK
       https://github.com/leadedge/Spout2/tree/develop
    5) Copy the entire "SpoutSDK folder into the Openframeworks "apps" folder
    6) Download the "SpoutDemo" repository
    7) Copy the "ofSpoutDemo" folder and contents to the Openframeworks "apps\myApps" folder.

    You will then have a folder structure as follows :

    OPENFRAMEWORKS11
        addons
        examples
        apps
            SpoutSDK <- the Spout SDK source files
            myApps
               emptyExample <- Openframeworks example template
			   ofSpoutDemo <- the ofSpoutDemo project folder
			       ofSpoutDemo.sln
			       ofSpoutDemo.vcxproj
			       ofSpoutDemo.vcxproj.filters
			       src <- source files
                       Addons <- Modifed source of addons
                           ofxSkyBox
                           ofxWinMenu
		           bin <- executable and required dlls
                       Data <- application data
					       fonts
					       images
					       shaders

    Open "ofSpoutDemo.sln" with Visual Studio 2017 and set to "Release"

	The first time you use Openframeworks you have to compile the library
	RH click on "openframeworksLib" - Build
		when it is compiled :
	RH click on "ofSpoutDemo" - Build
		ofSpoutDemo.exe will be in the "bin" folder
	Rename it as required. For example : ofSpoutSender.exe or ofSpoutReceiver.exe

	Build options :

	You can build the project either as a sender or receiver
	Find in ofApp.h :
		// Change to create a sender or a receiver
		// #define BUILDRECEIVER

	Modifications :

	The code may contain experimental functions which are not documented
	As many comments as possible are included in the source files.


    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

*/
#include "ofApp.h"
#include "resource.h"

// DIALOGS

INT_PTR CALLBACK UserSenderName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); // enter a sender name 
static PSTR szText;

INT_PTR  CALLBACK AdapterProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static std::string adaptername[10];
static int adaptercount = 0;
static int currentadapter = 0;
static int selectedadapter = 0;
static int senderadapter = 0;

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static int spoutVersion = 0;
static HINSTANCE g_hInstance;
static HWND g_hWnd;

// Diagnostics

INT_PTR CALLBACK Capabilities(HWND, UINT, WPARAM, LPARAM);
static char gldxcaps[1024]; // capability info
static char capsText[1024]; // For dialog
static bool bCopyClipboard = false;

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0, 0, 0);

#ifdef BUILDRECEIVER
	strcpy_s(senderName, 256, "ofSpoutReceiver");
	ofSetWindowTitle("Spout Receiver");
#else
	strcpy_s(senderName, 256, "ofSpoutSender"); // The sender name
	ofSetWindowTitle("Spout Sender");
#endif
	
	// OpenSpoutConsole(); // Empty console for debugging
	// EnableSpoutLog(); // Log to console
	EnableSpoutLogFile(senderName); // Log to file

	// Instance
	g_hInstance = GetModuleHandle(NULL);

	// Openframeworks window handle
	g_hWnd = ofGetWin32Window();

	// Set a custom window icon
	SetClassLongA(g_hWnd, GCLP_HICON, (LONG)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));

	// Load a font rather than the default
	if (!myFont.load("fonts/DejaVuSans.ttf", 12, true, true))
	  printf("Font not loaded\n");

	// Disable escape key exit
	ofSetEscapeQuitsApp(false);

	// For diagmostics
	g_ThreadedOptimization = 0;
	bLaptop = false; // NVAPI detection of a laptop
	bIntegrated = false; // Laptop using integrated GPU
	NvidiaMode = -1; // Optimus graphics mode
	NvidiaDriverVersion = 0; // Driver Version
	NvidiaBuildBranchString[128];
	DriverPrimary = 0;
	DriverSecondary = 0;
	bCopyClipboard = false;

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, g_hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	g_hMenu = menu->CreateWindowMenu();

	//
	// "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(g_hMenu, "File");
#ifndef BUILDRECEIVER
	menu->AddPopupItem(hPopup, "Sender name", false, false);
#endif
	menu->AddPopupItem(hPopup, "Graphics Adapter", false, false);
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// "Window" popup
	//
	hPopup = menu->AddPopupMenu(g_hMenu, "Window");
	bTopmost = false;
	menu->AddPopupItem(hPopup, "Show on top"); // Unchecked, auto check
#ifdef BUILDRECEIVER
	bFullScreen = false;
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and no auto-check
#endif
	bShowInfo = true;
	menu->AddPopupItem(hPopup, "Show info", true); // Checked, auto-check

	//
	// "Help" popup
	//
	hPopup = menu->AddPopupMenu(g_hMenu, "Help");
	menu->AddPopupItem(hPopup, "Diagnostics", false, false); // No auto check
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check
	
	// Adjust window to desired client size allowing for the menu
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_BORDER, true);

	// Centre on the screen
	SetWindowPos(g_hWnd, HWND_TOP,
		(ofGetScreenWidth() - (rect.right - rect.left)) / 2,
		(ofGetScreenHeight() - (rect.bottom - rect.top)) / 2,
		(rect.right - rect.left), (rect.bottom - rect.top), SWP_NOREDRAW);

	// Set the menu to the window
	menu->SetWindowMenu();

	// Get the window dimensions for fullscreen
	GetWindowRect(g_hWnd, &windowRect);
	GetClientRect(g_hWnd, &clientRect);

	// 3D drawing setup for the demo 
	ofDisableArbTex(); // Needed for ofBox texturing
	myBoxImage.load("images/SpoutLogoMarble3.jpg"); // image for the cube texture
 	rotX = 0.0f;
	rotY = 0.0f;

	// Set the sender size
	senderWidth = ofGetWidth();
	senderHeight = ofGetHeight();

	// Create an RGBA fbo for texture transfers
	myFbo.allocate(senderWidth, senderHeight, GL_RGBA);

	char name[64];
#ifdef BUILDRECEIVER
	// Allocate an RGBA texture to receive from the sender
	// It can be resized later to match the sender - see Update()
	myTexture.allocate(senderWidth, senderHeight, GL_RGBA);
	// Get the current graphics adapter index
	currentadapter = receiver.GetAdapter();
	// Create an adapter name list for the dialog
	adaptercount = receiver.GetNumAdapters();
	adaptername->clear();
	for (int i = 0; i < adaptercount; i++) {
		receiver.GetAdapterName(i, name, 64);
		adaptername[i] = name;
		// printf("%s\n", adaptername[i].c_str());
	}

	// Get the current user-set share mode from the registry
	// (0 - texture : 1 - memory : 2 - auto)
	shareMode = receiver.GetShareMode();
#else
	// Give the sender a name (if no name is specified, the executable name is used)
	sender.SetSenderName(senderName);
	// Get the current graphics adapter index
	currentadapter = sender.GetAdapter();
	// Create an adapter name list for the dialog
	adaptercount = sender.GetNumAdapters();
	adaptername->clear();
	for (int i = 0; i < adaptercount; i++) {
		sender.GetAdapterName(i, name, 64);
		adaptername[i] = name;
	}
	// Get the current user-set share mode from the registry
	// (0 - texture : 1 - memory : 2 - auto)
	shareMode = sender.GetShareMode();
#endif

	// Disable graphics adapter selection for memoryshare
	if (shareMode == 1) {
		menu->EnablePopupItem("Graphics Adapter", false);
	}

	// Skybox setup
	skybox.load();

	// Use ofEasyCam for mouse control instead of ofCamera
	easycam.setPosition(ofVec3f(0, 0, 0));

	// Enlarge the easycam control area so we have 
	// x and y control but not rotation outside it
	// Scale to the window width
	ofRectangle controlArea = easycam.getControlArea();
	float scale = (float)ofGetWidth() / (float)ofGetHeight();
	controlArea.scaleFromCenter(scale, scale);
	easycam.setControlArea(controlArea);

} // end setup


//--------------------------------------------------------------
void ofApp::update() {
#ifdef BUILDRECEIVER
	// If IsUpdated() returns true, the sender size has changed
	// and the receiving texture must be re-sized.
	if (receiver.IsUpdated()) {
		myTexture.allocate(receiver.GetSenderWidth(), receiver.GetSenderHeight(), GL_RGBA);
	}
#else

	// Double click reset of easycam position
	ofVec3f pos = easycam.getPosition();
	if (pos.z > 0.0f) {
		easycam.setPosition(ofVec3f(0, 0, 0));
	}

	// senderWidth and senderHeight are reset when the window is resized
	// Update the sending fbo dimensions
	// The sender is updated on the next call to SendTexture (2.007 only)
	if (senderWidth != (unsigned int)myFbo.getWidth()
	 || senderHeight != (unsigned int)myFbo.getHeight()) {
		myFbo.allocate(senderWidth, senderHeight, GL_RGBA);
		// Re-scale easycam control area
		ofRectangle controlArea = easycam.getControlArea(); // Current viewport
		float scale = (float)ofGetWidth() / (float)ofGetHeight();
		controlArea.scaleFromCenter(scale, scale);
		easycam.setControlArea(controlArea);
	}
		

#endif
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0, 0, 0);

#ifdef BUILDRECEIVER

	char text[256];
	std::string str;
	ofSetColor(255);

	// ReceiveTexture connects to and receives from a sender
	if(receiver.ReceiveTexture(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget))
		myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
	else
		ofClear(10, 100, 140, 255);

	if (bShowInfo && !bFullScreen) {

		if (receiver.IsConnected()) {

			// GetSenderFrame() will return false for senders < 2.007
			// Frame counting can also be disabled in SpoutSettings
			if (receiver.GetSenderFrame() > 0) {
				sprintf_s(text, 256, "Receiving : [%s] (%dx%d : fps %2.0f : frame %d)",
					receiver.GetSenderName(), // sender name
					receiver.GetSenderWidth(), // width
					receiver.GetSenderHeight(), // height 
					receiver.GetSenderFps(), // fps
					receiver.GetSenderFrame()); // frame since the sender started
			}
			else {
				sprintf_s(text, 256, "Receiving : [%s] (%dx%d)",
					receiver.GetSenderName(),
					receiver.GetSenderWidth(),
					receiver.GetSenderHeight());
			}
			str = text;
			myFont.drawString(str, 10, 30);
			
			// If the receiver and sender are using different adapters,
			// show the sender graphics adapter index and name.
			// The sender adapter index retrieved could be anything for < 2.007.
			// Make sure it's in the range of adapters and has a name.
			senderadapter = receiver.spout.interop.GetSenderAdapter(receiver.GetSenderName());
			if (senderadapter >= 0 
				&& senderadapter < adaptercount
				&& senderadapter != currentadapter
				&& !empty(adaptername[senderadapter])) {
					str = "Sender adapter ";
					str += ofToString(senderadapter);
					str += " : ";
					str += adaptername[senderadapter];
					myFont.drawString(str, 10, 74);
			}
			myFont.drawString("RH click - select sender\n'f'  full screen : '  '  hide info", 10, ofGetHeight() - 30);
		}
		else {
			myFont.drawString("No sender detected", 10, 30);
		}

		// Graphics adapter details
		//    o	Adapter name
		//    o User selected sharing mode
		//    o Compatibility sharing mode

		// Get the current graphics adapter index and name
		str = ofToString(currentadapter);
		str += " : ";
		str += adaptername[currentadapter];

		// Memoryshare
		if (shareMode == 1) {
			str += " : Memory share";
		}
		// Texture share (Auto share mode to be removed)
		else {
			if (receiver.GetMemoryShare())
				str += " : Memory share compatible";
			else
				str += " : Texture share";
		}
		myFont.drawString(str, 10, 52);

	} // endif show info

#else

	// - - - - - - - - - - - - - - - - 

	// Draw 3D graphics into the fbo
	myFbo.begin();

	// Clear to reset the background and depth buffer
	ofClear(0, 0, 0, 255);

	// Draw the skybox
	easycam.begin();
	skybox.draw();
	easycam.end();

	// Draw the rotating cube
	ofEnableDepthTest(); // enable depth comparisons
	ofPushMatrix();
	ofTranslate(myFbo.getWidth() / 2.0, myFbo.getHeight() / 2.0, 0);
	ofRotateYDeg(rotX); // rotate
	ofRotateXDeg(rotY);
	myBoxImage.bind(); // bind our box face image
	ofDrawBox(0.45*myFbo.getHeight()); // draw the box
	myBoxImage.unbind();
	ofPopMatrix();
	rotX += 0.6;
	rotY += 0.6;

	myFbo.end();

	// - - - - - - - - - - - - - - - - 

	// Draw the result in the fbo sized to the application window
	myFbo.draw(0, 0, ofGetWidth(), ofGetHeight());

	//
	// Send texture
	//

	// The fbo texture is already inverted so set the option false
	sender.SendTexture(myFbo.getTexture().getTextureData().textureID,
		myFbo.getTexture().getTextureData().textureTarget,
		senderWidth, senderHeight, false);

	// Show what it is sending
	if (bShowInfo) {

		ofSetColor(255);
		std::string str = "Sending as : ";
		str += sender.GetName(); str += " (";
		str += ofToString(sender.GetWidth()); str += "x";
		str += ofToString(sender.GetHeight()); str += ")";
		// Show sender fps and framecount if selected
		if (sender.GetFrame() > 0) {
			str += " fps ";
			str += ofToString((int)roundf(sender.GetFps()));
			str += " : frame  ";
			str += ofToString(sender.GetFrame());
		}
		else {
			// Show Openframeworks fps
			str += " fps : " + ofToString((int)roundf(ofGetFrameRate()));
		}
		myFont.drawString(str, 20, 30);

		// Graphics adapter details
		//    o	Adapter name
		//    o User selected sharing mode
		//    o Compatibility sharing mode

		// Get the current graphics adapter index and name
		str = ofToString(currentadapter);
		str += " : ";
		str += adaptername[currentadapter];

		// Memoryshare
		if (shareMode == 1) {
				str += " : Memory share";
		}
		// Texture share (Auto share mode to be removed)
		else {
			if (sender.GetMemoryShare())
				str += " : Memory share compatible";
			else
				str += " : Texture share";
		}

		myFont.drawString(str, 20, 52);

	} // endif show info

#endif

} // end Draw


//--------------------------------------------------------------
void ofApp::exit() {

	// Close the sender or receiver on exit
#ifdef BUILDRECEIVER
	receiver.ReleaseReceiver();
#else
	sender.ReleaseSender();
#endif

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);

#ifdef BUILDRECEIVER
	if (button == 2) { // rh button
		// Open the sender selection panel
		// Spout must have been installed
		receiver.SelectSender();
	}
#else
	UNREFERENCED_PARAMETER(button);
#endif

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

#ifdef BUILDRECEIVER
	if (key == OF_KEY_ESC) {
		if (bFullScreen) {
			bFullScreen = false;
			doFullScreen(bFullScreen);
		}
	}

	if(key == 'f') {
		bFullScreen = !bFullScreen;
		doFullScreen(bFullScreen);
	}

#endif

	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Handle menu check mark
		menu->SetPopupItem("Show info", bShowInfo);
	}

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) 
{

#ifndef BUILDRECEIVER
	if (w > 0 && h > 0) {
		// Update the sender dimensions to match the window size
		senderWidth = w;
		senderHeight = h;
	}
#endif

}


//--------------------------------------------------------------
//
// Menu function callback
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	//
	// File menu
	//

#ifndef BUILDRECEIVER
	if (title == "Sender name") {
		char sendername[256];
		strcpy_s(sendername, senderName);
		if (EnterSenderName(sendername)) {
			if (strcmp(sendername, senderName) != 0) {
				// Release the current sender and start again
				strcpy_s(senderName, 256, sendername);
				sender.ReleaseSender();
				sender.CreateSender(senderName, senderWidth, senderHeight);
				// Get the name in case of multiple numbered names of the same sender
				sender.spout.GetActiveSender(senderName);
			}
		}
	}
#endif

	if (title == "Graphics Adapter") {
		SelectAdapter();
	}

	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//

	if (title == "Full screen") {
		bFullScreen = !bFullScreen;
		doFullScreen(bFullScreen);
	}

	if (title == "Show on top") {
		bTopmost = bChecked;
		if (bTopmost) {
			// get the current top window
			g_hwndForeground = GetForegroundWindow();
			SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			// CheckMenuItem(hmenu, IDM_TOPMOST, MF_BYCOMMAND | MF_CHECKED);
			ShowWindow(g_hWnd, SW_SHOW);
		}
		else {
			SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			// CheckMenuItem(hmenu, IDM_TOPMOST, MF_BYCOMMAND | MF_UNCHECKED);
			ShowWindow(g_hWnd, SW_SHOW);
			// Reset the window that was top before
			if (GetWindowLong(g_hwndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
				SetWindowPos(g_hwndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			else
				SetWindowPos(g_hwndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	if (title == "Show info") {
		bShowInfo = bChecked;
	}


	//
	// Help menu
	//

	if (title == "Diagnostics") {
		DoDiagnostics();
		DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_CAPSBOX), g_hWnd, Capabilities);
	}

	if (title == "About") {
		DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ABOUTBOX), g_hWnd, About);
	}

} // end appMenuFunction


void ofApp::doFullScreen(bool bFullscreen)
{
	RECT rectTaskBar;
	HWND hWndTaskBar = NULL;
	char tmp[256];

	if (bFullscreen) { // Set to full screen

		// Get the first visible window in the Z order
		g_hwndTopmost = GetTopWindow(NULL);
		GetWindowTextA(g_hwndTopmost, (LPSTR)tmp, 256); // hwnd can be null
		do {
			if (g_hwndTopmost && tmp[0] && IsWindowVisible(g_hwndTopmost)
				&& GetWindowLong(g_hwndTopmost, GWL_EXSTYLE) & WS_EX_TOPMOST) {
				break;
			}
			g_hwndTopmost = GetNextWindow(g_hwndTopmost, GW_HWNDNEXT);
			GetWindowTextA(g_hwndTopmost, (LPSTR)tmp, 256);
		} while (g_hwndTopmost != NULL); // g_hwndTopmost is NULL if GetNextWindow finds no more windows

		// Get the client/window adjustment values
		GetWindowRect(g_hWnd, &windowRect);
		GetClientRect(g_hWnd, &clientRect);
		AddX = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
		AddY = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);
		nonFullScreenX = clientRect.right - clientRect.left;
		nonFullScreenY = clientRect.bottom - clientRect.top;

		// Save current size values
		GetWindowRect(g_hWnd, &windowRect);
		GetClientRect(g_hWnd, &clientRect);

		// Remove caption and borders
		g_dwStyle = GetWindowLongPtrA(g_hWnd, GWL_STYLE);
		SetWindowLongPtrA(g_hWnd, GWL_STYLE, WS_VISIBLE); // no other styles but visible

		// Remove the menu
		g_hMenu = GetMenu(g_hWnd);
		SetMenu(g_hWnd, NULL);

		// Get the taskbar window handle
		hWndTaskBar = FindWindowA("Shell_TrayWnd", "");
		GetWindowRect(g_hWnd, &rectTaskBar);

		// Put the taskbar lowest Z order
		SetWindowPos(hWndTaskBar, HWND_NOTOPMOST, 0, 0, (rectTaskBar.right - rectTaskBar.left), (rectTaskBar.bottom - rectTaskBar.top), SWP_NOMOVE | SWP_NOSIZE);

		// If it's on a different monitor, make it fullscreen on that monitor.
		// Thanks to Tim Thompson for resolving this
		HMONITOR monitor = MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfoA(monitor, &mi);
		int x = (int)mi.rcMonitor.left;
		int y = (int)mi.rcMonitor.top;
		int w = (int)(mi.rcMonitor.right - mi.rcMonitor.left); // rcMonitor dimensions are LONG
		int h = (int)(mi.rcMonitor.bottom - mi.rcMonitor.top);

		// Hide the window while re-sizing
		ShowWindow(g_hWnd, SW_HIDE);
		
		// SetWindowPos HWND_TOPMOST can cause a grey screen for Windows 10 
		// if scaling is set larger than 100% e.g. 125%
		// Also there seems to be a delay in receiving keystrokes even if set to 100%
		// This seems to fix it.
		SetWindowPos(g_hWnd, HWND_NOTOPMOST, x, y, w, h, SWP_SHOWWINDOW);
		SetWindowPos(g_hWnd, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
		ShowCursor(FALSE);

		ShowWindow(g_hWnd, SW_SHOW);
		SetFocus(g_hWnd);

	}
	else { // Exit full screen

		// Restore original style
		SetWindowLongPtrA(g_hWnd, GWL_STYLE, g_dwStyle);

		// Restore the menu
		SetMenu(g_hWnd, g_hMenu);

		// Restore our window and remove topmost after full screen
		SetWindowPos(g_hWnd, HWND_NOTOPMOST, windowRect.left, windowRect.top, nonFullScreenX + AddX, nonFullScreenY + AddY, SWP_SHOWWINDOW);

		// Reset the window that was topmost before - it could be ours
		if (g_hwndTopmost)
			SetWindowPos(g_hwndTopmost, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		ShowCursor(TRUE);
		SetFocus(g_hWnd);

	}

}


bool ofApp::EnterSenderName(char *SenderName)
{
	int retvalue = 0;
	unsigned char textin[256];

	strcpy_s(szText = (PSTR)textin, 256, SenderName); // move to a static string for the dialog

	// Show the dialog box 
	retvalue = (int)DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_NAMEBOX), g_hWnd, (DLGPROC)UserSenderName);

	// OK - sender name has been entered
	if (retvalue != 0) {
		strcpy_s(SenderName, 256, szText); // Return the entered name
		return true;
	}

	// Cancel
	return false;

}

// Message handler for sender name entry for a sender
INT_PTR CALLBACK UserSenderName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message) {

	case WM_INITDIALOG:
		// Fill text entry box with passed string
		SetDlgItemTextA(hDlg, IDC_NAMETEXT, szText);
		// Select all text in the edit field
		SendDlgItemMessage(hDlg, IDC_NAMETEXT, EM_SETSEL, 0, 0x7FFF0000L);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			// Get contents of edit field into static 256 char string
			GetDlgItemTextA(hDlg, IDC_NAMETEXT, szText, 256);
			EndDialog(hDlg, 1);
			break;
		case IDCANCEL:
			// User pressed cancel.  Just take down dialog box.
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		default:
			return (INT_PTR)FALSE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}



void ofApp::SelectAdapter()
{
	// OpenGL/DirectX interop compatibility is unknown for the selected adapter unless tested.
	// Either use Memory share to bypass the test or the share mode will switch if necessary.
	// The adapter name list should not have changed since the program started

	#ifdef BUILDRECEIVER
		shareMode = receiver.GetShareMode(); // User selected share mode
		currentadapter = receiver.GetAdapter(); // Get the current adapter index
		selectedadapter = currentadapter; // The index to be selected in the dialog
	#else
		shareMode = sender.GetShareMode(); // User selected share mode
		currentadapter = sender.GetAdapter(); // Get the current adapter index
		selectedadapter = currentadapter; // The index to be selected in the dialog
	#endif

	// Show the dialog box 
	int retvalue = (int)DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ADAPTERBOX), g_hWnd, (DLGPROC)AdapterProc);
	
	if (retvalue != 0) {
		// OK - adapter index (selectedadapter) has been selected
#ifdef BUILDRECEIVER
		// Set the selected adapter if different
		// A new sender will be detected on the first ReceiveTexture call (Requires 2.007)
		if (selectedadapter != currentadapter) {
			// A new sender using the selected adapter will be created
			// on the first SendTexture call (Requires 2.007)
			receiver.ReleaseReceiver();
			// Update the share mode flags for repeats
			// The application could have changed to memory share
			// the last time an adapter was selected.
			receiver.SetShareMode(shareMode);
			if (!receiver.SetAdapter(selectedadapter)) {
				// SetAdapter returns to the primary adapter for failure
				// Refer to error logs for diagnostics
				MessageBoxA(NULL, "Could not select graphics adapter", "SpoutDemo", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
				// Set the adapter back to what it was (the compatibility test is repeated)
				receiver.SetAdapter(currentadapter);
			}
			else {
				// Change the application current adapter index to the one selected
				// This will take effect when the sender is re-created
				currentadapter = selectedadapter;
			}
			// If the user selected Texture share mode, the compatibility test
			// could have switched the application to memory share.
			// This can be tested with : GetMemoryShare();
		}
#else
		// Set the selected adapter if different
		if (selectedadapter != currentadapter) {
			// A new sender using the selected adapter will be created
			// on the first SendTexture call (Requires 2.007)
			sender.ReleaseSender();
			// Update the share mode flags for repeats
			// The application could have changed to memory share
			// the last time an adapter was selected.
			sender.SetShareMode(shareMode);
			if (!sender.SetAdapter(selectedadapter)) {
				// SetAdapter returns to the primary adapter for failure
				// Refer to error logs for diagnostics
				MessageBoxA(NULL, "Could not select graphics adapter", "SpoutDemo", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
				// Set the adapter back to what it was (the compatibility test is repeated)
				sender.SetAdapter(currentadapter);
			}
			else {
				// Change the application current adapter index to the one selected
				// This will take effect when the sender is re-created
				currentadapter = selectedadapter;
			}
			// If the user selected Texture share mode, the compatibility test
			// could have switched the application to memory share.
			// This can be tested with : GetMemoryShare();
		}
#endif
	}

}

// Message handler for selecting adapter
INT_PTR  CALLBACK AdapterProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam); // suppress warning

	HWND hwndList = NULL;
	int i = 0;
	char name[128];

	switch (message) {

	case WM_INITDIALOG:
		// Adapter combo selection
		hwndList = GetDlgItem(hDlg, IDC_ADAPTERS);
		if (adaptercount < 10) {
			for (i = 0; i < adaptercount; i++) {
				sprintf_s(name, 128, "%d : %s", i, adaptername[i].c_str());
				SendMessageA(hwndList, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)name);
			}
			// Display an initial item in the selection field
			SendMessageA(hwndList, CB_SETCURSEL, (WPARAM)currentadapter, (LPARAM)0);
		}
		return TRUE;

	case WM_COMMAND:

		// Combo box selection
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			selectedadapter = (int)SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		}
		// Drop through

		switch (LOWORD(wParam)) {

		case IDOK:
			// Return the selected adapter index
			EndDialog(hDlg, 1);
			break;

		case IDCANCEL:
			// User pressed cancel.
			// Reset the selected index and take down dialog box.
			selectedadapter = currentadapter;
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;

		default:
			return (INT_PTR)FALSE;
		}
	}

	return (INT_PTR)FALSE;
}

// Message handler for About box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char tmp[MAX_PATH];
	char fname[MAX_PATH];
	char about[1024];
	DWORD dummy, dwSize, dwVersion = 0;

	LPDRAWITEMSTRUCT lpdis;
	HWND hwnd = NULL;
	HCURSOR cursorHand = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	switch (message) {

	case WM_INITDIALOG:

		sprintf_s(about, 256, "Spout demo program\r\n       Version ");
		// Get product version number
		if (GetModuleFileNameA(hInstance, tmp, MAX_PATH)) {
			dwSize = GetFileVersionInfoSizeA(tmp, &dummy);
			if (dwSize > 0) {
				vector<BYTE> data(dwSize);
				if (GetFileVersionInfoA(tmp, NULL, dwSize, &data[0])) {
					LPVOID pvProductVersion = NULL;
					unsigned int iProductVersionLen = 0;
					if (VerQueryValueA(&data[0], ("\\StringFileInfo\\080904E4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
						sprintf_s(tmp, MAX_PATH, "%s\r\n", (char *)pvProductVersion);
						strcat_s(about, 1024, tmp);
					}
				}
			}
		}
		// strcat_s(about, 1024, "\r\n");

		// Get the Spout version
		spoutVersion = 0;
		if (ReadDwordFromRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\Spout", "Version", &dwVersion))
			spoutVersion = (int)dwVersion; // 0 for earlier than 2.005

		if (spoutVersion > 0) {
			sprintf_s(tmp, 256, "Spout 2.00%1d", (spoutVersion - 2000));
			strcat_s(about, 1024, tmp);
			// Check Spout installation
			if (ReadPathFromRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\SpoutPanel", "InstallPath", fname)) {
				// Does the SpoutPanel path have "Program Files" in it ?
				if (strstr(fname, "Program Files"))
					sprintf_s(tmp, 256, " installed\r\n");
				else
					sprintf_s(tmp, 256, " portable\r\n");
				strcat_s(about, 1024, tmp);
			}
			else {
				strcat_s(about, 1024, "\r\n");
			}
		}
		else {
			sprintf_s(tmp, 256, "Spout 2.004 or earlier\r\n");
			strcat_s(about, 1024, tmp);
		}

		SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

		// Hyperlink hand cursor
		cursorHand = LoadCursor(NULL, IDC_HAND);
		hwnd = GetDlgItem(hDlg, IDC_SPOUT_URL);
		SetClassLongA(hwnd, GCLP_HCURSOR, (long)cursorHand);
		break;

	case WM_DRAWITEM:

		// The blue hyperlink
		lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID == -1) break;
		SetTextColor(lpdis->hDC, RGB(6, 69, 173));
		DrawTextA(lpdis->hDC, "http://spout.zeal.co", -1, &lpdis->rcItem, DT_LEFT);
		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDC_SPOUT_URL) {
			sprintf_s(tmp, MAX_PATH, "http://spout.zeal.co");
			ShellExecuteA(hDlg, "open", tmp, NULL, NULL, SW_SHOWNORMAL);
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

// 
// Diagnostics
//

// Message handler for caps dialog
INT_PTR CALLBACK Capabilities(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Copied from SpoutSettings
	UNREFERENCED_PARAMETER(lParam);
	HCURSOR cursorHand = NULL;
	LPDRAWITEMSTRUCT lpdis;
	char tmp[MAX_PATH];

	switch (message) {

	case WM_INITDIALOG:

		// Diagnostics text
		SetDlgItemTextA(hDlg, IDC_CAPSTEXT, (LPCSTR)gldxcaps);

		// Hyperlink hand cursor
		cursorHand = LoadCursor(NULL, IDC_HAND);
		SetClassLongPtrA(GetDlgItem(hDlg, IDC_SPOUT_URL), GCLP_HCURSOR, (LONG_PTR)cursorHand);

		return (INT_PTR)TRUE;

	case WM_DRAWITEM:

		// The blue hyperlinks
		lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID == -1) break;
		SetTextColor(lpdis->hDC, RGB(6, 69, 173));
		switch (lpdis->CtlID) {
		case IDC_SPOUT_URL:
			DrawTextA(lpdis->hDC, "https://spout.zeal.co", -1, &lpdis->rcItem, DT_LEFT);
			break;
		default:
			break;
		}
		break;

	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_SPOUT_URL:
			sprintf_s(tmp, MAX_PATH, "http://spout.zeal.co");
			ShellExecuteA(hDlg, "open", tmp, NULL, NULL, SW_SHOWNORMAL);
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;

		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDC_LOG:
			ShowSpoutLogs();
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDC_COPY:
		{
			// Crash on repeat for some reason
			if (!bCopyClipboard && gldxcaps[0] && strlen(gldxcaps) > 16) {
				if (OpenClipboard(g_hWnd)) {
					HGLOBAL clipbuffer = NULL;
					char* buffer = nullptr;
					EmptyClipboard();
					clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(gldxcaps) + 1);
					if (clipbuffer) {
						buffer = (char*)GlobalLock(clipbuffer);
						strcpy_s(buffer, strlen(gldxcaps) + 1, LPCSTR(gldxcaps));
						GlobalUnlock(clipbuffer);
						SetClipboardData(CF_TEXT, clipbuffer); // ??? crash here on repeat
						MessageBoxA(hDlg, "Diagnostics copied to the clipboard.", "Spout Demo", MB_OK);
						GlobalFree(clipbuffer);
						bCopyClipboard = true;
					}
					CloseClipboard();
				}
				else {
					MessageBoxA(hDlg, "Unknown clipboard open error.", "Spout Demo", MB_OK);
				}
			}
			else {
				MessageBoxA(hDlg, "Diagnostics copied to the clipboard.", "Spout Demo", MB_OK);
			}
			return (INT_PTR)TRUE;
		}

		default:
			return (INT_PTR)FALSE;

		} // end switch (LOWORD(wParam))
	}

	return (INT_PTR)FALSE;
}

bool ofApp::DoDiagnostics()
{

	SpoutLogNotice("\nSpout Demo Program : DoDiagnostics\n");

	// Copied from SpoutSettings
	bool bMemory = false;
	bool bDX9 = false;
	bool bDX9available = false;
	bool bDX11available = false;

	Spout *spout = new Spout; // local (no diagnostics are initially performed)

	gldxcaps[0] = 0; // for repeats
	sprintf_s(gldxcaps, 1024, "Spout Demo Program\r\n");

	//
	// Check DirectX
	//

	// Check for DirectX 9.0c
	if (CheckForDirectX9c()) {
		strcat_s(gldxcaps, 1024, "DirectX 9c installed\r\n");
		// DirectX 9 installed - try to open a DX9 device
		bDX9available = spout->interop.OpenDirectX9(g_hWnd);
		if (!bDX9available) {
			strcat_s(gldxcaps, 1024, "Warning : DirectX 9 device not available\r\n");
			bDX9 = false;
		}
		else {
			spout->interop.CleanupDX9();
			strcat_s(gldxcaps, 1024, "DirectX 9 device available\r\n");
		}
		// Try to open a DX11 device
		bDX11available = spout->interop.OpenDirectX11();
		if (!bDX11available) {
			strcat_s(gldxcaps, 1024, "Warning : DirectX 11 device not available\r\n");
		}
		else {
			spout->interop.CleanupDX11();
			strcat_s(gldxcaps, 1024, "DirectX 11 device available\r\n");
		}
	}
	else {
		strcat_s(gldxcaps, 1024, "DirectX 9.0c not installed\r\n");
		strcat_s(gldxcaps, 1024, "DirectX 9 device not available\r\n");
		bDX9 = false;
		bDX11available = spout->interop.OpenDirectX11();
		if (!bDX11available) {
			strcat_s(gldxcaps, 1024, "DirectX 11 device not available\r\n");
		}
		else {
			spout->interop.CleanupDX11();
			strcat_s(gldxcaps, 1024, "DirectX 11 device available\r\n");
		}
	}

	// Check the vendor of the primary GPU currently in use
	strcat_s(gldxcaps, 1024, "Primary adapter : ");
	strcat_s(gldxcaps, 1024, (char *)glGetString(GL_VENDOR));
	strcat_s(gldxcaps, 1024, "\r\n");

	// Find the primary adapter details using DirectX and Windows functions
	char adapter[256]; adapter[0] = 0;;
	char display[256]; display[0] = 0;
	spout->interop.spoutdx.GetAdapterInfo(adapter, display, 256);

	//
	// Add the driver desc to the caps text
	//

	// The display adapter
	if (display) trim(display);
	if (display[0]) {
		strcat_s(gldxcaps, 1024, display);
		strcat_s(gldxcaps, 1024, " (Display) : ");
		strcat_s(gldxcaps, 1024, "\r\n");
	}

	// Secondary adapter
	if (adapter) trim(adapter);
	if (adapter[0]) {
		trim(adapter);
		if (strcmp(adapter, display) != 0) {
			strcat_s(gldxcaps, 1024, adapter);
			strcat_s(gldxcaps, 1024, " (Secondary)");
			strcat_s(gldxcaps, 1024, "\r\n");
		}
	}

	// Check for Laptop using NVAPI
	bIntegrated = false;
	if (g_NvApi.IsLaptop(&bIntegrated)) {
		strcat_s(gldxcaps, 1024, "Laptop system detected\r\n");
		if (bIntegrated) {
			// Get the current Optimus NVIDIA setting from the NVIDIA base profile
			// This will just fail for unsupported hardware and return -1
			// 0 - nvidia : 1 - integrated : 2 - auto-select, -1 fail
			NvidiaMode = g_NvApi.GetNvidiaGPU(&NvidiaDriverVersion, NvidiaBuildBranchString);
			if (NvidiaMode >= 0) {
				if (NvidiaMode == 0)
					strcat_s(gldxcaps, 1024, "Optimus using NVIDIA processor\r\n");
				if (NvidiaMode == 1)
					strcat_s(gldxcaps, 1024, "Optimus using Integrated processor\r\n");
				if (NvidiaMode == 2)
					strcat_s(gldxcaps, 1024, "Optimus Auto-select processor\r\n");
			}
		}
	}
	else {
		strcat_s(gldxcaps, 1024, "Desktop system detected\r\n");
	}

	// Check for user set memoryshare mode first (> vers 2.005)
	// Will always return false for 2.004 apps.
	bMemory = spout->GetMemoryShareMode();

	// Compatibilty check
	// DirectX is OK but check for availabilty of the GL/DX extensions.
	// The NV_DX_interop extensions will fail if the graphics driver does not support them
	if (bDX11available || bDX9available) {
		if (wglGetProcAddress("wglDXOpenDeviceNV")) { // extensions can be loaded
			strcat_s(gldxcaps, 1024, "NV_DX_interop extensions available\r\n");
			// Now actually load the extensions
			if (!spout->interop.LoadGLextensions()) {
				strcat_s(gldxcaps, 1024, "OpenGL extensions failed to load\r\n");
				strcat_s(gldxcaps, 1024, "Graphics not texture share compatible\r\n");
			}
			else {
				// It is possible that extensions load OK but that the GL/DX interop functions fail.
				// This has been noted on dual graphics machines with the NVIDIA Optimus driver.
				// If the compatibility test fails, fall back to memoryshare
				// Check OpenGL GL/DX extension functions
				// and create an interop device for success
				if (bDX11available) {
					// Create a DX11 device again
					if (spout->interop.OpenDirectX11()) {
						if (!spout->interop.GLDXready()) {
							strcat_s(gldxcaps, 1024, "Warning : OpenGL/DX11 texture sharing failed\r\n");
						}
						else {
							strcat_s(gldxcaps, 1024, "OpenGL/DX11 texture sharing succeeded\r\n");
						}
						spout->interop.CleanupDX11();
					}
					else {
						strcat_s(gldxcaps, 1024, "DX11 initialization failed\r\n");
					}
				}
				else if (bDX9available) {
					// Create a DX9 device again
					if (spout->interop.OpenDirectX9(g_hWnd)) {
						if (!GLDXready(true)) {
							strcat_s(gldxcaps, 1024, "Warning : OpenGL/DX9 texture sharing failed\r\n");
						}
						else {
							strcat_s(gldxcaps, 1024, "OpenGL/DX9 texture sharing suceeded\r\n");
						}
						spout->interop.CleanupDX9();
					}
				}

				bDX9 = spout->GetDX9(); // DX9 or DX11
				bMemory = spout->interop.GetMemoryShare(); // Auto share mode
				strcat_s(gldxcaps, 1024, "Compatibility");
				if (bMemory)
					strcat_s(gldxcaps, 1024, " - Memory share\r\n");
				else {
					if (bDX9)
						strcat_s(gldxcaps, 1024, " - Texture (DirectX 9)\r\n");
					else
						strcat_s(gldxcaps, 1024, " - Texture (DirectX 11)\r\n");
				}

			} // loaded extensions OK
		}
		else {
			// The extensions required for texture access are not available.
			strcat_s(gldxcaps, 1024, "NV_DX_interop extensions not supported\r\n");
			strcat_s(gldxcaps, 1024, "Graphics not texture share compatible\r\n");
		}
	}

	delete spout;

	return true;
}


//
// Test whether the NVIDIA OpenGL/DirectX interop extensions function correctly. 
// Creates dummy textures and uses the interop functions.
// Creates an interop device on success.
// Must be called after OpenDirectX.
// Failure means fall back to Memoryshare mode
// Success means the GLDX interop functions can be used.
// Other errors should not happen if OpenDirectX succeeded
bool ofApp::GLDXready(bool bDX9)
{
	HANDLE dxShareHandle = NULL; // Shared texture handle
	LPDIRECT3DTEXTURE9  dxTexture = nullptr; // shared DX9 texture
	ID3D11Texture2D* pSharedTexture = nullptr; // shared DX11 texture
	HANDLE hInteropObject = NULL; // handle to the DX/GL interop object
	GLuint glTexture = 0; // OpenGL texture linked to the shared DX texture

	Spout spout;

	// Create an opengl texture for the test
	glGenTextures(1, &glTexture);
	if (glTexture == 0) {
		MessageBoxA(NULL, "GLDXReady - glGenTextures failed", "Spout demo", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
		return false;
	}

	SpoutLogNotice("SpoutDemo::GLDXready - testing GL/DX interop functions");

	//
	// Create a directX texture and link using the NVIDIA GLDX interop functions
	//
	if (bDX9) {

		if (spout.interop.m_pDevice == NULL) {
			glDeleteTextures(1, &glTexture);
			MessageBoxA(NULL, "GLDXready (DX9) - No D3D9ex device", "SpoutSettings", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
			SpoutLogError("SpoutDemo::GLDXready (DX9) - No D3D9ex device");
			return false;
		}

		SpoutLogNotice("    Creating test DX9 texture");

		// Create a shared DirectX9 texture for the test
		dxShareHandle = NULL;
		if (!spout.interop.spoutdx.CreateSharedDX9Texture(spout.interop.m_pDevice,
			256, 256,
			D3DFMT_A8R8G8B8, // default
			dxTexture, dxShareHandle)) {
			glDeleteTextures(1, &glTexture);
			MessageBoxA(NULL, "GLDXready (DX9) - CreateSharedDX9Texture failed", "SpoutSettings", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
			SpoutLogError("SpoutDemo::GLDXready (DX9) - CreateSharedDX9Texture failed");
			return false;
		}

		SpoutLogNotice("    Linking test DX9 texture (0x%Ix)", dxTexture);

		// Link the shared DirectX9 texture to the OpenGL texture
		// If sucessful, LinkGLDXtextures initializes a class handle
		// to a GL/DirectX interop device - m_hInteropDevice
		hInteropObject = spout.interop.LinkGLDXtextures(spout.interop.m_pDevice, dxTexture, dxShareHandle, glTexture);
		if (hInteropObject == NULL) {
			SpoutLogError("SpoutDemo::GLDXready (DX9) - LinkGLDXtextures failed");
			dxTexture->Release();
			glDeleteTextures(1, &glTexture);
			return false;
		}
		SpoutLogNotice("    Test DX9 texture created and linked OK");

		if (spout.interop.m_hInteropDevice && hInteropObject) {
			wglDXUnregisterObjectNV(spout.interop.m_hInteropDevice, hInteropObject);
			if (!wglDXCloseDeviceNV(spout.interop.m_hInteropDevice)) {
				SpoutLogWarning("SpoutDemo::GLDXReady - DX9 could not close interop");
			}
			spout.interop.m_hInteropDevice = NULL;
		}
		if (dxTexture)
			dxTexture->Release();
		if (glTexture)
			glDeleteTextures(1, &glTexture);

	} // endif DX9
	else {
		SpoutLogNotice("    Creating test DX11 texture");
		// Create a new shared DirectX resource
		dxShareHandle = NULL;
		if (!spout.interop.spoutdx.CreateSharedDX11Texture(spout.interop.m_pd3dDevice,
			256, 256,
			DXGI_FORMAT_B8G8R8A8_UNORM, // default
			&pSharedTexture,
			dxShareHandle)) {
			glDeleteTextures(1, &glTexture);
			SpoutLogError("SpoutDemo::GLDXready (DX11) - CreateSharedDX11Texture failed");
			return false;
		}

		SpoutLogNotice("    Linking test DX11 texture (0x%Ix) OpenGL texture (%2d)", pSharedTexture, glTexture);

		// Link the shared DirectX texture to the OpenGL texture
		// If sucessful, LinkGLDXtextures initializes a class handle
		// to a GL/DirectX interop device - m_hInteropDevice
		hInteropObject = spout.interop.LinkGLDXtextures(spout.interop.m_pd3dDevice,
			pSharedTexture,
			dxShareHandle,
			glTexture);
		if (!hInteropObject) {
			spout.interop.spoutdx.ReleaseDX11Texture(spout.interop.m_pd3dDevice, pSharedTexture);
			SpoutLogError("SpoutDemo::GLDXready (DX11) - LinkGLDXtextures failed");
			return false;
		}

		SpoutLogNotice("    Test DX11 texture created and linked OK");

		// All passes, so unregister and release textures
		// m_hInteropDevice remains and does not need to be created again
		if (spout.interop.m_hInteropDevice && hInteropObject) {
			wglDXUnregisterObjectNV(spout.interop.m_hInteropDevice, hInteropObject);
			if (!wglDXCloseDeviceNV(spout.interop.m_hInteropDevice)) {
				SpoutLogWarning("SpoutDemo::GLDXReady DX11 - could not close interop");
			}
			spout.interop.m_hInteropDevice = NULL;
		}

		spout.interop.spoutdx.ReleaseDX11Texture(spout.interop.m_pd3dDevice, pSharedTexture);

		if (glTexture)
			glDeleteTextures(1, &glTexture);

	}

	return true;

}

bool ofApp::CheckForDirectX9c()
{
	// HKLM\Software\Microsoft\DirectX\Version should be 4.09.00.0904
	// handy information : http://en.wikipedia.org/wiki/DirectX
	HKEY  hRegKey;
	LONG  regres;
	DWORD  dwSize, major, minor, revision, notused;
	char value[256];
	dwSize = 256;

	// Does the key exist
	regres = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\DirectX", NULL, KEY_READ, &hRegKey);
	if (regres == ERROR_SUCCESS) {
		// Read the key
		regres = RegQueryValueExA(hRegKey, "Version", 0, NULL, (LPBYTE)value, &dwSize);
		// Decode the string : 4.09.00.0904
		sscanf_s(value, "%d.%d.%d.%d", &major, &minor, &notused, &revision);
		RegCloseKey(hRegKey);
		if (major == 4 && minor == 9 && revision == 904)
			return true;
	}
	return false;

}


void ofApp::trim(char* s) {
	char* p = s;
	int l = (int)strlen(p);

	while (isspace(p[l - 1])) p[--l] = 0;
	while (*p && isspace(*p)) ++p, --l;

	memmove(s, p, l + 1);
}
