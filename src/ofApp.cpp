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
INT_PTR  CALLBACK AdapterProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static std::string adaptername[10];
static int adaptercount = 0;
static int currentadapter = 0;

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static int spoutVersion = 2004;
static HINSTANCE g_hInstance;


//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(10, 100, 140);

#ifdef BUILDRECEIVER
	strcpy_s(senderName, 256, "Spout Receiver");
#else
	strcpy_s(senderName, 256, "Spout Sender"); // The sender name
#endif
	ofSetWindowTitle(senderName); // show it on the title bar
	
	// OpenSpoutConsole(); // Empty console for debugging
	// EnableSpoutLog(); // Log to console
	// EnableSpoutLogFile(senderName); // Log to file

	// Instance
	g_hInstance = GetModuleHandle(NULL);

	// Openframeworks window handle
	g_hWnd = ofGetWin32Window();

	// Set a custom window icon
	SetClassLongA(g_hWnd, GCLP_HICON, (LONG)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));

	// Load a font rather than the default
	if (!myFont.load("fonts/verdana.ttf", 12, true, true))
		printf("Font not loaded\n");

	// Remove maximize button
	DWORD dwStyle = GetWindowLong(g_hWnd, GWL_STYLE);
	g_dwStyle = dwStyle ^= WS_MAXIMIZEBOX;
	SetWindowLong(g_hWnd, GWL_STYLE, g_dwStyle);

	// Disable escape key exit
	ofSetEscapeQuitsApp(false);

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
	// TODO : menu->AddPopupItem(hPopup, "Sender Name", false, false);
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
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check
	// TODO : diagnostics, documentation
	
	// Adjust window to desired client size allowing for the menu
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_BORDER, true);

	// Centre on the screen
	SetWindowPos(g_hWnd, HWND_TOP,
		(ofGetScreenWidth() - (rect.right - rect.left)) / 2,
		(ofGetScreenHeight() - (rect.bottom - rect.top)) / 2,
		(rect.right - rect.left), (rect.bottom - rect.top),	SWP_SHOWWINDOW);

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
	// Get the current graphics adapter name
	currentadapter = receiver.GetAdapter();
	receiver.GetAdapterName(currentadapter, name, 64);
	// Get the current user-set share mode from the registry
	// (0 - texture : 1 - memory : 2 - auto)
	shareMode = receiver.GetShareMode();
#else
	// Give the sender a name (if no name is specified, the executable name is used)
	sender.SetSenderName(senderName);
	// Get the current graphics adapter index and name
	currentadapter = sender.GetAdapter();
	sender.GetAdapterName(currentadapter, name, 64);
	// Get the current user-set share mode from the registry
	// (0 - texture : 1 - memory : 2 - auto)
	shareMode = sender.GetShareMode();
#endif

	// Set the current adapter name into the name list
	adaptername[currentadapter] = name;

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

	ofBackground(10, 100, 140);

#ifdef BUILDRECEIVER

	char text[256];
	std::string str;
	ofSetColor(255);

	// ReceiveTexture connects to and receives from a sender
	if(receiver.ReceiveTexture(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget))
		myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
	else
		ofClear(10, 100, 140, 255);

	if (bShowInfo) {

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
			myFont.drawString("RH click to select sender", 10, ofGetHeight() - 20);
		}
		else {
			myFont.drawString("No sender detected", 10, 30);
		}

		// The current graphics adapter name
		str = adaptername[currentadapter];

		// Auto share mode
		if (shareMode == 2)
			str += " : Auto share mode ";
		else
			str += " : User share mode ";

		// Get the current sharing mode (changes with graphics compatibility for Auto share mode)
		// (GetMemoryShareMode reads the user registry setting instead)
		if (receiver.GetMemoryShare())
			str += "(Memory)";
		else
			str += "(Texture)";

		myFont.drawString(str, 10, 52);

	} // endif show info

