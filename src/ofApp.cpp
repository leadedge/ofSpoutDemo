/*

    Spout OpenFrameworks Demo program

    Spout 2.007
    OpenFrameworks 11.1
    Visual Studio 2022

    Copyright (C) 2020-2023 Lynn Jarvis.

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
	30.11.22 - Averaging to stabilise fps display
			   DoFullScreen :
				 Removed TaskBar Z-order change
				 Fix for slow rendering if resized exactly to monitor extents
			   Test receiver connected before fullscreen of preview
	09.12.22 - AboutBox 
			     Correct Spout version for latest major/minor/release
				 numbering in registry. Use GetSpoutSDK string instead.
			     Change SetClassLong to SetClassLongPtr
			   Code cleanup
			   Version 1.010
	05.08.23 - Options for OpenGL sending and receiving format
	12.08.23 - Video recording with options for audio/rgb/prompt
	13.08.23 - Add ini file to save/restore settings
			   Capture menu with options for capture and image type
			   Version 1.011
	14.08.23 - Replace capture and recording menu options Options dialog box
			   Add View Menu for capture and recording folders
			   Version 1.012
	22.08.23 - Change from F9/F10 to F1/F2 due to WM_SYSCOMMAND message for F10
	26.08.23 - Change from SpoutRecorder command line to SpoutRecord class
			   Remove rgb option. No benefit after testing.
			   Remove View menu and add to Window menu
			   Remove "Video folder" item if disabled
			   Add recording time display
			   Version 1.013
	02.09.23 - Receiver - add h264 quality and preset recording options
			   Version 1.014


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

#ifdef BUILDRECEIVER
static INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); // enter a sender name 
static char g_ExePath[MAX_PATH]={0}; // Executable path
static std::string extension = ".png"; // Capture type

static bool bPrompt = false; // file name prompt
static bool bDuration = false; // record for fixed duration
static long SecondsDuration = 0L; // time to record
static bool bAudio = false; // system audio
static int codec = 0; // mpg4 / h264
static int quality = 0; // h264 CRF
static int preset = 0; // h264 preset
static int presetindex = 0; // for the combobox

// For cancel
static std::string old_extension = extension;
static bool old_bPrompt = bPrompt;
static bool old_bDuration = bDuration;
static long old_SecondsDuration = SecondsDuration;
static bool old_bAudio = bAudio;
static int old_codec = codec;
static int old_quality = quality;
static int old_preset = preset;

#endif


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
	// printf("ofSpoutDemo\n");

	// Instance
	g_hInstance = GetModuleHandle(NULL);

	// Openframeworks window handle
	g_hWnd = ofGetWin32Window();

	// Set a custom window icon
	SetClassLongPtrA(g_hWnd, GCLP_HICON, (LONG_PTR)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));

	// Load a font rather than the default
	if(!myFont.load("fonts/DejaVuSans.ttf", 12, true, true))
	// if (!myFont.load("fonts/Verdana.ttf", 12, true, true))
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
	menu->AddPopupItem(hPopup, "Save video", false, false);
#else
	menu->AddPopupItem(hPopup, "Sender name", false, false);
	menu->AddPopupSeparator(hPopup);
#endif
	// Final File popup menu item is "Exit"
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// "Window" popup
	//
	hPopup = menu->AddPopupMenu(g_hMenu, "Window");
	bTopmost = false;
	menu->AddPopupItem(hPopup, "Show on top"); // Unchecked, auto check
	bShowInfo = true;
#ifdef BUILDRECEIVER
	bPreview = false;
	menu->AddPopupItem(hPopup, "Preview", false, false); // Unchecked, no auto-check
	bFullScreen = false;
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Unchecked, no auto-check
	menu->AddPopupItem(hPopup, "Show info", true); // Checked, auto-check
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Capture folder", false, false);
	// The "Video folder" item is removed from the Window menu by ReadInitFile
	// if recording is not enabled and removed or added after the Options dialog
	menu->AddPopupItem(hPopup, "Video folder", false, false);
#else
	menu->AddPopupItem(hPopup, "Show info", true); // Checked, auto-check
#endif

	//
	// "Help" popup
	//
	hPopup = menu->AddPopupMenu(g_hMenu, "Help");
#ifdef BUILDRECEIVER
	menu->AddPopupItem(hPopup, "Options", false, false); // No auto check
#endif
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

	// Application OpenGL format
	// Sender   : see SetSenderFormat below
	// Receiver : see ReceiveTexture/IsUpdated and Snapshot (F12 key)
	glFormat = GL_RGBA;

#ifdef BUILDRECEIVER
	// Allocate an RGBA texture to receive from the sender
	// It is re-allocated later to match the size and format of the sender.
	myTexture.allocate(senderWidth, senderHeight, glFormat);

	// Executable path
	GetModuleFileNameA(NULL, g_ExePath, MAX_PATH);
	PathRemoveFileSpecA(g_ExePath);

	// FFmpeg path
	g_FFmpegPath = g_ExePath;
	g_FFmpegPath +="\\DATA\\FFMPEG\\ffmpeg.exe";

	// Does FFmpeg.exe exist ?
	if (_access(g_FFmpegPath.c_str(), 0) == -1) {
		g_FFmpegPath.clear(); // disable functions using FFmpeg
	}

	// SpoutReceiver.ini file path
	strcpy_s(g_Initfile, MAX_PATH, g_ExePath);
	strcat_s(g_Initfile, MAX_PATH, "\\Spout Receiver.ini");

	// Read recording and menu settings
	ReadInitFile(g_Initfile);


#else
	// ---------------------------------------------------------------------------
	//
	// Option
	//
	// Set the sender application OpenGL format
	//       OpenGL                             Compatible DX11 format
	//       GL_RGBA16    16 bit				(DXGI_FORMAT_R16G16B16A16_UNORM)			
	//       GL_RGBA16F   16 bit float			(DXGI_FORMAT_R16G16B16A16_FLOAT)
	//       GL_RGBA32F   32 bit float			(DXGI_FORMAT_R32G32B32A32_FLOAT)
	//       GL_RGB10_A2  10 bit 2 bit alpha	(DXGI_FORMAT_R10G10B10A2_UNORM)
	//       GL_RGBA       8 bit                (DXGI_FORMAT_R8G8B8A8_UNORM)
	//
	glFormat = GL_RGB16; // Example 16 bit rgba
	//
	// Set a compatible DirectX 11 shared texture format for the sender
	// so that receivers get a texture with the same format.
	// Note that some applications may not receive other formats.
	sender.SetSenderFormat(sender.DX11format(glFormat));
	//
	// ---------------------------------------------------------------------------

	// Create an fbo for texture transfers
	myFbo.allocate(senderWidth, senderHeight, glFormat);

	// Give the sender a name (if no name is specified, the executable name is used)
	sender.SetSenderName(senderName);
#endif

	// Starting value for sender fps display
	g_SenderFps = GetRefreshRate();

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


#ifndef BUILDRECEIVER

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
		myFbo.allocate(senderWidth, senderHeight, glFormat);
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
			
		// If IsUpdated() returns true, the sender size has changed
		// and the receiving texture must be re-allocated.
		if (receiver.IsUpdated()) {

			// Allocate an application texture with the same OpenGL format as the sender
			// The received texture format (glFormat) determines image capture bit depth and type
			glFormat = receiver.GLDXformat();
			myTexture.allocate(receiver.GetSenderWidth(), receiver.GetSenderHeight(), glFormat);

			// Has the sender name changed ?
			if (strcmp(receiver.GetSenderName(), senderName) != 0) {
				// Stop recording
				StopRecording();
			}

			// Has the sender size changed ?
			if (senderWidth != receiver.GetSenderWidth() || senderHeight != receiver.GetSenderHeight()) {
				StopRecording();
			}

			strcpy_s(senderName, 256, receiver.GetSenderName());
			senderWidth = receiver.GetSenderWidth();
			senderHeight = receiver.GetSenderHeight();

		}

		if (bRecording) {
			recorder.Write(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget, senderWidth, senderHeight);
			ElapsedRecordingTime = (ElapsedMicroseconds()-StartRecordingTime)/1000000.0; // seconds
			// Check for fixed duration
			if (bDuration) {
				if (ElapsedRecordingTime > (double)SecondsDuration) {
					StopRecording();
				}
			}
		}
	}

	myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Show on-screen details
	if (bShowInfo && !bFullScreen && !bPreview) {
		if (receiver.IsConnected()) {
			ofSetColor(255);
			str = "[";
			str += receiver.GetSenderName();
			str += "]  (";
			str += ofToString(receiver.GetSenderWidth()); str += "x";
			str += ofToString(receiver.GetSenderHeight()); str += " - ";
			// Received texture OpenGL format
			str += receiver.GLformatName(receiver.GLDXformat());
			str += ")";
			myFont.drawString(str, 20, 30);
			// GetSenderFrame() will return false for senders < 2.007
			// Frame counting can also be disabled in SpoutSettings
			str.clear();
			if (receiver.GetSenderFrame() > 0) {
				str = "fps ";
				str += ofToString((int)roundf(receiver.GetSenderFps()));
				str += " frame  ";
				str += ofToString(receiver.GetSenderFrame());
			}
			else {
				// Show Openframeworks fps
				str += "fps : " + ofToString((int)roundf(ofGetFrameRate()));
			}

			// Is the receiver using CPU sharing ?
			if (receiver.GetCPUshare()) {
				str += " - CPU share";
			}
			else {
				str += " - Texture share";
				// Graphics can still be incompatible if the user
				// did not select "Auto" or "CPU" in SpoutSettings
				if (!receiver.IsGLDXready())
					str += " - Graphics not compatible)";
			}
			myFont.drawString(str, 20, 52);

			// Show action keys
			if (!bRecording) {
				str = "F1 - start recording : F12 - capture";
				myFont.drawString(str, 170, ofGetHeight() - 36);
			}
			else {
				str = "F2 - stop recording";
				myFont.drawString(str, 170, ofGetHeight() - 36);
			}

			// Show keyboard shortcuts
			str = "RH click - select sender : f - fullscreen : p - preview : Space - hide info";
			myFont.drawString(str, 40, ofGetHeight() - 14);

			// Show recording status
			if (bRecording) {
				ofSetColor(255, 255, 0);
				// Format as hh:mm:ss
				int total = (int)ElapsedRecordingTime;
				int hrs   = total/3600;
				int mins  = (total % 3600)/60;
				int secs  = (total % 60);
				int tick  = (int)((ElapsedRecordingTime-(double)total)*10.0);
				char tmp[256]={};
				sprintf_s(tmp, 256, "(%2.2d:%2.2d:%2.2d.%.1d)", hrs, mins, secs, tick);
				myFont.drawString(tmp, ofGetWidth()-300, ofGetHeight()-36);
				ofSetColor(255, 255, 255);
			}
		}
		else {
			myFont.drawString("No sender detected", 20, 30);
		}

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

	// Send fbo while bound
	// The fbo texture is already inverted so set the invert option false
	sender.SendFbo(myFbo.getId(), (unsigned int)myFbo.getWidth(), (unsigned int)myFbo.getHeight(), false);

	myFbo.end();

	// - - - - - - - - - - - - - - - - 

	// Draw the result in the fbo sized to the application window
	myFbo.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Show what it's sending
	if (bShowInfo) {

		ofSetColor(255);
		std::string str = "Sending as : ";
		str += sender.GetName(); str += " (";
		str += ofToString(sender.GetWidth()); str += "x";
		str += ofToString(sender.GetHeight()); str += ") ";
		// Sender OpenGL texture format description
		// for 16 bit and floating point types
		GLint glformat = sender.GLDXformat();
		if(glformat != GL_RGBA)
			str += sender.GLformatName(sender.GLDXformat());
		myFont.drawString(str, 20, 30);

		// Is the sender using CPU sharing?
		str.clear();
		if (sender.GetCPUshare()) {
			str = "CPU share";
		}
		else {
			str = "Texture share";
			// Graphics can still be incompatible if the user
			// did not select "Auto" or "CPU" in SpoutSettings
			if (!sender.IsGLDXready())
				str = "(Graphics not compatible)";
		}

		// Show sender fps and framecount if selected
		if (sender.GetFrame() > 0) {
			str = "fps ";
			// Average to stabilise fps display
			g_SenderFps = g_SenderFps*.85 + 0.15*sender.GetFps();
			// Round first or integer cast will truncate to the whole part
			str += ofToString((int)(round(g_SenderFps)));
			str += " : frame  ";
			str += ofToString(sender.GetFrame());
		}
		else {
			// Show Openframeworks fps
			str = "fps : " + ofToString((int)roundf(ofGetFrameRate()));
		}
		str += " ";
		myFont.drawString(str, 20, 52);

		// Show the keyboard shortcuts
		str = "Space - hide info";
		myFont.drawString(str, 10, ofGetHeight() - 20);

	} // endif show info

#endif

} // end Draw

//--------------------------------------------------------------
void ofApp::exit() {

	// Close the sender or receiver on exit
#ifdef BUILDRECEIVER
	StopRecording();
	receiver.ReleaseReceiver();
	// Save capture and recording settings
	WriteInitFile(g_Initfile);
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
		// No select while recording from this window
		if (!bRecording) {
			// Open the sender selection panel
			// SpoutSettings must have been run at least once
			receiver.SelectSender();
		}
		else {
			SpoutMessageBox(NULL, "No sender selection while recording", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 2000);
		}
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
		if (receiver.IsConnected()) {
			bPreview = !bPreview;
			doFullScreen(bPreview, true);
		}
	}

	// Full screen
	if (key == 'f' && !bPreview) {
		if (receiver.IsConnected()) {
			bFullScreen = !bFullScreen;
			doFullScreen(bFullScreen, false);
		}
	}

	if (receiver.IsConnected() && !bFullScreen) {

		// Recording - F1 / F2 keys
		if (key == OF_KEY_F1) {
			StartRecording(bPrompt);
		}

		if (key == OF_KEY_F2) {
			StopRecording();
		}
		// Snapshot - F12 key
		if (key == OF_KEY_F12) {
			appMenuFunction("Capture image", false);
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


#ifdef BUILDRECEIVER

	//
	// File Menu
	//

	if (title == "Save image") {
		if (receiver.IsConnected())
		{
			char imagename[MAX_PATH];
			imagename[0] = 0; // No existing name
			if (EnterSenderName(imagename, "Enter image name and extension")) {
				std::string name = imagename;
				// Ad an extension if none entered
				std::size_t found = name.find('.');
				if (found == std::string::npos)
					name += ".png";
				ofImage myImage;
				myTexture.readToPixels(myImage.getPixels());
				// Save to bin>data
				myImage.save(name);
			}
		}
		else {
			SpoutMessageBox(NULL, "No sender", "Spout Receiver", MB_OK | MB_TOPMOST, 1400);
		}
	}

	if (title == "Save video") {
		if (receiver.IsConnected())	{
			StartRecording(true);
		}
		else {
			SpoutMessageBox(NULL, "No sender", "Spout Receiver", MB_OK | MB_TOPMOST, 1400);
		}
	}
	
	//
	// View menu
	//

	if (title == "Capture folder") {
		std::string datapath = ofFilePath::getCurrentExeDir();
		datapath += "\\data\\captures";
		ShellExecuteA(ofGetWin32Window(), "open", datapath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	if (title == "Video folder") {
		std::string datapath = ofFilePath::getCurrentExeDir();
		datapath += "\\data\\videos";
		ShellExecuteA(ofGetWin32Window(), "open", datapath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	// Key commands
	// Snapshot - F12 key
	if (title == "Capture image") {
		if (receiver.IsConnected() && !bFullScreen)	{
			// Make a timestamped image file name
			std::string imagename = receiver.GetSenderName();
			imagename += "_" + ofGetTimestampString() + extension;

			// Save image to bin>data>captures
			std::string savepath = "captures\\";
			savepath += imagename;

			// Get pixels from received texture
			// Sender texture format determines image type
			// 16 or 8 bit depth, png or tif
			// The application texture format is set in ReceiveTexture/IsUpdated
			if (glFormat == GL_RGBA16
				|| glFormat == GL_RGBA16F
				|| glFormat == GL_RGBA32F) {
				ofShortImage myImage; // Bit depth 16 bits
				myTexture.readToPixels(myImage.getPixels());
				myImage.save(savepath); // save png or tif
			}
			else if (glFormat == GL_RGBA) {
				ofImage myImage; // Bit depth 8 bits
				myTexture.readToPixels(myImage.getPixels());
				myImage.save(savepath); // save png or tif
			}
			else {
				SpoutMessageBox(NULL, "Unsupported format", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 2000);
				return;
			}
			SpoutMessageBox(NULL, imagename.c_str(), "Saved image", MB_OK | MB_TOPMOST, 2000); // 2 second timeout
		}
		else {
			SpoutMessageBox(NULL, "No sender\nImage capture not possible", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 2000);
		}
	}

#else

	//
	// File menu
	//
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
#ifdef BUILDRECEIVER
		StopRecording();
#endif
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//

#ifdef BUILDRECEIVER
	if (title == "Preview") {
		if (receiver.IsConnected()) {
			bPreview = !bPreview;
			doFullScreen(bPreview, true);
		}
	}

	if (title == "Full screen") {
		if (receiver.IsConnected()) {
			bFullScreen = !bFullScreen;
			doFullScreen(bFullScreen, false);
		}
	}
#endif

	if (title == "Show on top") {
		bTopmost = bChecked;
		if (bTopmost) {
			// get the current top window
			g_hwndForeground = GetForegroundWindow();
			SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			menu->SetPopupItem("Show on top", true);
			ShowWindow(g_hWnd, SW_SHOW);
			g_hwndTopmost = g_hWnd;
		}
		else {
			SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			menu->SetPopupItem("Show on top", false);
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
#ifdef BUILDRECEIVER
	if (title == "Options") {

		old_extension = extension;
		old_bPrompt = bPrompt;
		old_bDuration = bDuration;
		old_SecondsDuration = SecondsDuration;
		old_bAudio = bAudio;
		old_codec = codec;
		old_quality = quality;
		old_preset = preset;

		if (!DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_OPTIONSBOX), g_hWnd, Options)) {
			extension = old_extension;
			bPrompt = old_bPrompt;
			bDuration = old_bDuration;
			SecondsDuration = old_SecondsDuration;
			bAudio = old_bAudio;
			codec = old_codec;
			quality = old_quality;
			preset = old_preset;
		}
	}
#endif

	if (title == "About") {
		DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_ABOUTBOX), g_hWnd, About);
	}

} // end appMenuFunction


void ofApp::doFullScreen(bool bEnable, bool bPreviewMode)
{
	char tmp[256]={};
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

		int x = 0; int y = 0; int w = 0; int h = 0;

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

		// Hide the window while re-sizing to avoid a flash effect
		ShowWindow(g_hWnd, SW_HIDE);

		// For reasons unknown, rendering is slow if the window is resized
		// to the monitor extents. Making it 1 pixel larger seems to fix it.
		SetWindowPos(g_hWnd, HWND_TOPMOST, x-1, y-1, w+2, h+2, SWP_SHOWWINDOW);

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

#ifdef BUILDRECEIVER
// Recording start - F1
bool ofApp::StartRecording(bool prompt)
{
	// Return if not receiving, already recording, preview or full screen
	if (!receiver.IsConnected() || bRecording || bPreview || bFullScreen)
		return false;

	if (prompt) {
		std::string str = EnterVideoName();
		if (str.empty())
			return false;
		g_OutputFile = str;
	}
	else {
		// Default output file
		g_OutputFile = g_ExePath; // exe folder
		g_OutputFile += "\\data\\videos\\";
		g_OutputFile += (char*)senderName;
		if (codec == 0)
			g_OutputFile += ".mp4";
		else
			g_OutputFile += ".mkv";
	}

	// Options for audio, codec and fps
	recorder.EnableAudio(bAudio); // For recording system audio
	recorder.SetCodec(codec); // 0-mpeg4, 1-x264 codec
	recorder.SetFps(30); // Fps for FFmpeg (see HoldFps)

	if (recorder.Start(g_FFmpegPath, g_OutputFile, receiver.GetSenderWidth(), receiver.GetSenderHeight(), true)) { // RGBA pixel format
		bRecording = true;
		StartRecordingTime = ElapsedMicroseconds();
		return true;
	}
	else {
		SpoutMessageBox(NULL, "FFmpeg not found", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST);
	}

	return false;
}


// Recording stop - F2
void ofApp::StopRecording()
{
	if (bRecording) {

		recorder.Stop();

		// Show a messagebox with video file details
		if (bPrompt) {

			// Video file name without full path
			char filename[MAX_PATH]={};
			strcpy_s(filename, MAX_PATH, g_OutputFile.c_str());
			PathStripPathA(filename);
			std::string msgstr = filename;

			// Duration
			char tmp[MAX_PATH]={};
			sprintf_s(tmp, MAX_PATH, " (%.2f seconds)\n\n", (float)ElapsedRecordingTime);
			msgstr += tmp;

			// Settings
			if (codec == 0) {
				msgstr += "mpeg4 codec, ";
			}
			else {
				msgstr += "x264 codec, ";
				if (quality == 0)
					msgstr += "low quality, ";
				if (quality == 1)
					msgstr += "medium quality, ";
				if (quality == 2)
					msgstr += "high quality, ";
				if (preset == 0)
					msgstr += "ultrafast, ";
				if (preset == 1)
					msgstr += "superfast, ";
				if (preset == 2)
					msgstr += "veryfast, ";
				if (preset == 3)
					msgstr += "faster, ";
			}
			if (bAudio)
				msgstr += "system audio";
			else
				msgstr += "no audio\n";
			SpoutMessageBox(NULL, msgstr.c_str(), "Saved video file", MB_OK | MB_ICONWARNING, 2000);
		}
	}

	bRecording = false;
	StartRecordingTime = 0.0;

}

//
// User file name entry
//
std::string ofApp::EnterVideoName()
{
	char filePath[MAX_PATH]={0};
	sprintf_s(filePath, MAX_PATH, g_OutputFile.c_str());

	OPENFILENAMEA ofn={};
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	HWND hwnd = NULL;
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.nMaxFileTitle = 31;
	// if ofn.lpstrFile contains a path, that path is the initial directory.
	ofn.lpstrFile = (LPSTR)filePath;
	ofn.nMaxFile = MAX_PATH;
	if (codec == 1)
		ofn.lpstrFilter = "Matroska (*.mkv)\0*.mkv\0Mpeg-4 (*.mp4)\0*.mp4\0Audio Video Interleave (*.avi)\0*.avi\0Quicktime (*.mov)\0*.mov\0All files (*.*)\0*.*\0";
	else
		ofn.lpstrFilter = "Mpeg-4 (*.mp4)\0*.mp4\0Matroska (*.mkv)\0*.mkv\0Audio Video Interleave (*.avi)\0*.avi\0Quicktime (*.mov)\0*.mov\0All files (*.*)\0*.*\0";
	ofn.lpstrDefExt = "";
	// OFN_OVERWRITEPROMPT prompts for over-write
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrTitle = "Output File";
	if (!GetSaveFileNameA(&ofn)) {
		// FFmpeg has not been started yet, return to try again
		return "";
	}
	// User file name entry
	return filePath;
}


//--------------------------------------------------------------
// Save a configuration file in the executable folder
// Ini file is created if it does not exist
void ofApp::WriteInitFile(const char* initfile)
{
	char tmp[MAX_PATH]={0};

	// Capture options
	if (extension == ".tif")
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Imagetype", (LPCSTR)".tif", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Imagetype", (LPCSTR)".png", (LPCSTR)initfile);
	
	// Recording options
	if (bPrompt)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Prompt", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Prompt", (LPCSTR)"0", (LPCSTR)initfile);

	if (bDuration)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Duration", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Duration", (LPCSTR)"0", (LPCSTR)initfile);

	sprintf_s(tmp, MAX_PATH, "%8ld", SecondsDuration);
	WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Seconds", (LPCSTR)tmp, (LPCSTR)initfile);

	if (bAudio)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Audio", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Audio", (LPCSTR)"0", (LPCSTR)initfile);

	if (codec == 1)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Codec", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Codec", (LPCSTR)"0", (LPCSTR)initfile);

	if (quality == 0)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Quality", (LPCSTR)"0", (LPCSTR)initfile);
	else if (quality == 1)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Quality", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Quality", (LPCSTR)"2", (LPCSTR)initfile);

	sprintf_s(tmp, MAX_PATH, "%d", preset);
	WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Preset", (LPCSTR)tmp, (LPCSTR)initfile);

	// Menu options
	if (bShowInfo)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Info", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Info", (LPCSTR)"0", (LPCSTR)initfile);

	if (bTopmost)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Topmost", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Topmost", (LPCSTR)"0", (LPCSTR)initfile);

}


//--------------------------------------------------------------
// Read back settings from configuration file
void ofApp::ReadInitFile(const char* initfile)
{
	char tmp[MAX_PATH]={0};

	// Capture options
	extension = ".png";
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Imagetype", NULL, (LPSTR)tmp, 5, initfile);
	if (tmp[0]) extension = tmp;
	
	// Recording options
	bPrompt = false; bDuration=false; SecondsDuration=0L;
	bAudio = false; codec = 0; quality = 1; preset = 0;
	
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Prompt", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bPrompt = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Duration", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bDuration = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Seconds", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) SecondsDuration = atol(tmp);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Audio", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bAudio = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Codec", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) codec = atoi(tmp);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Quality", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) quality = atoi(tmp);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Preset", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) preset = atoi(tmp);

	// Menu options
	bShowInfo = true; bTopmost = false;
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Info", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bShowInfo = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Topmost", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bTopmost = (atoi(tmp) == 1);

	// Check menu items
	if (extension == ".tif") {
		menu->SetPopupItem("tif", true);
		menu->SetPopupItem("png", false);
	}
	else {
		menu->SetPopupItem("tif", false);
		menu->SetPopupItem("png", true);
	}
	if (bShowInfo)
		menu->SetPopupItem("Show info", true);
	else
		menu->SetPopupItem("Show info", false);

	// Set topmost or not
	appMenuFunction("Show on top", bTopmost);

}
#endif

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


// Message handler for sender name entry
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

#ifdef BUILDRECEIVER
// Message handler for options
INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndList = NULL;
	HWND hwndUpDnCtl = NULL;
	UINT nCode = 0;
	int count = 0;
	LPNMUPDOWN lpnmud={0};
	char tmp[256]={0};
	char presets[4][128] ={ "Ultrafast", "Superfast", "Veryfast", "Faster" };

	switch (message) {

	case WM_INITDIALOG:
		{
			// Capture type - png/tif
			int iPos = 0;
			if (extension == ".tif") iPos = 1; // 0-png, 1-tif
			CheckRadioButton(hDlg, IDC_PNG, IDC_TIF, IDC_PNG+iPos);
			// File name prompt
			CheckDlgButton(hDlg, IDC_PROMPT, (UINT)bPrompt);
			// Record for duration
			CheckDlgButton(hDlg, IDC_DURATION, (UINT)bDuration);
			// Codec - 0-mpeg4, 1-x264
			CheckRadioButton(hDlg, IDC_MPEG4, IDC_X264, IDC_MPEG4+codec);
			// System audio
			CheckDlgButton(hDlg, IDC_AUDIO, (UINT)bAudio);
			// SecondsDuration
			sprintf_s(tmp, "%ld", SecondsDuration);
			SetDlgItemTextA(hDlg, IDC_SECONDS, (LPCSTR)tmp);
			// Quality
			CheckRadioButton(hDlg, IDC_QLOW, IDC_QHIGH, IDC_QLOW+(int)quality);
			// Presets
			hwndList = GetDlgItem(hDlg, IDC_PRESET);
			SendMessageA(hwndList, (UINT)CB_RESETCONTENT, 0, 0L);
			for (int k = 0; k < 4; k++)
				SendMessageA(hwndList, (UINT)CB_ADDSTRING, 0, (LPARAM)presets[k]);
			presetindex = preset;
			SendMessageA(hwndList, CB_SETCURSEL, (WPARAM)presetindex, (LPARAM)0);
			// Disable h264 options for mpeg4
			if (codec == 0) {
				// Quality
				EnableWindow(GetDlgItem(hDlg, IDC_QLOW), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_QMED), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_QHIGH), FALSE);
				// Preset
				EnableWindow(GetDlgItem(hDlg, IDC_PRESET), FALSE);
			}
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		// Combo box selection
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_PRESET)) {
				presetindex = static_cast<unsigned int>(SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
			}
		}
		// Drop though if not selected

		switch (LOWORD(wParam)) {

		case IDC_MPEG4:
			EnableWindow(GetDlgItem(hDlg, IDC_QLOW), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_QMED), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_QHIGH), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_PRESET), FALSE);
			break;

		case IDC_X264:
			EnableWindow(GetDlgItem(hDlg, IDC_QLOW), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_QMED), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_QHIGH), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_PRESET), TRUE);
			break;

		case IDC_OPTIONS_HELP:
			{
				std::string str = "Image type\n";
				str += "Tif images, instead of default png, ";
				str += "may be useful if the sender is producing 16 bit textures. ";
				str += "The texture format is shown by the on-screen display or more details in the \"SpoutPanel\" sender selection dialog.\n\n";
				
				str += "File name\nPrompt for file name and show file details after save. By default, a file with the sender name is saved in \"data\\videos\" and over-written if it exists.\n\n";

				str += "Duration\nRecord for a fixed amount of time.\n";
				str += "Seconds - the time to record in seconds.\n\n";

				str += "Audio\nRecord system audio with the video, ";
				str += "A DirectShow <a href=\"https://github.com/rdp/virtual-audio-capture-grabber-device/\">virtual audio device</a>, ";
				str += "developed by Roger Pack, allows FFmpeg to record system audio together with the video. ";
				str += "Register it using \"VirtualAudioRegister.exe\" in the <a href=\"";
				str += g_ExePath;
				str += "\\data\\audio\\VirtualAudio\">\"VirtualAudio\"</a> folder.\n\n";

				str += "Codec\n";
				str += "<a href=\"https://trac.ffmpeg.org/wiki/Encode/MPEG-4\">Mpeg4</a> is a well established codec that performs well for most systems. ";
				str += "<a href=\"https://trac.ffmpeg.org/wiki/Encode/H.264\">h264</a> is a modern codec with more control over ";
				str += "quality, encoding speed and file size.\n\n";

				str += "Quality\n";
				str += "h264 constant rate factor CRF (0 > 51) : low = 28, medium = 23, high = 18. ";
				str += "High quality is effectively lossless, but will create a larger file. ";
				str += "Low quality will create a smaller file at the expense of quality. ";
				str += "Medium is the default, a balance between file size and quality.\n\n";

				str += "Preset\n";
				str += "h264 preset : ultrafast, superfast, veryfast, faster.\n";
				str += "Presets affect encoding speed and compression ratio. ";
				str += "These are the presets necessary for real-time encoding. ";
				str += "Higher speed presets encode faster but produce progressively larger files. ";
				str += "If a slower preset is chosen to reduce file size, check that it does not ";
				str += "introduce hesitations or missed frames.\n\n\n";

				SpoutMessageBox(NULL, str.c_str(), "Options", MB_OK | MB_TOPMOST);
			}
			break;

		case IDC_OPTIONS_RESET:
		{
			extension = ".png";
			bPrompt = false;
			bDuration = false;
			SecondsDuration = 0L;
			bAudio = false;
			codec = 0;
			quality = 1;
			preset = 0;
			SendMessage(hDlg, WM_INITDIALOG, 0, 0L);
		}
		break;

		case IDOK:
			extension = ".png";
			bPrompt = false;
			bDuration = false;
			SecondsDuration = 0L;
			bAudio = false;
			codec = 0;
			quality = 1;
			preset = 0;
			if (IsDlgButtonChecked(hDlg, IDC_TIF) == BST_CHECKED)
				extension = ".tif";
			if (IsDlgButtonChecked(hDlg, IDC_PROMPT) == BST_CHECKED)
				bPrompt = true;
			if (IsDlgButtonChecked(hDlg, IDC_DURATION) == BST_CHECKED)
				bDuration = true;
			GetDlgItemTextA(hDlg, IDC_SECONDS, (LPSTR)tmp, 256);
			SecondsDuration = atol(tmp);
			if (IsDlgButtonChecked(hDlg, IDC_AUDIO) == BST_CHECKED)
				bAudio = true;
			if (IsDlgButtonChecked(hDlg, IDC_X264) == BST_CHECKED)
				codec = 1;
			if (IsDlgButtonChecked(hDlg, IDC_QLOW) == BST_CHECKED)
				quality = 0;
			if (IsDlgButtonChecked(hDlg, IDC_QHIGH) == BST_CHECKED)
				quality = 2;
			preset = presetindex;
			EndDialog(hDlg, 1);
			break;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;

		default:
			return (INT_PTR)FALSE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}
#endif


// Message handler for About box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char tmp[MAX_PATH]={};
	char about[1024]={};
	DWORD dummy = 0;
	DWORD dwSize = 0;
	DWORD dwVersion = 0;
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
		// Set by Spout Installer (2005, 2006, etc.) 
		// or by SpoutSettings for 2.007 and later
		spoutVersion = 0;
		if (ReadDwordFromRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\Spout", "Version", &dwVersion)) {
			spoutVersion = (int)dwVersion;
		}

		if (spoutVersion == 0) {
			sprintf_s(tmp, 256, "Spout 2.004 or earlier\r\n");
			strcat_s(about, 1024, tmp);
		}
		else if (spoutVersion <= 2007) {
			sprintf_s(tmp, 256, "Spout 2.00%1d\r\n", (spoutVersion - 2000));
			strcat_s(about, 1024, tmp);
		}
		else {
			// Contains major, minor and release - e.g. 2007009
			// Use Spout SDK version string insted - e.g. 2.007.009
			strcat_s(about, 1024, "Spout  ");
			strcat_s(about, 1024, GetSDKversion().c_str());
		}
		SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

		// Graphics adapter index and name
		sprintf_s(tmp, MAX_PATH, "%s\r\n", adapterName);
		SetDlgItemTextA(hDlg, IDC_ADAPTER_TEXT, (LPCSTR)tmp);

		// Hyperlink hand cursor
		cursorHand = LoadCursor(NULL, IDC_HAND);
		hwnd = GetDlgItem(hDlg, IDC_SPOUT_URL);
		SetClassLongPtrA(hwnd, GCLP_HCURSOR, (LONG_PTR)cursorHand);
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
