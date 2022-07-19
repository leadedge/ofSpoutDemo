/*

    Spout OpenFrameworks Demo program

    Spout 2.007
    OpenFrameworks 11.1
    Visual Studio 2022

    Copyright (C) 2020-2022 Lynn Jarvis.

    This program can be used to test Spout development code.
    See also GitHub user project (https://github.com/leadedge/ofSpoutDemo)

    Uses the ofxSkybox addon
    https://github.com/kalwalt/ofxSkyBox
    Created by kalwalt alias Walter Perdan on 27/12/11
    Copyright 2011 http://www.kalwaltart.it/ All rights reserved.

    Uses the ofxWinMenu addon
    https://github.com/leadedge/ofxWinMenu

	15.06.21 - Add image snapshot with F12 key for receiver
	16.06.21 - Add "Save image" and ""Open data folder" to receiver file menu
			   Auto create image name for F12
			   Create GitHub release
			   Version 1.007
	16.06.21 - Use Openframeworks timestamp format for F12 image name
	29.03.22 - Update to Openframeworks 11.1 and VS2022
			   Remove "Portable/Installation" check from About function
			   (2.006 installation no longer supported).
			   Remove unused "char fname[MAX_PATH]" from About functon.
			   resource.rc - Remove X offset for version number from About.
	28.04.22 - Add MessageBox warning if SpoutPanel is not found (Spout.cpp).
			   Rebuild x64 /MD. Provide required dlls in executable folder.
	           Version 1.008
	18.07.22 - Receiver allow for 16 bit sender textures and image save
			   Update SetClassLong to SetClassLongPtr
			   Changed F12 snapshot messagebox timeout from 4 to 2.5 seconds
			   Version 1.009

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

static INT_PTR CALLBACK UserSenderName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); // enter a sender name 
static PSTR szText;
static PSTR szCaption;

static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static int spoutVersion = 0;
static HINSTANCE g_hInstance;
static HWND g_hWnd;

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0, 0, 0);

#ifdef BUILDRECEIVER
	strcpy_s(senderName, 256, "Spout Receiver");
	// Graphics adapter index and name for about box
	adapterIndex = receiver.GetAdapter();
	receiver.GetAdapterName(adapterIndex, adapterName, 256);
#else
	strcpy_s(senderName, 256, "Spout Sender"); // The sender name
	adapterIndex = sender.GetAdapter();
	sender.GetAdapterName(adapterIndex, adapterName, 256);
#endif

	// Show the application title
	ofSetWindowTitle(senderName);
	// OpenSpoutConsole(); // Empty console for debugging
	// EnableSpoutLog(); // Log to console
	// SetSpoutLogLevel(SpoutLogLevel::SPOUT_LOG_WARNING); // Log warnings only
	// EnableSpoutLogFile(senderName); // Log to file

	// Instance
	g_hInstance = GetModuleHandle(NULL);

	// Openframeworks window handle
	g_hWnd = ofGetWin32Window();

	// Set a custom window icon
	SetClassLongPtrA(g_hWnd, GCLP_HICON, (LONG_PTR)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));

	// Load a font rather than the default
	if(!myFont.load("fonts/DejaVuSans.ttf", 12, true, true))
	  printf("ofApp error - Font not loaded\n");

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
#ifdef BUILDRECEIVER
	menu->AddPopupItem(hPopup, "Save image", false, false);
	menu->AddPopupItem(hPopup, "Open data folder", false, false);
#else
	menu->AddPopupItem(hPopup, "Sender name", false, false);
#endif
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
	bPreview = false;
	menu->AddPopupItem(hPopup, "Preview", false, false); // Unchecked, no auto-check
	bFullScreen = false;
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Unchecked, no auto-check
#endif
	bShowInfo = true;
	menu->AddPopupItem(hPopup, "Show info", true); // Checked, auto-check

	//
	// "Help" popup
	//
	hPopup = menu->AddPopupMenu(g_hMenu, "Help");
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

#ifdef BUILDRECEIVER
	// Allocate an RGBA texture to receive from the sender
	// It can be resized later to match the sender - see Update()
	myTexture.allocate(senderWidth, senderHeight, GL_RGBA);
#else
	// Create an RGBA fbo for texture transfers
	myFbo.allocate(senderWidth, senderHeight, GL_RGBA);
	// Give the sender a name (if no name is specified, the executable name is used)
	sender.SetSenderName(senderName);
#endif

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
		// Re-allocate the receving OpenGL texture to match the
		// dimensions and DirectX texture format of the sender
		// Allow for 8 and 16 bit textures
		DWORD dwFormat = receiver.GetSenderFormat();
		myTextureFormat = GL_RGBA; // default 8 bit
		switch (dwFormat) {
		case 10: // 16 bit float DXGI_FORMAT_R16G16B16A16_FLOAT
		case 11: // 16 bit unorm DXGI_FORMAT_R16G16B16A16_UNORM
		case 13: // 16 bit snorm DXGI_FORMAT_R16G16B16A16_SNORM
		case 24: // 10 bit unorm DXGI_FORMAT_R10G10B10A2_UNORM
			myTextureFormat = GL_RGBA16;
			break;
		default:
			break;

		}
		myTexture.allocate(receiver.GetSenderWidth(), receiver.GetSenderHeight(), myTextureFormat);
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

	std::string str;
	ofSetColor(255);

	// ReceiveTexture connects to and receives from a sender
	if (receiver.ReceiveTexture(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget)) {
		myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
	}

	if (bShowInfo && !bFullScreen && !bPreview) {

		if (receiver.IsConnected()) {

			ofSetColor(255);

			std::string str = "[";
			str += receiver.GetSenderName();
			str += "]  ";
			str += ofToString(receiver.GetSenderWidth()); str += "x";
			str += ofToString(receiver.GetSenderHeight()); str += " ";
			// GetSenderFrame() will return false for senders < 2.007
			// Frame counting can also be disabled in SpoutSettings
			if (receiver.GetSenderFrame() > 0) {
				str += " fps ";
				str += ofToString((int)roundf(receiver.GetSenderFps()));
				str += " frame  ";
				str += ofToString(receiver.GetSenderFrame());
			}
			else {
				// Show Openframeworks fps
				str += " fps : " + ofToString((int)roundf(ofGetFrameRate()));
			}
			myFont.drawString(str, 20, 30);
		}
		else {
			myFont.drawString("No sender detected", 20, 30);
		}

		// Is the receiver using CPU sharing ?
		if (receiver.GetCPUshare()) {
			str = "CPU share";
		}
		else {
			str = "Texture share";
			// Graphics can still be incompatible if the user
			// did not select "Auto" or "CPU" in SpoutSettings
			if (!receiver.IsGLDXready())
				str += " (Graphics not compatible)";
		}
		myFont.drawString(str, 20, 52);

		// Show the keyboard shortcuts
		str = "RH click sender : f - fullscreen : p - preview : Space - hide info";
		myFont.drawString(str, 20, ofGetHeight() - 20);

	} // endif show info

#else

	// - - - - - - - - - - - - - - - - 

	// Draw 3D graphics into the fbo
	myFbo.begin();

	// Clear to reset the background and depth buffer
	ofClear(0, 16, 64, 255);

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

	// Show what it's sending
	if (bShowInfo) {

		ofSetColor(255);
		std::string str = "Sending as : ";
		str += sender.GetName(); str += " (";
		str += ofToString(sender.GetWidth()); str += "x";
		str += ofToString(sender.GetHeight());
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
		str += ")";
		myFont.drawString(str, 20, 30);

		// Is the sender using CPU sharing?
		if (sender.GetCPUshare()) {
			str = "CPU share";
		}
		else {
			str = "Texture share";
			// Graphics can still be incompatible if the user
			// did not select "Auto" or "CPU" in SpoutSettings
			if (!sender.IsGLDXready())
				str += " (Graphics not compatible)";
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

	// ESC key (exit on ESC disabled)
	if (key == OF_KEY_ESC) {
		// Exit preview
		if (bPreview) {
			bPreview = false;
			doFullScreen(bPreview, true); // Disable preview
		}
		// Exit full screen
		else if (bFullScreen) {
			bFullScreen = false;
			doFullScreen(bFullScreen, false); // Disable full screen
		}
	}

	// Preview
	if (key == 'p' && !bFullScreen) {
		bPreview = !bPreview;
		doFullScreen(bPreview, true);
	}

	// Full screen
	if(key == 'f' && !bPreview) {
		bFullScreen = !bFullScreen;
		doFullScreen(bFullScreen, false);
	}

	// Snapshot - F12 key
	if (key == OF_KEY_F12) {
		if (receiver.IsConnected())
		{
			// Make a timestamped image file name
			std::string imagename = receiver.GetSenderName();

			// Save image to bin>data>captures
			std::string savepath = "captures\\";

			// Get pixels from received texture
			// Sender texture format determines image and file type
			if (myTextureFormat == GL_RGBA16) {
				ofShortImage myImage; // Bit depth 16 bits
				myTexture.readToPixels(myImage.getPixels());
				// TIFF image
				imagename += "_" + ofGetTimestampString() + ".tif";
				savepath += imagename;
				myImage.save(savepath);
			}
			else {
				ofImage myImage; // Bit depth 8 bits
				myTexture.readToPixels(myImage.getPixels());
				// PNG image
				imagename += "_" + ofGetTimestampString() + ".png";
				savepath += imagename;
				myImage.save(savepath);
			}

			// Spout timeout messagebox with 2 second delay
			char tmp[256];
			sprintf_s(tmp, "Saved : %s", savepath.c_str());
			SpoutMessageBox(NULL, savepath.c_str(), "Message", MB_OK, 2500);

}
		else {
			SpoutMessageBox(NULL, "No sender", "Message", MB_OK, 1500);
		}
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
#else
	UNREFERENCED_PARAMETER(w);
	UNREFERENCED_PARAMETER(h);
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

#ifdef BUILDRECEIVER

	if (title == "Save image") {
		if (receiver.IsConnected())
		{
			char imagename[MAX_PATH];
			imagename[0] = 0; // No existing name
			if (EnterSenderName(imagename, "Image name")) {

				std::string savepath = imagename;

				// Add default png extension if none entered
				std::size_t f = savepath.find('.');
				if (f == std::string::npos)
					savepath += ".png";

				// Only png and tif support 16 bit
				if (myTextureFormat == GL_RGBA16 && !(savepath.find(".png") || savepath.find(".tif"))) {
					savepath = savepath.substr(0, f);
					savepath += ".tif";
				}

				// Path for existence check
				savepath = ofToDataPath(savepath);
				if (PathFileExistsA(savepath.c_str())) {
					if (MessageBoxA(NULL, "Image file exists\nOverwrite ?", "Warning", MB_YESNO) == IDNO)
						return;
				}

				// Get pixels from received texture
				// Sender texture format determines image type
				if (myTextureFormat == GL_RGBA16) {
					ofShortImage myShortImage; // Bit depth 16 bits
					myTexture.readToPixels(myShortImage.getPixels());
					myShortImage.save(savepath);
				}
				else {
					ofImage myImage; // Bit depth 8 bits
					myTexture.readToPixels(myImage.getPixels());
					myImage.save(savepath);
				}
				char tmp[256];
				sprintf_s(tmp, "Saved : %s", savepath.c_str());
				SpoutMessageBox(NULL, savepath.c_str(), "Message", MB_OK, 2500);
			}
		}
		else {
			SpoutMessageBox(NULL, "No sender", "Message", MB_OK, 1500);
		}
	}

	if (title == "Open data folder") {
		std::string datapath = ofFilePath::getCurrentExeDir();
		datapath += "\\data";
		ShellExecuteA(ofGetWin32Window(), "open", datapath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

#else

	if (title == "Sender name") {
		char sendername[256]; // Name comparison
		strcpy_s(sendername, senderName);
		if (EnterSenderName(sendername, "Sender name")) {
			if (strcmp(sendername, senderName) != 0) {
				// Change to the user entered name
				strcpy_s(senderName, 256, sendername);
				// Release the current sender
				// SendTexture looks after sender creation for the new name
				sender.ReleaseSender();
				// SetSenderName handles duplicate names with "_1", "_2" etc. appended.
				sender.SetSenderName(senderName);
			}
		}
	}
#endif

	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//

	if (title == "Preview") {
		bPreview = !bPreview;
		doFullScreen(bPreview, true);
	}

	if (title == "Full screen") {
		bFullScreen = !bFullScreen;
		doFullScreen(bFullScreen, false);
	}

	if (title == "Show on top") {
		bTopmost = bChecked;
		if (bTopmost) {
			// get the current top window
			g_hwndForeground = GetForegroundWindow();
			SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			// CheckMenuItem(hmenu, IDM_TOPMOST, MF_BYCOMMAND | MF_CHECKED);
			ShowWindow(g_hWnd, SW_SHOW);
			g_hwndTopmost = g_hWnd;
		}
		else {
			SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			// CheckMenuItem(hmenu, IDM_TOPMOST, MF_BYCOMMAND | MF_UNCHECKED);
			ShowWindow(g_hWnd, SW_SHOW);
			// Reset the window that was top before
			if (GetWindowLong(g_hwndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST) {
				SetWindowPos(g_hwndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				g_hwndTopmost = g_hwndForeground;
			}
			else {
				SetWindowPos(g_hwndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			g_hwndTopmost = NULL;
			// hwndTopmost is also found with full screen / preview
		}
	}

	if (title == "Show info") {
		bShowInfo = bChecked;
	}


	//
	// Help menu
	//

	if (title == "About") {
		DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ABOUTBOX), g_hWnd, About);
	}

} // end appMenuFunction


void ofApp::doFullScreen(bool bEnable, bool bPreviewMode)
{
	RECT rectTaskBar;
	HWND hWndTaskBar = NULL;
	char tmp[256]; tmp[0] = 0;
	HWND hwndTopmost = NULL;

	// Avoid compiler warnings
	if (!g_hWnd)
		return;

	if (bEnable) { // Set to full screen or preview

		// g_hwndTopmost is set by user selection "Show on top"
		if (g_hwndTopmost) {
			hwndTopmost = g_hwndTopmost;
		}
		else {
			//
			// Find the current topmost window - if any.
			//
			// Get the first visible window in the Z order
			hwndTopmost = GetTopWindow(NULL);
			GetWindowTextA(hwndTopmost, (LPSTR)tmp, 256); // hwnd can be null
			do {
				if (hwndTopmost && tmp[0] && IsWindowVisible(g_hwndTopmost)
					&& GetWindowLong(hwndTopmost, GWL_EXSTYLE) & WS_EX_TOPMOST) {
					break;
				}
				else {
					if (hwndTopmost) {
						// Get next window
						hwndTopmost = GetNextWindow(hwndTopmost, GW_HWNDNEXT);
						if (hwndTopmost) {
							// If we got it, save the title
							GetWindowTextA(hwndTopmost, (LPSTR)tmp, 256);
						}
						else {
							break; // no more windows
						}
					}
					else {
						break;
					}
				}
			} while (hwndTopmost != NULL); // hwndTopmost is NULL if GetNextWindow finds no more windows
		}

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

		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;

		// PREVIEW
		if (bPreviewMode) {
			x = (int)windowRect.left
				+ GetSystemMetrics(SM_CXBORDER) * 2
				+ GetSystemMetrics(SM_CXFRAME)
				+ GetSystemMetrics(SM_CXDLGFRAME);
			y = (int)windowRect.top
				+ GetSystemMetrics(SM_CYCAPTION)
				+ GetSystemMetrics(SM_CYMENU)
				+ GetSystemMetrics(SM_CYBORDER) * 2
				+ GetSystemMetrics(SM_CYFRAME)
				+ GetSystemMetrics(SM_CYDLGFRAME);
			w = (int)(clientRect.right - clientRect.left);
			h = (int)(clientRect.bottom - clientRect.top);
		}
		else {
			// FULL SCREEN
			// If it's on a different monitor, make it fullscreen on that monitor.
			// Thanks to Tim Thompson for resolving this
			HMONITOR monitor = MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfoA(monitor, &mi);
			x = (int)mi.rcMonitor.left;
			y = (int)mi.rcMonitor.top;
			w = (int)(mi.rcMonitor.right - mi.rcMonitor.left); // rcMonitor dimensions are LONG
			h = (int)(mi.rcMonitor.bottom - mi.rcMonitor.top);
		}

		// Hide the window while re-sizing
		ShowWindow(g_hWnd, SW_HIDE);

		// hwndTopmost is either the g_hwndTopmost, user selection "Show on top", or found above
		if (hwndTopmost && g_hWnd == hwndTopmost) {
			SetWindowPos(g_hWnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
		}
		else {
			// Double SetWindowPos avoids a grey screen with Windows 10
			// if scaling is greater than 100%. This could be resolved eventually.
			SetWindowPos(g_hWnd, HWND_NOTOPMOST, x, y, w, h, SWP_SHOWWINDOW);
			SetWindowPos(g_hWnd, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
		}
		ShowCursor(FALSE);

		ShowWindow(g_hWnd, SW_SHOW);
		SetFocus(g_hWnd);

	}
	else { // Exit full screen or preview

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


bool ofApp::EnterSenderName(char *SenderName, char *caption)
{
	int retvalue = 0;
	unsigned char textin[256];
	unsigned char captin[256];

	// Caption 
	strcpy_s(szCaption = (PSTR)captin, 256, caption); // move to a static string for the dialog

	// Existing name
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
		// Set dialog caption
		SetWindowTextA(hDlg, szCaption);
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


// Message handler for About box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char tmp[MAX_PATH];
	// char fname[MAX_PATH];
	char about[1024];
	DWORD dummy, dwSize, dwVersion = 0;

	LPDRAWITEMSTRUCT lpdis;
	HWND hwnd = NULL;
	HCURSOR cursorHand = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	switch (message) {

	case WM_INITDIALOG:

		sprintf_s(about, 256, "Spout demo program\r\nVersion ");
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

		// Get the Spout version
		spoutVersion = 0;
		if (ReadDwordFromRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\Spout", "Version", &dwVersion))
			spoutVersion = (int)dwVersion; // 0 for earlier than 2.005

		if (spoutVersion > 0) {
			sprintf_s(tmp, 256, "Spout 2.00%1d\r\n", (spoutVersion - 2000));
			strcat_s(about, 1024, tmp);
		}
		else {
			sprintf_s(tmp, 256, "Spout 2.004 or earlier\r\n");
			strcat_s(about, 1024, tmp);
		}

		SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

		// Graphics adapter index and name
		sprintf_s(tmp, MAX_PATH, "%s\r\n", adapterName);
		SetDlgItemTextA(hDlg, IDC_ADAPTER_TEXT, (LPCSTR)tmp);

		// Hyperlink hand cursor
		cursorHand = LoadCursor(NULL, IDC_HAND);
		hwnd = GetDlgItem(hDlg, IDC_SPOUT_URL);
		SetClassLongA(hwnd, GCLP_HCURSOR, (LONGLONG)cursorHand);
		break;

	case WM_DRAWITEM:

		// The blue hyperlink
		lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID == -1) break;
		SetTextColor(lpdis->hDC, RGB(6, 69, 173));
		DrawTextA(lpdis->hDC, "https://spout.zeal.co", -1, &lpdis->rcItem, DT_LEFT);
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