#else

	ofClear(0, 0, 0, 0);

	// - - - - - - - - - - - - - - - - 

	// Draw 3D graphics into the fbo
	myFbo.begin();

	// Clear to reset the background and depth buffer
	ofClear(0, 0, 0);

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

		// Get the current graphics adapter name
		str = adaptername[currentadapter];

		// Show the sharing mode
		switch (shareMode) {
			case 0 :
				str += " : Texture share mode ";
				break;
			case 1 :
				str += " : Memory share mode ";
				break;
			default :
				str += " : Auto share mode ";
				
				//
				// Get the current sharing mode using GetMemoryShare()
				// This changes according to graphics compatibility for Auto share mode
				// GetMemoryShareMode() and GetShareMode() read user-selected mode from the registry
				//
				if (sender.GetMemoryShare())
					str += " : Memory";
				else
					str += " : Texture";
				break;
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

#ifdef BUILDRECEIVER
	if (button == 2) { // rh button
		// Open the sender selection panel
		// Spout must have been installed
		receiver.SelectSender();
	}
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

	// TODO : fill out

	//
	// File menu
	//

	if (title == "Sender Name") {
		// 
	}

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
	if (title == "Information") {
		string text = "Spout demo program\n";
		MessageBoxA(NULL, text.c_str(), "Information", MB_OK | MB_TOPMOST);
	}


	if (title == "About") {
		DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ABOUTBOX), g_hWnd, About);
	}

	/*
	if (title == "Documentation") {
		char path[MAX_PATH];
		HMODULE hModule = GetModuleHandle(NULL);
		GetModuleFileNameA(hModule, path, MAX_PATH);
		PathRemoveFileSpecA(path);
		strcat_s(path, MAX_PATH, "\\SpoutLink.pdf");
		ShellExecuteA(g_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}
	*/

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
		} while (g_hwndTopmost != NULL);
		// g_hwndTopmost is NULL if GetNextWindow finds no more windows

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



void ofApp::SelectAdapter()
{
	char name[64];
	int retvalue = -1;
	bool bRet = false;
	int oldmode = 0;

	// Save the old adapter index
	int oldadapter = currentadapter;
	adaptername->clear();

	#ifdef BUILDRECEIVER
		currentadapter = receiver.GetAdapter();
		oldadapter = currentadapter; // to test for change
		adaptercount = receiver.GetNumAdapters();
		for (int i = 0; i < adaptercount; i++) {
			receiver.GetAdapterName(i, name, 64);
			adaptername[i] = name;
		}
	#else
		currentadapter = sender.GetAdapter();
		// oldadapter = currentadapter; // to test for change
		adaptercount = sender.GetNumAdapters();
		for (int i = 0; i < adaptercount; i++) {
			sender.GetAdapterName(i, name, 64);
			adaptername[i] = name;
		}
	#endif
	// printf("SelectAdapter() : currentadapter = %d : oldadapter = %d\n", currentadapter, oldadapter);

	// Show the dialog box 
	retvalue = (int)DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ADAPTERBOX), g_hWnd, (DLGPROC)AdapterProc);
	// OK (1) - a new adapter index (currentadapter) has been selected

	// printf("retvalue = %d : currentadapter = %d : oldadapter = %d\n", retvalue, currentadapter, oldadapter);
	if (retvalue != 0) {
#ifdef BUILDRECEIVER
		// A new sender will be detected on the first ReceiveTexture call (Requires 2.007)
		receiver.ReleaseReceiver();
		// Set the selected adapter if different
		if (currentadapter != oldadapter) {
			if (!receiver.SetAdapter(currentadapter)) {
				MessageBoxA(NULL, "Could not select graphics adapter", "Error", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
				currentadapter = oldadapter;
				receiver.SetAdapter(currentadapter);
			}
		}
#else
		// A new sender will be created on the first SendTexture call (Requires 2.007)
		sender.ReleaseSender();
		// Set the selected adapter if different
		if (currentadapter != oldadapter) {
			if (!sender.SetAdapter(currentadapter)) {
				MessageBoxA(NULL, "Could not select graphics adapter", "Error", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
				currentadapter = oldadapter;
				sender.SetAdapter(currentadapter);
			}
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
	int old = currentadapter;

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
			currentadapter = (int)SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		}
		// Drop through

		switch (LOWORD(wParam)) {

		case IDOK:
			// Keep the adapter index selection
			EndDialog(hDlg, 1);
			break;

		case IDCANCEL:
			// User pressed cancel.
			// Set the adapter index back to what it was and take down dialog box.
			currentadapter = old;
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
	char about[1024];
	DWORD dummy, dwSize;
	LPDRAWITEMSTRUCT lpdis;
	HWND hwnd = NULL;
	HCURSOR cursorHand = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	switch (message) {

	case WM_INITDIALOG:

		// TODO : use windowtitle
		sprintf_s(about, 256, "Spout demo program - Version ");
		// Get product version number
		if (GetModuleFileNameA(hInstance, tmp, MAX_PATH)) {
			dwSize = GetFileVersionInfoSizeA(tmp, &dummy);
			if (dwSize > 0) {
				vector<BYTE> data(dwSize);
				if (GetFileVersionInfoA(tmp, NULL, dwSize, &data[0])) {
					LPVOID pvProductVersion = NULL;
					unsigned int iProductVersionLen = 0;
					if (VerQueryValueA(&data[0], ("\\StringFileInfo\\080904E4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
						sprintf_s(tmp, MAX_PATH, "%s\n", (char *)pvProductVersion);
						strcat_s(about, 1024, tmp);
					}
				}
			}
		}
		strcat_s(about, 1024, "\n\n");
		SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

		//
		// Hyperlink hand cursor
		//
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
