/*

    Spout OpenFrameworks Demo program

    Spout 2.007
    OpenFrameworks 12.0
    Visual Studio 2022

    Copyright (C) 2020-2024 Lynn Jarvis.

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
	04.09.23 - Options for display of file save as well as prompt for file name
			   Changes to image save to use common dialog
			   Version 1.014
	08.09.23 - Larger font for dialogs
			   SpoutMessageBox for About instead of dialog
	09.09.23 - Corrected save for float format GL_RGBA16F Float and GL_RGBA32F
	11.10.23 - Add hdr format for 32/16bit float textures (stb_image_write.h)
			   Add image adjustment dialog and shaders (SpoutShader class)
	09.11.23 - Add contrast adaptive sharpening
			   Sharpness range changed from 0-400 to 0-100
			   Handle show/hide cursor in Adjust dialog
			   Version 1.015
	23.11.23 - Modify ofxSkyBox addon for loadShaders function
			   Load skybox images from resources (resource.h)
			   Add sender format selection
			   Load Truetype font from Windows folder
			   Add DrawString function in case font was not loaded
			   Receiver ofDisableArbTex needed for hdr image capture
			   Version 1.016
	07.12.23 - Use spoututils RemovePath for image save and stop recording messagebox
	08.12.23 - Add Help button to About dialog
	20.12.23 - Rebuild with revised SpoutUtils
	28.12.23 - Revise SelectSenderFormat and comments
			   Add preprocessor define _HAS_STD_BYTE=0 for Openframeworks 12.0
			   to avoid std::byte definition conflict for C++17
			   OF120 VS2022 x64 /MT
			   Version 1.017
	02.01.24 - Increase sender starting resolution to 1280x720
			   Modify skybox addon draw for the larger size.
	14.01.24 - WindowResized - update the sender dimensions to match the window size
			   unless smaller than the starting resolution 1280x720
	16.02.24 - Move BUILDRECEIVER define from ofApp.h to resource.h so that it is 
			   available in resource.rc and version information is set as receiver or sender
			   Version 1.018
	07.03.24 - Receiver
			     Check for rgba8 for shaders
			     Activate shaders on every frame to prevent hesitations
			     Add "re-size" option for enlarge to fbo.
			     Shaders operate on enlarged image and is used for "re-send"
				 Move SpoutMessageBoxIcon to precede about box
	11.03.24 - Sender
				 SpoutMessageBox edit control for sender name entry
				 SpoutMessageBox combo box control for sender format selection
	27.03.24 - Receiver - enable Adjust and Save menu items only when connected
			   Version 1.019
	03.04.24 - Remove ReadTextureData function and move to SpoutGL.cpp ReadTexturePixels
			   Remove resize option
			   Centre messagebox dialogs on the window
			   Receiver - Enable/disable menu items if no sender
	13.04.24 - EnterFileName - corrected default data path double backslash
			   EnterFileName - prevent topmost window hiding file entry dialog
			   Remove ConfirmFileSave, use OFN_OVERWRITEPROMPT in EnterFileName
			   Version 1.020
	19.04.24 - Add colour temperature shader to SpoutShaders
	24.04.24 - Add shader support for changing OpenGL format
	27.04.24 - Test information display with format namimg
	29.04.24 - SpoutShaders - load shaders from resources for easier
			   editing and debug of shader code and adding or change.
	30.04.24 - Add bloom shader
			   Revise shader adjust dialog and open on left side of window.
			   Add GL_RGB10_A2 format for sender
	03.05.24 - Receiver - SpoutRecord.cpp 
			   Add h265 encoding and chroma subsampling options for recording
			   Add h265 to Options > Help
	04.05.24 - Chroma details in Options dialog and help text
			   Version 1.021
	23.05.24 - Sender : SelectSenderFormat - correct default format
			   Version 1.022

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

// STB for hdr image format
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// Capture and recording options
#ifdef BUILDRECEIVER
static INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static char g_ExePath[MAX_PATH]={0}; // Executable path
static bool bResend = false; // Option to send received texture
static char g_ReceiverName[256]={0}; // Re-sending name
static std::string extension = ".png"; // Capture type
static bool bPrompt = false; // file name entry dialog
static bool bShowFile = false; // show file name details after save
static bool bDuration = false; // record for fixed duration
static long SecondsDuration = 0L; // time to record
static bool bAudio = false; // system audio
static int codec = 1; // mpg4 / h264 / h265
static int quality = 0; // h264 CRF
static int chroma = 2; // 4:2:0, 4:2:2, 4:4:4
static int preset = 0; // h264 preset
static int presetindex = 0; // for the combobox
static int imagetype = 0; // Image type index
static int imagetypeindex = 0; // Image type index
static char imagetypes[5][128] ={ ".png", ".tif", ".jpg", ".bmp", ".hdr" };
// For cancel
std::string old_extension = extension;
bool old_bPrompt = bPrompt;
bool old_bShowFile = bShowFile;
bool old_bDuration = bDuration;
long old_SecondsDuration = SecondsDuration;
bool old_bAudio = bAudio;
int old_codec = codec;
int old_quality = quality;
int old_chroma = chroma;
int old_preset = preset;
int old_imagetype = imagetype;
bool old_bResend = bResend;
char old_ReceiverName[256]{0};

//
// Adjustment controls modeless dialog
//
LRESULT CALLBACK UserAdjust(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static HWND hwndAdjust = NULL;
static ofApp* pThis = nullptr;
// Statics for the dialog
static float Brightness  = 0.0;
static float Contrast    = 1.0;
static float Saturation  = 1.0;
static float Gamma       = 1.0;
static float Temp        = 6500.0; // daylight
static float Blur        = 0.0;
static float Bloom       = 0.0;
static float Sharpness   = 0.0;
static float Sharpwidth  = 3.0; // 3x3, 5x5, 7x7
static bool bAdaptive    = false; // CAS adaptive sharpen
static bool bFlip        = false;
static bool bMirror      = false;
static bool bSwap        = false;
// 
static float OldBrightness = 0.0;
static float OldContrast   = 1.0;
static float OldSaturation = 1.0;
static float OldGamma      = 1.0;
static float OldTemp       = 6500.0;
static float OldBlur       = 0.0;
static float OldBloom      = 0.0;
static float OldSharpness  = 0.0;
static float OldSharpwidth = 3.0;
static bool OldAdaptive    = false;
static bool OldFlip        = false;
static bool OldMirror      = false;
static bool OldSwap        = false;
#endif

//--------------------------------------------------------------
void ofApp::setup() {

	ofBackground(0, 0, 0);

	// OpenSpoutConsole(); // Empty console for debugging
	// EnableSpoutLog(); // Log to console
	// SetSpoutLogLevel(SpoutLogLevel::SPOUT_LOG_WARNING); // Log warnings only
	
#ifdef BUILDRECEIVER
	strcpy_s(g_ReceiverName, 256, "Spout Receiver"); // Re-sending name
	// Graphics adapter index and name for about box
	adapterIndex = receiver.GetAdapter();
	receiver.GetAdapterName(adapterIndex, adapterName, 256);
	// Show the application title
	ofSetWindowTitle("Spout Receiver");
	EnableSpoutLogFile("SpoutReceiver.log"); // Log to file
#else
	strcpy_s(senderName, 256, "Spout Sender"); // The sender name
	adapterIndex = sender.GetAdapter();
	sender.GetAdapterName(adapterIndex, adapterName, 256);
	ofSetWindowTitle("Spout Sender");
	EnableSpoutLogFile("Spout Sender.log"); // Log to file
#endif

	// Instance
	g_hInstance = GetModuleHandleA(NULL);

	// Openframeworks window handle
	g_hWnd = ofGetWin32Window();

#ifdef BUILDRECEIVER
	// ofApp class pointer
	pThis = this;
#endif

	// Set a custom window icon
	SetClassLongPtrA(g_hWnd, GCLP_HICON, (LONG_PTR)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));

	// Load a Windows truetype font
	// Arial, Verdana, Tahoma
	LoadWindowsFont(myFont, "Arial", 13);

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
	menu->AddPopupItem(hPopup, "Sender format", false, false);
	menu->AddPopupSeparator(hPopup);
#endif
	// Final File popup menu item is "Exit"
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// "View" popup
	//
#ifdef BUILDRECEIVER
	hPopup = menu->AddPopupMenu(g_hMenu, "View");
	menu->AddPopupItem(hPopup, "Adjust", false, false);
	menu->AddPopupItem(hPopup, "Options", false, false); // No auto check
	menu->AddPopupItem(hPopup, "Capture folder", false, false);
	menu->AddPopupItem(hPopup, "Video folder", false, false);
#endif

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
	menu->AddPopupItem(hPopup, "Show info", true);  // Checked, auto-check
#else
	menu->AddPopupItem(hPopup, "Show info", true); // Checked, auto-check
#endif

	//
	// "Help" popup
	//
	hPopup = menu->AddPopupMenu(g_hMenu, "Help");

	// Common
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

	// Set the sender size now so that the fbo has initial dimensions
	senderWidth  = 1280;
	senderHeight = 720;

	// Application OpenGL format
	// Sender   : see SetSenderFormat below
	// Receiver : see ReceiveTexture/IsUpdated and Snapshot (F12 key)

#ifdef BUILDRECEIVER

	// Allocate an RGBA texture to receive from the sender
	// It is re-allocated later to match the size and format of the sender.
	myTexture.allocate(senderWidth, senderHeight, glFormat);

	// Executable path
	strcpy_s(g_ExePath, MAX_PATH, GetExePath().c_str());

	// FFmpeg path
	g_FFmpegPath = g_ExePath;
	g_FFmpegPath +="\\DATA\\FFMPEG\\ffmpeg.exe";

	// Does FFmpeg.exe exist ?
	if (_access(g_FFmpegPath.c_str(), 0) == -1) {
		g_FFmpegPath.clear(); // disable functions using FFmpeg
	}

	// SpoutReceiver.ini file path
	strcpy_s(g_Initfile, MAX_PATH, g_ExePath);
	strcat_s(g_Initfile, MAX_PATH, "\\SpoutReceiver.ini");

	// Read options and menu settings
	ReadInitFile(g_Initfile);

	// Set the sender name for re-sending
	sender.SetSenderName(g_ReceiverName);

	// Needed for float image capture
	ofDisableArbTex();

#else

	// ---------------------------------------------------------------------------
	//
	// Option
	//
	// Set the sender application starting OpenGL format
	// Default DirectX format is DXGI_FORMAT_B8G8R8A8_UNORM
	//       OpenGL                             Compatible DX11 format
	//       GL_RGBA       8 bit (default)      (DXGI_FORMAT_B8G8R8A8_UNORM)
	//       GL_RGBA8      8 bit                (DXGI_FORMAT_R8G8B8A8_UNORM)
	//       GL_RGB10_A2  10 bit 2 bit alpha	(DXGI_FORMAT_R10G10B10A2_UNORM)
	//       GL_RGBA16    16 bit				(DXGI_FORMAT_R16G16B16A16_UNORM)			
	//       GL_RGBA16F   16 bit float			(DXGI_FORMAT_R16G16B16A16_FLOAT)
	//       GL_RGBA32F   32 bit float			(DXGI_FORMAT_R32G32B32A32_FLOAT)
	//
	// glFormat = GL_RGBA16; // Example 16 bit rgba
	// A compatible DirectX 11 shared texture format must be set
	// so that receivers get a texture with the same format.
	// Note that some applications may not receive other formats.
	// sender.SetSenderFormat(sender.DX11format(glFormat));

	//
	// In this application, the format can be selected from the menu
	//
	// ---------------------------------------------------------------------------

	// Create an fbo for texture transfers
	myFbo.allocate(senderWidth, senderHeight, glFormat);

	// Give the sender a name (if no name is specified, the executable name is used)
	sender.SetSenderName(senderName);

	// Skybox setup
	// Load images from resources
	LoadSkyboxImages();

	// 3D drawing setup for the demo 
	ofDisableArbTex(); // Needed for ofBox texturing
	LoadResourceImage(myBoxImage, 6); // image for the cube texture
	rotX = 0.0f;
	rotY = 0.0f;

	// Use ofEasyCam for mouse control instead of ofCamera
	easycam.setPosition(ofVec3f(0, 0, 0));

	// Enlarge the easycam control area so we have 
	// x and y control but not rotation outside it
	// Scale to the window width
	ofRectangle controlArea = easycam.getControlArea();
	float scale = (float)ofGetWidth() / (float)ofGetHeight();
	controlArea.scaleFromCenter(scale, scale);
	easycam.setControlArea(controlArea);
#endif

	// Centre messagebox dialogs on the window
	SpoutMessageBoxWindow(ofGetWin32Window());

	// Starting value for sender fps display
	g_SenderFps = GetRefreshRate();

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

	// If re-sending do not receive from self
	if (bResend && strstr(receiver.GetSenderName(), g_ReceiverName) != 0) {
		// Release the receiving sender and receive from the next
		sender.ReleaseSender();
	}

	// ReceiveTexture connects to and receives from a sender
	if (receiver.ReceiveTexture(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget)) {

		// If IsUpdated() returns true, the sender has changed
		// and the receiving texture must be re-allocated.
		if (receiver.IsUpdated()) {

			// Allocate an application texture with the same OpenGL format as the sender
			// The received texture format determines image capture bit depth and type
			// GL_RGBA	  0x1908 (default)
			// GL_BGRA    0x08E1
			// GL_RGB_A2  0x8059
			// GL_RGBA16  0x805B
			// GL_RGBA16F 0x881A
			// GL_RGBA32F 0x8814
			glFormat = receiver.GLDXformat();
			myTexture.allocate(receiver.GetSenderWidth(), receiver.GetSenderHeight(), glFormat);

			// Set OpenGL format for shaders
			shaders.SetGLformat(glFormat);

			// Stop recording if the sender name or size has changed
			if (strcmp(receiver.GetSenderName(), senderName) != 0
				|| senderWidth != receiver.GetSenderWidth()
				|| senderHeight != receiver.GetSenderHeight()) {
				StopRecording();
			}

			strcpy_s(senderName, 256, receiver.GetSenderName());
			senderWidth = receiver.GetSenderWidth();
			senderHeight = receiver.GetSenderHeight();

			// If re-sending, set the sender name
			if (bResend) {
				sender.ReleaseSender();
				sender.SetSenderName(g_ReceiverName);
			}

			// Enable Adjust menu when connected
			menu->EnablePopupItem("Adjust", true);

		}

		// Activate shaders on the received texture if the frame is new
		// otherwise they repeat on the processed texture.
		// Shaders have source and destination textures
		// but the source texture can also be the destination
		if (receiver.IsFrameNew()) {
	
			GLuint textureID = myTexture.getTextureData().textureID;
			unsigned int width  = (unsigned int)myTexture.getWidth();
			unsigned int height = (unsigned int)myTexture.getHeight();

			// Shaders have source and destination textures
			// but the source texture can also be the destination

			// Brightness    -1 - 1   default 0
			// Contrast       0 - 4   default 1
			// Saturation     0 - 4   default 1
			// Gamma          0 - 4   default 1
			// 0.005 - 0.007 msec
			if (Brightness != 0.0
				|| Contrast != 1.0
				|| Saturation != 1.0
				|| Gamma != 1.0) {
				shaders.Adjust(textureID, textureID, width, height,
					Brightness, Contrast, Saturation, Gamma);
			}

			// Temperature : 3500 - 9500  (default 6500 daylight)
			if (Temp != 6500.0) {
				shaders.Temperature(textureID, textureID, width, height, Temp);
			}

			// Sharpness 0 - 1  (default 0)
			// 0.001 - 0.002 msec
			if (Sharpness > 0.0) {
				if (bAdaptive) {
					// Sharpness width radio buttons
					// 3x3, 5x5, 7x7 : 3.0, 5.0, 7.0
					float caswidth = 1.0f+(Sharpwidth-3.0f)/2.0f; // 1.0, 2.0, 3.0
					// Sharpness; // 0.0 - 1.0
					shaders.AdaptiveSharpen(textureID, width, height, caswidth, Sharpness);
				}
				else {
					shaders.Sharpen(textureID, textureID, width, height, Sharpwidth, Sharpness);
				}
			}

			// Blur 0 - 4  (default 0)
			// 0.001 - 0.002 msec
			if (Blur > 0.0)
				shaders.Blur(textureID, textureID, width, height, Blur);

			// Blur 0 - 1  (default 0)
			if (Bloom > 0.0)
				shaders.Bloom(textureID, width, height, Bloom);

			if (bFlip)
				shaders.Flip(textureID, width, height);
			if (bMirror)
				shaders.Mirror(textureID, width, height);
			if (bSwap)
				shaders.Swap(textureID, width, height);

			// Re-send if the option is selected
			if (bResend) {
				sender.SendTexture(textureID,
					myTexture.getTextureData().textureTarget,
					width, height, false);
			}

		} // Endif new frame

		// Add a new frame to video if recording
		// Record at the sender dimensions
		if (bRecording && receiver.IsFrameNew()) {
			recorder.Write(myTexture.getTextureData().textureID, myTexture.getTextureData().textureTarget, senderWidth, senderHeight);
			ElapsedRecordingTime = (ElapsedMicroseconds()-StartRecordingTime)/1000000.0; // seconds
			// Check for fixed duration
			if (bDuration) {
				if (ElapsedRecordingTime > (double)SecondsDuration) {
					StopRecording();
				}
			}
		}

		myTexture.draw(0, 0, ofGetWidth(), ofGetHeight());

	} // endif ReceiveTexture

	// Show on-screen details
	if (bShowInfo && !bFullScreen && !bPreview) {
		if (receiver.IsConnected()) {
			ofSetColor(255);
			str = "[";
			str += receiver.GetSenderName();
			str += "]  (";
			str += ofToString(receiver.GetSenderWidth()); str += "x";
			str += ofToString(receiver.GetSenderHeight());

			// Received texture OpenGL format
			// Default DirectX format is DXGI_FORMAT_B8G8R8A8_UNORM
			// If OpenGL format is default GL_RGBA. Do not show.
			if (receiver.GLDXformat() != GL_RGBA) {
				str += " - ";
				str += receiver.GLformatName(receiver.GLDXformat());
			}
			str += ")";
			DrawString(str, 20, 30);
			// GetSenderFrame() will return false for senders < 2.007
			// Frame counting can also be disabled in SpoutSettings
			str.clear();
			// Is the receiver using CPU sharing ?
			if (receiver.GetCPUshare()) {
				str = "CPU share";
			}
			else {
				str = "Texture share";
				// Graphics can still be incompatible if the user
				// did not select "Auto" or "CPU" in SpoutSettings
				if (!receiver.IsGLDXready())
					str = "Graphics not compatible";
			}

			if (receiver.GetSenderFrame() > 0) {
				str += " - fps ";
				str += ofToString((int)roundf(receiver.GetSenderFps()));
				str += " frame  ";
				str += ofToString(receiver.GetSenderFrame());
			}
			else {
				// Show Openframeworks fps
				str += " - fps : " + ofToString((int)roundf(ofGetFrameRate()));
			}
			DrawString(str, 20, 52);

			// Show action keys
			if (!bRecording) {
				str = "F1 - start recording : F12 - capture";
				DrawString(str, 170, ofGetHeight() - 36);
			}
			else {
				str = "F2 - stop recording";
				DrawString(str, 170, ofGetHeight() - 36);
			}

			// Show keyboard shortcuts
			str = "RH click - select sender : f - fullscreen : p - preview : Space - hide info";
			DrawString(str, 40, ofGetHeight() - 14);

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
				DrawString(tmp, ofGetWidth()-300, ofGetHeight()-36);
				ofSetColor(255, 255, 255);
			}
		}
		else {
			DrawString("No sender detected", 20, 30);
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

		std::string str;
		ofSetColor(255);

		if (sender.IsInitialized()) {

			str = "Sending as : ";
			str += sender.GetName(); str += " (";
			str += ofToString(sender.GetWidth()); str += "x";
			str += ofToString(sender.GetHeight()); str += ") ";
			// Sender OpenGL texture format description
			// If the sender format has been set as above
			if (sender.GetDX11format() != DXGI_FORMAT_B8G8R8A8_UNORM) { // default
				str += sender.GLformatName(sender.GLDXformat());
			}
			DrawString(str, 20, 30);

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
			DrawString(str, 20, 52);
		}
		else {
			str = "Could not create sender";
			DrawString(str, 20, 30);
			str = "Help About Logs for details";
			DrawString(str, 20, 52);
		}

		// Show the keyboard shortcuts
		str = "Space - hide info";
		DrawString(str, 20, ofGetHeight() - 20);

	} // endif show info

#endif

} // end Draw

//--------------------------------------------------------------
void ofApp::DrawString(std::string str, int posx, int posy)
{
	if (myFont.isLoaded()) {
		myFont.drawString(str, posx, posy);
	}
	else {
		// This will only happen if the Windows font is not foud
		// Quick fix because the default font is wider
		int x = posx-20;
		if (x <= 0) x = 10;
		ofDrawBitmapString(str, x, posy);
	}
}

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
			SpoutMessageBox(g_hWnd, "No sender selection while recording", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 2000);
		}
	}
	else if (button == 1) {
		// Middle click for adjust
		appMenuFunction("Adjust", false);
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
		// unless smaller than the starting resolution 1280x720
		if (w >= 1280 && h >= 720) {
			senderWidth = w;
			senderHeight = h;
		}
		else {
			senderWidth = 1280;
			senderHeight = 720;
		}

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
void ofApp::appMenuFunction(std::string title, bool bChecked) {


#ifdef BUILDRECEIVER

	if (title == "WM_ENTERMENULOOP") {
		// ofxWinMenu internal command
		// Enable Adjust and Save menu items only when connected
		if (receiver.IsConnected()) {
			menu->EnablePopupItem("Adjust", true);
			menu->EnablePopupItem("Save image", true);
			menu->EnablePopupItem("Save video", true);
			menu->EnablePopupItem("Preview", true);
			menu->EnablePopupItem("Full screen", true);
		}
		else {
			menu->EnablePopupItem("Adjust", false);
			menu->EnablePopupItem("Save image", false);
			menu->EnablePopupItem("Save video", false);
			menu->EnablePopupItem("Preview", false);
			menu->EnablePopupItem("Full screen", false);
		}
	}

	//
	// File Menu
	//

	if (title == "Save image") {
		if (receiver.IsConnected()) {
			std::string name = EnterFileName(2);
			if (name.empty())
				return;
			if (name.substr(name.rfind('.')) == ".hdr" && !(glFormat == GL_RGBA32F || glFormat == GL_RGBA16F)) {
				std::string str ="High dynamic range (hdr) images\nrequire floating point textures.\n";
				str += "Sender format is ";
				str += receiver.GLformatName(receiver.GLDXformat());
				str += "\nSelect a different image type.";
				SpoutMessageBox(g_hWnd, str.c_str(), "Incorrect image type", MB_OK | MB_ICONWARNING | MB_TOPMOST);
				return;
			}
			SaveImageFile(name);
		}
		else {
			SpoutMessageBox(g_hWnd, "No sender", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 1500);
		}
	}

	if (title == "Save video") {
		if (receiver.IsConnected())	{
			StartRecording(true);
		}
		else {
			SpoutMessageBox(g_hWnd, "No sender", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 1500);
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
		if (receiver.IsConnected() && !bFullScreen) {
			// Make a timestamped image file name
			std::string imagename = receiver.GetSenderName();
			imagename += "_" + ofGetTimestampString() + extension;
			// Save image to bin>data>captures
			std::string savepath = g_ExePath;
			savepath += "\\Data\\captures\\";
			savepath += imagename;
			SaveImageFile(savepath);
		}
		else {
			SpoutMessageBox(g_hWnd, "No sender\nImage capture not possible", "Spout Receiver", MB_OK | MB_ICONWARNING | MB_TOPMOST, 2000);
		}
	}

#else

	//
	// File menu
	//
	if (title == "Sender name") {
		char sendername[256]{}; // Name comparison
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

	if (title == "Sender format") {
		SelectSenderFormat();
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

	// Capture/recording options
	if (title == "Options") {

		old_extension = extension;
		old_bPrompt = bPrompt;
		old_bShowFile = bShowFile;
		old_bDuration = bDuration;
		old_SecondsDuration = SecondsDuration;
		old_bAudio = bAudio;
		old_codec = codec;
		old_quality = quality;
		old_chroma = chroma;
		old_preset = preset;
		old_imagetype = imagetype;
		old_bResend = bResend;
		strcpy_s(old_ReceiverName, 256, g_ReceiverName);
		if (!DialogBoxA(g_hInstance, MAKEINTRESOURCEA(IDD_OPTIONSBOX), g_hWnd, Options)) {
			extension = old_extension;
			bPrompt = old_bPrompt;
			bShowFile = old_bShowFile;
			bDuration = old_bDuration;
			SecondsDuration = old_SecondsDuration;
			bAudio = old_bAudio;
			codec = old_codec;
			quality = old_quality;
			chroma = old_chroma;
			preset = old_preset;
			imagetype = old_imagetype;
			imagetype = old_imagetype;
			bResend = old_bResend;
			strcpy_s(g_ReceiverName, 256, old_ReceiverName);
		}
		else {
			// Release sender if option has been unchecked or the sender name is different
			if (!bResend || strcmp(g_ReceiverName, old_ReceiverName) != 0)
				sender.ReleaseSender();
			sender.SetSenderName(g_ReceiverName);
		}

	}

	// Image adjustment
	if (title == "Adjust") {
		if (receiver.IsConnected()) {
			if (!hwndAdjust) {
				OldBrightness = Brightness;
				OldContrast   = Contrast;
				OldSaturation = Saturation;
				OldGamma      = Gamma;
				OldTemp       = Temp;
				OldBlur       = Blur;
				OldBloom      = Bloom;
				OldSharpness  = Sharpness;
				OldSharpwidth = Sharpwidth;
				OldAdaptive   = bAdaptive;
				OldFlip       = bFlip;
				OldMirror     = bMirror;
				OldSwap       = bSwap;
				if (bFullScreen) {
					CURSORINFO info{};
					info.cbSize =sizeof(CURSORINFO);
					GetCursorInfo(&info);
					if (!info.hCursor) ShowCursor(TRUE);
				}
				hwndAdjust = CreateDialogA(g_hInstance, MAKEINTRESOURCEA(IDD_ADJUSTBOX), g_hWnd, (DLGPROC)UserAdjust);
			}
			else {
				SendMessageA(hwndAdjust, WM_DESTROY, 0, 0L);
				hwndAdjust = NULL;
			}
		}
	}
#endif

	if (title == "About") {
		
		// About using SpoutMessageBox instead of dialog
		// to avoid the overhead of a dialog and message handler
		std::vector<std::string>lines;
		std::string line;

		line = "Spout demo program\n";
		lines.push_back(line);
		line = "Version ";

		// Get product version number
		char tmp[MAX_PATH]{};
		if (GetModuleFileNameA(g_hInstance, tmp, MAX_PATH)) {
			DWORD dummy = 0;
			DWORD dwSize = GetFileVersionInfoSizeA(tmp, &dummy);
			if (dwSize > 0) {
				std::vector<BYTE> data(dwSize);
				if (GetFileVersionInfoA(tmp, NULL, dwSize, &data[0])) {
					LPVOID pvProductVersion = NULL;
					unsigned int iProductVersionLen = 0;
					if (VerQueryValueA(&data[0], ("\\StringFileInfo\\080904E4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
						sprintf_s(tmp, MAX_PATH, "%s\n", (char*)pvProductVersion);
						line += tmp;
					}
				}
			}
		}
		lines.push_back(line);

		// Get the Spout version
		// Set by Spout Installer (2005, 2006, etc.) 
		// or by SpoutSettings for 2.007 and later
		spoutVersion = 0;
		DWORD dwVersion = 0;
		if (ReadDwordFromRegistry(HKEY_CURRENT_USER, "Software\\Leading Edge\\Spout", "Version", &dwVersion)) {
			spoutVersion = (int)dwVersion;
		}
		if (spoutVersion == 0) {
			sprintf_s(tmp, 256, "Spout 2.004 or earlier\n");
			line = tmp;
		}
		else if (spoutVersion <= 2007) {
			sprintf_s(tmp, 256, "Spout 2.00%1d\n", (spoutVersion - 2000));
			line = tmp;
		}
		else {
			// Contains major, minor and release - e.g. 2007009
			// Use Spout SDK version string insted - e.g. 2.007.009
			line = "Spout  ";
			line += GetSDKversion().c_str();
			line += "\n";
		}
		lines.push_back(line);

		// Graphics adapter index and name retreived above
		sprintf_s(tmp, MAX_PATH, "%s\n", adapterName);
		lines.push_back(tmp);

		// Empty line
		line = " \n";
		lines.push_back(line);

		line = "<a href=\"http://spout.zeal.co/\">http://spout.zeal.co/</a>\n";
		lines.push_back(line);

		// Pad lines to centre
		// No way to calculate the spaces required
		// for padding due to proportional font
		int i = 0;
		for (i=0; i<18; i++) lines[0]=" "+lines[0];
		for (i=0; i<25; i++) lines[1]=" "+lines[1];
		for (i=0; i<22; i++) lines[2]=" "+lines[2];
		for (i=0; i<16; i++) lines[3]=" "+lines[3];
		for (i=0; i<10; i++) lines[4]=" "+lines[4];
		for (i=0; i<19; i++) lines[5]=" "+lines[5];

		// Empty lines at bottom
		line = " \n\n";
		lines.push_back(line);

		// Construct string
		std::string str;
		for (i = 0; i<(int)lines.size(); i++)
			str += lines[i];

		SpoutMessageBoxButton(1000, L"Logs");
		SpoutMessageBoxButton(2000, L"Help");
		// Custom icon for the SpoutMessagBox, activated by MB_USERICON
		SpoutMessageBoxIcon(LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));
		int iRet = SpoutMessageBox(g_hWnd, str.c_str(), "About", MB_OK | MB_USERICON | MB_USERBUTTON | MB_TOPMOST);
		if (iRet == 1000) {
			// Logs button
			OpenSpoutLogs();
		}
		else if (iRet == 2000) {
			// Help button
#ifdef BUILDRECEIVER
			str =  "F1         - Start recording video\n";
			str += "F2         - Stop recording video\n";
			str += "F12       - Capture image\n";
			str += "'f'          - full screen (repeat 'f' or ESC to exit)\n";
			str += "'p'         - preview mode with no window border\n";
			str += "'space'  - hide on-screen information\n";
			str += "Right mouse button   - select sender\n";
			str += "Centre mouse button - image adjust dialog\n";
			SpoutMessageBox(g_hWnd, str.c_str(), "Help", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
#else
			str  = "Sender name   - set a different sender name\n";
			str += "Sender format - sets the DirectX shared texture format\n";
			str +="    Default             DXGI_FORMAT_B8G8R8A8_UNORM\n";
			str +="    GL_RGBA         DXGI_FORMAT_R8G8B8A8_UNORM\n";
			str +="    GL_RGB_A2      DXGI_FORMAT_R10G10B10A2_UNORM\n";
			str +="    GL_RGBA16      DXGI_FORMAT_R16G16B16A16_UNORM\n";
			str +="    GL_RGBA16F    DXGI_FORMAT_R16G16B16A16_FLOAT\n";
			str +="    GL_RGBA32F    DXGI_FORMAT_R32G32B32A32_FLOAT\n";
			str += "'space' - hide on-screen information\n";
			SpoutMessageBox(g_hWnd, str.c_str(), "Help", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
#endif
		}
	}

} // end appMenuFunction



//--------------------------------------------------------------
// Load a Windows truetype font
bool ofApp::LoadWindowsFont(ofTrueTypeFont& font, std::string name, int size)
{
	std::string fontfolder;
	char* path = nullptr;
	errno_t err = _dupenv_s(&path, NULL, "WINDIR");
	if (err == 0 && path) {
		fontfolder = path;
		fontfolder += "\\Fonts\\";
		fontfolder += name;
		fontfolder += ".ttf";
		if (_access(fontfolder.c_str(), 0) != -1) {
			return font.load(fontfolder, size, true, true);
		}
	}
	return false;
}


#ifdef BUILDRECEIVER

//--------------------------------------------------------------
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

		// For reasons unknown, rendering is slow if the window is resized
		// to the monitor extents. Making it 1 pixel larger seems to fix it.
		// Hide the window while re-sizing to avoid a flash effect
		SetWindowPos(g_hWnd, HWND_TOPMOST, x-1, y-1, w+2, h+2, SWP_HIDEWINDOW | SWP_NOREDRAW);

		if (!hwndAdjust)
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

//--------------------------------------------------------------
// Recording start - F1
bool ofApp::StartRecording(bool prompt)
{
	// Return if not receiving, already recording, preview or full screen
	if (!receiver.IsConnected() || bRecording || bPreview || bFullScreen)
		return false;

	if (prompt) {
		std::string str = EnterFileName(codec);
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

	// Options for audio, codec, preset, quality (crf), chroma and fps
	recorder.EnableAudio(bAudio); // For recording system audio
	recorder.SetCodec(codec);     // 0-mpeg4, 1-libx264, 2-libx265
	recorder.SetPreset(preset);   // 0-ultrafast, 1-superfast, 2-veryfast, 3-faster
	recorder.SetQuality(quality); // 0-low, 1-medium, 2-high (crf)
	recorder.SetChroma(chroma);   // 0 - low 4:2:0, 1 - medium 4:2:2, 2 - high 4:4:4
	recorder.SetFps(30);          // Fps for FFmpeg (see HoldFps)

	if (recorder.Start(g_FFmpegPath, g_OutputFile, receiver.GetSenderWidth(), receiver.GetSenderHeight(), true)) { // RGBA pixel format
		bRecording = true;
		StartRecordingTime = ElapsedMicroseconds();
		return true;
	}
	else {
		std::string msg = "FFmpeg not found\n\n";
		msg += "Go to <a href=\"https://github.com/GyanD/codexffmpeg/releases/\">https://github.com/GyanD/codexffmpeg/releases/</a>\n\n";
		msg += " * Choose the \"Essentials\" build (for example \"ffmpeg-6.1- essentials_build.zip\")\n";
		msg += " * Download the archive to any convenient folder.\n";
		msg += " * Unzip the archive and copy \"bin\\ffmpeg.exe\" to : \"bin\\data\\ffmpeg\"\n\n";
		msg += "Do you want to down;oad it now?\n\n";
		if (SpoutMessageBox(g_hWnd, msg.c_str(), "Spout Receiver", MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDYES) {
			ShellExecuteA(NULL, "open", "https://github.com/GyanD/codexffmpeg/releases/", 0, 0, SW_SHOWNORMAL);
		}
	}

	return false;
}

//--------------------------------------------------------------
// Recording stop - F2
void ofApp::StopRecording()
{
	if (bRecording) {

		recorder.Stop();

		// Show a messagebox with video file details
		// Video file name without full path
		std::string msgstr = g_OutputFile;
		RemovePath(msgstr);

		// Duration
		char tmp[MAX_PATH]={};
		sprintf_s(tmp, MAX_PATH, " (%.2f seconds)\n\n", (float)ElapsedRecordingTime);
		msgstr += tmp;

		// Settings
		if (codec == 0) {
			msgstr += "mpeg4 codec, ";
		}
		else {
			if (codec == 1)
				msgstr += "x264 codec, ";
			if (codec == 2)
				msgstr += "x265 codec, ";
			if (quality == 0)
				msgstr += "low quality, ";
			if (quality == 1)
				msgstr += "medium quality, ";
			if (quality == 2)
				msgstr += "high quality, ";
			if (chroma == 0)
				msgstr += "4:2:0, ";
			if (chroma == 1)
				msgstr += "4:2:2, ";
			if (chroma == 2)
				msgstr += "4:4:4, ";
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

		if (bShowFile)
			SpoutMessageBox(g_hWnd, msgstr.c_str(), "Saved video file", MB_OK | MB_ICONINFORMATION);
		else
			SpoutMessageBox(g_hWnd, msgstr.c_str(), "Saved video file", MB_OK, 2000);
	}

	bRecording = false;
	StartRecordingTime = 0.0;

}

//--------------------------------------------------------------
//
// User file name entry
//
// Type :
//    0 - mp4 video
//    1 - mkv video
//    2 - image file png, tif or other
//
std::string ofApp::EnterFileName(int type)
{
	// Prevent topmost window hiding file entry dialog
	if (bTopmost)
		SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	char filePath[MAX_PATH]={0};
	if (type < 2) {
		// Default video output path
		if (g_OutputFile.empty()) {
			g_OutputFile = g_ExePath; // exe folder
			g_OutputFile += "Data\\videos\\";
			g_OutputFile += senderName;
			sprintf_s(filePath, MAX_PATH, g_OutputFile.c_str());
		}
	}
	else {
		// Default image output path
		if (g_OutputImage.empty()) {
			g_OutputImage = g_ExePath; // exe folder
			g_OutputImage += "Data\\captures\\";
			g_OutputImage += senderName;
			sprintf_s(filePath, MAX_PATH, g_OutputImage.c_str());
		}
	}

	OPENFILENAMEA ofn={};
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	HWND hwnd = NULL;
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.nMaxFileTitle = 31;
	// if ofn.lpstrFile contains a path, that path is the initial directory.
	ofn.lpstrFile = (LPSTR)filePath;
	ofn.nMaxFile = MAX_PATH;
	// Type can be codec = 0, 1 or 2 for image
	if (type == 0)
		ofn.lpstrFilter = "Mpeg-4 (*.mp4)\0*.mp4\0Matroska (*.mkv)\0*.mkv\0Audio Video Interleave (*.avi)\0*.avi\0Quicktime (*.mov)\0*.mov\0All files (*.*)\0*.*\0";
	else if (type == 1)
		ofn.lpstrFilter = "Matroska (*.mkv)\0*.mkv\0Mpeg-4 (*.mp4)\0*.mp4\0Audio Video Interleave (*.avi)\0*.avi\0Quicktime (*.mov)\0*.mov\0All files (*.*)\0*.*\0";
	else {
		// Image types supported by Freeimage
		// .png, .tif, .jpg, .bmp, .hdr
		// Other image types supported by Openframeworks
		// .pcx, .pgm, .ppm, .tga, .gif, 
		if (extension == ".jpg")
			ofn.lpstrFilter = "JPG (JPEG file interchange format)\0*.jpg\0PNG (Portable Network Graphics)\0*.png\0TIF (Tagged Image File Format)\0*.tif\0BMP (Windows bitmap)\0*.bmp\0HDR (High Dynamic Range image)\0*.hdr\0All files (*.*)\0*.*\0";
		else if (extension == ".bmp")
			ofn.lpstrFilter = "BMP (Windows bitmap)\0*.bmp\0PNG (Portable Network Graphics)\0*.png\0Tiff (Tagged Image File Format)\0*.tif\0JPG (JPEG file interchange format)\0*.jpg\0HDR (High Dynamic Range image)\0*.hdr\0All files (*.*)\0*.*\0";
		else if (extension == ".tif")
			ofn.lpstrFilter = "TIF (Tagged Image File Format)\0*.tif\0PNG (Portable Network Graphics)\0*.png\0JPG (JPEG file interchange format)\0*.jpg\0BMP (Windows bitmap)\0*.bmp\0HDR (High Dynamic Range image)\0*.hdr\0All files (*.*)\0*.*\0";
		else if (extension == ".hdr")
			ofn.lpstrFilter = "HDR (High Dynamic Range image)\0*.hdr\0PNG (Portable Network Graphics)\0*.png\0TIF (Tagged Image File Format)\0*.tif\0JPG (JPEG file interchange format)\0*.jpg\0BMP (Windows bitmap)\0*.bmp\0All files (*.*)\0*.*\0";
		else
			ofn.lpstrFilter = "PNG (Portable Network Graphics)\0*.png\0TIF (Tagged Image File Format)\0*.tif\0JPG (JPEG file interchange format)\0*.jpg\0BMP (Windows bitmap)\0*.bmp\0HDR (High Dynamic Range image)\0*.hdr\0All files (*.*)\0*.*\0";
	}

	ofn.lpstrDefExt = "";
	// OFN_OVERWRITEPROMPT prompts for over-write
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrTitle = "Output File";
	BOOL bRet = GetSaveFileNameA(&ofn);

	if (bTopmost)
		SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// Return to try again for fail or cancel
	if(!bRet) return "";

	// User file name entry
	return filePath;
}

//--------------------------------------------------------------
void ofApp::SaveImageFile(std::string name)
{
	// Add the default extension if none entered
	std::size_t pos = name.rfind('.');
	if (pos == std::string::npos)
		name += ".png";

	// hdr image
	if (name.substr(pos) == ".hdr" && !(glFormat == GL_RGBA32F || glFormat == GL_RGBA16F)) {
		std::string str ="High dynamic range (hdr) images\nrequire floating point textures.\n";
		str += "Sender format is ";
		str += receiver.GLformatName(receiver.GLDXformat());
		str += "\nSelect a different image type.";
		SpoutMessageBox(g_hWnd, str.c_str(), "Incorrect image type", MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return;
	}

	// Get pixels from received texture
	// Sender texture format determines image type, 16 or 8 bit depth float or unsigned
	// The format is found in ReceiveTexture/IsUpdated and displayed on-screen
	if (glFormat == GL_RGBA32F) {
		// HDR format
		// see: https://booksite.elsevier.com/samplechapters/9780123749147/02~Chapter_3.pdf
		pos = name.rfind(".");
		if (name.substr(pos) != ".hdr") {
			SpoutMessageBox(g_hWnd, "High dynamic range format is required\nfor 32 bit floating point textures.\n\
					Change to HDR image type.", "Incorrect image type", MB_OK | MB_ICONWARNING | MB_TOPMOST);
			return;
		}
		// STB RGB float HDR
		unsigned int width = (unsigned int)myTexture.getWidth();
		unsigned int height = (unsigned int)myTexture.getHeight();
		float* fbuffer = new float[width*height*3];
		receiver.ReadTexturePixels(myTexture.getTextureData().textureID,
			myTexture.getTextureData().textureTarget,
			width, height, fbuffer, glFormat, 3);
		stbi_write_hdr(name.c_str(), width, height, 3, fbuffer);
		delete[] fbuffer;
	}
	else if (glFormat == GL_RGBA16F) {
		// 16 bit RGBA float - tif and hdr
		pos = name.rfind(".");
		if (!(name.substr(pos) == ".tif" || name.substr(pos) == ".hdr")) {
			SpoutMessageBox(g_hWnd, "TIF or HDR are the only image formats\nsupported for 16 bit float textures\n\
					Change to TIF or HDR image type.", "Incorrect file type", MB_OK | MB_ICONWARNING | MB_TOPMOST);
			return;
		}
		if (name.substr(pos) == ".hdr") {
			// STB RGB float HDR
			unsigned int width = (unsigned int)myTexture.getWidth();
			unsigned int height = (unsigned int)myTexture.getHeight();
			float* fbuffer = new float[width*height*3];
			receiver.ReadTexturePixels(myTexture.getTextureData().textureID,
				myTexture.getTextureData().textureTarget,
				width, height, fbuffer, glFormat, 3);
			stbi_write_hdr(name.c_str(), width, height, 3, fbuffer);
			delete[] fbuffer;
		}
		else {
			// Freeimage rgba tif
			ofFloatImage myimage; // Float pixels
			myTexture.readToPixels(myimage.getPixels());
			myimage.save(name); // save png or tif
		}
	}
	else if (glFormat == GL_RGBA16) {
		// TIF and PNG supported
		// 16 bit RGBA - png or tif
		pos = name.rfind(".");
		if (!(name.substr(pos) == ".tif" || name.substr(pos) == ".png")) {
			SpoutMessageBox(g_hWnd, "PNG or TIF are the only image formats supported for 16 bit unsigned textures\n\
					Change to PNG or TIF image type.", "Incorrect image type", MB_OK | MB_ICONWARNING | MB_TOPMOST);
			return;
		}
		ofShortImage myimage; // Bit depth 16 bits
		myTexture.readToPixels(myimage.getPixels());
		myimage.save(name); // save png or tif
	}
	else if (glFormat == GL_RGBA || glFormat == GL_RGBA8) {
		// 8 bit rgba - png, tif, jpg, bmp
		pos = name.rfind(".");
		if (name.substr(pos) == ".tif" || name.substr(pos) == ".png"
			|| name.substr(pos) == ".jpg" || name.substr(pos) == ".bmp") {
			ofImage myimage; // Bit depth 8 bits
			myTexture.readToPixels(myimage.getPixels());
			myimage.save(name); // save tif, png, jpg, bmp
		}
	}
	else {
		std::string str ="Sender format [";
		str += receiver.GLformatName(receiver.GLDXformat());
		str += "] not supported";
		SpoutMessageBox(g_hWnd, str.c_str(), "Incompatible format", MB_OK | MB_ICONWARNING | MB_TOPMOST);
		return;
	}

	// Image name without full path
	std::string imagename = name;
	RemovePath(name);
	if (bShowFile)
		SpoutMessageBox(g_hWnd, imagename.c_str(), "Saved image", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	else
		SpoutMessageBox(g_hWnd, imagename.c_str(), "Saved image", MB_OK | MB_USERICON | MB_TOPMOST, 2000);

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
	if (bShowFile)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Showfile", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Showfile", (LPCSTR)"0", (LPCSTR)initfile);
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
	if (chroma == 0)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Chroma", (LPCSTR)"0", (LPCSTR)initfile);
	else if (chroma == 1)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Chroma", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Chroma", (LPCSTR)"2", (LPCSTR)initfile);

	sprintf_s(tmp, MAX_PATH, "%d", preset);
	WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Preset", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%d", imagetype);
	WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Imagetype", (LPCSTR)tmp, (LPCSTR)initfile);

	// Image adjustment
	sprintf_s(tmp, MAX_PATH, "%.3f", Brightness);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Brightness", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Contrast);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Contrast", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Saturation);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Saturation", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Gamma);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Gamma", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Temp);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Temp", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Blur);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Blur", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Bloom);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Bloom", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Sharpness);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Sharpness", (LPCSTR)tmp, (LPCSTR)initfile);
	sprintf_s(tmp, MAX_PATH, "%.3f", Sharpwidth);
	WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Sharpwidth", (LPCSTR)tmp, (LPCSTR)initfile);
	if (bAdaptive)
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Adaptive", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Adaptive", (LPCSTR)"0", (LPCSTR)initfile);
	if (bFlip)
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Flip", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Flip", (LPCSTR)"0", (LPCSTR)initfile);
	if (bMirror)
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Mirror", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Mirror", (LPCSTR)"0", (LPCSTR)initfile);
	if (bSwap)
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Swap", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Adjust", (LPCSTR)"Swap", (LPCSTR)"0", (LPCSTR)initfile);

	// Options
	if (bShowInfo)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Info", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Info", (LPCSTR)"0", (LPCSTR)initfile);

	if (bTopmost)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Topmost", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Topmost", (LPCSTR)"0", (LPCSTR)initfile);

	//
	// Receiver sending options
	//

	// Re-send
	if (bResend)
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Resend", (LPCSTR)"1", (LPCSTR)initfile);
	else
		WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Resend", (LPCSTR)"0", (LPCSTR)initfile);

	WritePrivateProfileStringA((LPCSTR)"Options", (LPCSTR)"Receivername", (LPCSTR)g_ReceiverName, (LPCSTR)initfile);

}


//--------------------------------------------------------------
// Read back settings from configuration file
void ofApp::ReadInitFile(const char* initfile)
{
	char tmp[MAX_PATH]={0};

	// Capture options
	extension = ".png";
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Imagetype", NULL, (LPSTR)tmp, 5, initfile);
	if (tmp[0]) imagetype = atoi(tmp);
	extension = imagetypes[imagetype];
	
	// Recording options
	bPrompt = false; bShowFile = false; bDuration=false; SecondsDuration=0L;
	bAudio = false; codec = 1; quality = 1; chroma = 2; preset = 0;
	
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Prompt", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bPrompt = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Showfile", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bShowFile = (atoi(tmp) == 1);
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
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Chroma", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) chroma = atoi(tmp);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Preset", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) preset = atoi(tmp);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Imagetype", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) imagetype = atoi(tmp);

	// Image dajustment
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Brightness", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Brightness = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Contrast", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Contrast = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Saturation", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Saturation = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Gamma", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Gamma = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Temp", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0])Temp = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Blur", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Blur = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Bloom", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Bloom = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Sharpness", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Sharpness = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Sharpwidth", NULL, (LPSTR)tmp, 8, initfile);
	if (tmp[0]) Sharpwidth = (float)atof(tmp);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"Adaptive", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bAdaptive = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"bFlip", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bFlip = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"bMirror", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bMirror = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Adjust", (LPSTR)"bSwap", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bSwap = (atoi(tmp) == 1);

	// Menu options
	bShowInfo = true; bTopmost = false;
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Info", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bShowInfo = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Topmost", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bTopmost = (atoi(tmp) == 1);
	
	// Receiver sending options
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Resend", NULL, (LPSTR)tmp, 3, initfile);
	if (tmp[0]) bResend = (atoi(tmp) == 1);
	GetPrivateProfileStringA((LPCSTR)"Options", (LPSTR)"Receivername", NULL, (LPSTR)tmp, 256, initfile);
	if (tmp[0]) strcpy_s(g_ReceiverName, 256, tmp);

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
#else

//--------------------------------------------------------------
// Load skybox images
bool ofApp::LoadSkyboxImages()
{
	bool bRet = false;
	bRet = LoadResourceImage(skyImage[0], 0);
	bRet = LoadResourceImage(skyImage[1], 1);
	bRet = LoadResourceImage(skyImage[2], 2);
	bRet = LoadResourceImage(skyImage[3], 3);
	bRet = LoadResourceImage(skyImage[4], 4);
	bRet = LoadResourceImage(skyImage[5], 5);
	if (bRet) {
		skybox.loadShaders();
		// pos_x, pos_y, pos_z, neg_x, neg_y, neg_z
		skybox.cubeMap.loadFromOfImages(skyImage[0], skyImage[1], skyImage[2],
			skyImage[3], skyImage[4], skyImage[5]);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
// Load an image from resources
bool ofApp::LoadResourceImage(ofImage &image, int index)
{
	HMODULE hModule = NULL;
	DWORD size = 0;
	HRSRC hResInfo = NULL;
	HGLOBAL rcImageData = NULL;
	const char* imagedata = NULL;

	hModule = GetCurrentModule();
	if (hModule) {
		hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDC_IMAGEFILE0+index), RT_RCDATA);
		if (hResInfo) {
			size = SizeofResource(hModule, hResInfo);
			if (size > 0) {
				rcImageData = LoadResource(hModule, hResInfo);
				if (rcImageData) {
					imagedata = static_cast<const char*>(LockResource(rcImageData));
					ofBuffer buffer;
					buffer.set(imagedata, size);
					ofPixels pixels;
					if (ofLoadImage(pixels, buffer)) {
						image.setFromPixels(pixels);
						UnlockResource(rcImageData);
						FreeResource(rcImageData);
						return true;
					}
					UnlockResource(rcImageData);
				}
				FreeResource(rcImageData);
			}
		}
		return false;
	}
	return false;
}

//--------------------------------------------------------------
bool ofApp::EnterSenderName(char *name, const char *caption)
{
	std::string strname;
	SpoutMessageBoxIcon(LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));
	if (SpoutMessageBox(NULL, NULL, caption, MB_OKCANCEL | MB_USERICON, strname) == IDOK) {
		strcpy_s(name, 256, strname.c_str()); // Return the entered name
		return true;
	}
	// Cancel
	return false;

}


//--------------------------------------------------------------
bool ofApp::SelectSenderFormat()
{
	std::vector<std::string> items;
	// "Default", "8 bit RGBA", "16 bit RGBA", "16 bit RGBA float", "32 bit RGBA float"
	items.push_back("Default");       // DXGI_FORMAT_B8G8R8A8_UNORM
	items.push_back("8 bit RGBA");    // DXGI_FORMAT_R8G8B8A8_UNORM
	items.push_back("10 bit RGB A2"); // DXGI_FORMAT_R10G10B10A2_UNORM
	items.push_back("16 bit RGBA");
	items.push_back("16 bit RGBA float");
	items.push_back("32 bit RGBA float");
	int selected = 0;
	switch (glFormat) {
		case GL_RGBA8:    selected = 1;	break;
		case GL_RGB10_A2: selected = 2; break;
		case GL_RGBA16:	  selected = 3; break;
		case GL_RGBA16F:  selected = 4; break;
		case GL_RGBA32F:  selected = 5; break;
		default: selected = 0; break;
	}
	if (sender.GetDX11format() == DXGI_FORMAT_B8G8R8A8_UNORM) // default
		selected = 0;

	if (SpoutMessageBox(NULL, NULL, "Sender format", MB_ICONINFORMATION | MB_OKCANCEL, items, selected) == IDOK) {
		switch (selected) {
			// Default DirectX output format (GL_RGBA)
			default:
			case 0: glFormat = GL_RGBA;		break; // 0x1908 - changed below
			case 1:	glFormat = GL_RGBA8;    break; // 0x8058
			case 2:	glFormat = GL_RGB10_A2; break; // 0x8059
			case 3:	glFormat = GL_RGBA16;   break; // 0x805B
			case 4:	glFormat = GL_RGBA16F;  break; // 0x881A
			case 5:	glFormat = GL_RGBA32F;  break; // 0x8814
		}
		// Set sender DirectX texture format
		sender.ReleaseSender();
		if (selected == 0) {
			sender.SetSenderFormat(DXGI_FORMAT_B8G8R8A8_UNORM); // Default
		}
		else {
			sender.SetSenderFormat(sender.DX11format(glFormat)); // Selected
		}

		sender.SetSenderName(senderName); // Keep the same name
		myFbo.allocate(senderWidth, senderHeight, glFormat);
		return true;
	}
	// Cancel
	return false;

}

#endif

#ifdef BUILDRECEIVER

// Message handler for capture and recording options
INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndList = NULL;
	char tmp[256]={0};
	char presets[4][128] ={ "Ultrafast", "Superfast", "Veryfast", "Faster" };
	// imagetypes array is static
	int state = 1; // TRUE/FALSE


	switch (message) {

	case WM_INITDIALOG:
		{

			// Centre on the application window
			RECT wrect{};
			RECT drect{};
			GetWindowRect(pThis->g_hWnd, &wrect);
			GetWindowRect(hDlg, &drect);
			int x = wrect.left+((wrect.right-wrect.left)-(drect.right-drect.left))/2;
			int y = wrect.top+((wrect.bottom-wrect.top)-(drect.bottom-drect.top))/2;
			SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE);

			// Image type
			hwndList = GetDlgItem(hDlg, IDC_IMAGE_TYPE);
			SendMessageA(hwndList, (UINT)CB_RESETCONTENT, 0, 0L);
			for (int k = 0; k < 5; k++)
				SendMessageA(hwndList, (UINT)CB_ADDSTRING, 0, (LPARAM)imagetypes[k]);
			imagetypeindex = imagetype;
			SendMessageA(hwndList, CB_SETCURSEL, (WPARAM)imagetypeindex, (LPARAM)0);

			// File name entry prompt
			CheckDlgButton(hDlg, IDC_PROMPT, (UINT)bPrompt);
			// Show file details
			CheckDlgButton(hDlg, IDC_SHOWFILE, (UINT)bShowFile);
			// Record for duration
			CheckDlgButton(hDlg, IDC_DURATION, (UINT)bDuration);
			// Codec : 0-mpeg4, 1-libx264, 2-libx265
			// TODO  : 0-mpeg4, 1-libx264, 2-libx265, 3-h264_nvenc, 4-hevc_nvenc, 5-hap 
			CheckRadioButton(hDlg, IDC_MPEG4, IDC_X265, IDC_MPEG4+codec);
			// System audio
			CheckDlgButton(hDlg, IDC_AUDIO, (UINT)bAudio);
			// SecondsDuration
			sprintf_s(tmp, "%ld", SecondsDuration);
			SetDlgItemTextA(hDlg, IDC_SECONDS, (LPCSTR)tmp);
			// Quality
			CheckRadioButton(hDlg, IDC_QLOW, IDC_QHIGH, IDC_QLOW+(int)quality);
			// Chroma
			CheckRadioButton(hDlg, IDC_CLOW, IDC_CHIGH, IDC_CLOW+(int)chroma);
			// Presets
			hwndList = GetDlgItem(hDlg, IDC_PRESET);
			SendMessageA(hwndList, (UINT)CB_RESETCONTENT, 0, 0L);
			for (int k = 0; k < 4; k++)
				SendMessageA(hwndList, (UINT)CB_ADDSTRING, 0, (LPARAM)presets[k]);
			presetindex = preset;
			SendMessageA(hwndList, CB_SETCURSEL, (WPARAM)presetindex, (LPARAM)0);

			// Disable h264 options for mpeg4
			if (codec == 0)	state = FALSE;
			else state = TRUE;
			// Quality
			EnableWindow(GetDlgItem(hDlg, IDC_QLOW),  state);
			EnableWindow(GetDlgItem(hDlg, IDC_QMED),  state);
			EnableWindow(GetDlgItem(hDlg, IDC_QHIGH), state);
			// Chroma
			EnableWindow(GetDlgItem(hDlg, IDC_CLOW), state);
			EnableWindow(GetDlgItem(hDlg, IDC_CMED), state);
			EnableWindow(GetDlgItem(hDlg, IDC_CHIGH), state);
			// Preset
			EnableWindow(GetDlgItem(hDlg, IDC_PRESET), state);

			// Re-send option
			if (bResend)
				CheckDlgButton(hDlg, IDC_RESEND, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_RESEND, BST_UNCHECKED);
			
			// Re-sending name
			SetDlgItemTextA(hDlg, IDC_RESEND_NAME, (LPCSTR)g_ReceiverName);

		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		// Combo box selection
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_IMAGE_TYPE)) {
				imagetypeindex = static_cast<unsigned int>(SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
			}
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_PRESET)) {
				presetindex = static_cast<unsigned int>(SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
			}
		}
		// Drop though if not selected

		switch (LOWORD(wParam)) {

		case IDC_MPEG4:
			EnableWindow(GetDlgItem(hDlg, IDC_QLOW),   FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_QMED),   FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_QHIGH),  FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CLOW),   FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CMED),   FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHIGH),  FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_PRESET), FALSE);
			break;

		case IDC_X264:
		case IDC_X265:
			EnableWindow(GetDlgItem(hDlg, IDC_QLOW),   TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_QMED),   TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_QHIGH),  TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CLOW),   TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CMED),   TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHIGH),  TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_PRESET), TRUE);
			break;

		case IDC_OPTIONS_HELP:
			{
				std::string str = "Image type\nImages of several types can be saved depending on the received texture format.\n";
				str += "8 bit unsigned, 16 bit unsigned, 16 bit float or 32 bit float.\n";
				str += "<a href=\"https://paulbourke.net/dataformats/pic/index.html\">High dynamic range</a> images can be saved for float textures ";
				str += "and <a href=\"https://sourceforge.net/projects/qtpfsgui/\">tone mapping</a> applied.\n";
				str += "   o High dynamic range format (hdr) is required for 32 bit floating point textures.\n";
				str += "   o Tif or Hdr are the only formats supported for 16 bit float textures.\n";
				str += "   o Png or Tif are the only formats supported for 16 bit unsigned textures.\n";
				str += "   o All formats other than Hdr are supported for 8 bit unsigned textures.\n";
				str += "Texture format details in the \"SpoutPanel\" sender selection dialog.\n\n";

				str += "File save\nOpen a file save dialog before recording video using F1. ";
				str += "For immediate recording, a file with the sender name ";
				str += "is saved in \"data\\videos\" and over-written if it exists\n\n";

				str += "Details\nShow file details after recording a video or saving an image.\n\n";

				str += "Duration\nRecord for a fixed amount of time. ";
				str += "\"Seconds\" - the time to record in seconds.\n\n";

				str += "Audio\nRecord system audio with the video, ";
				str += "A DirectShow <a href=\"https://github.com/rdp/virtual-audio-capture-grabber-device/\">virtual audio device</a>, ";
				str += "developed by Roger Pack, allows FFmpeg to record system audio together with the video. ";
				str += "Register it using \"VirtualAudioRegister.exe\" in the <a href=\"";
				str += g_ExePath;
				str += "\\data\\audio\\VirtualAudio\">\"VirtualAudio\"</a> folder.\n\n";

				str += "Codec\n";
				str += "<a href=\"https://trac.ffmpeg.org/wiki/Encode/MPEG-4\">Mpeg4</a> is a well established codec ";
				str += "that provides good video quality at high speed.\n";
				str += "<a href=\"https://trac.ffmpeg.org/wiki/Encode/H.264\">h264</a> is a modern codec with more control over ";
				str += "quality, encoding speed and file size.\n";
				str += "<a href=\"https://trac.ffmpeg.org/wiki/Encode/H.265\">h265</a> (High Efficiency Video Encoding) is similar to h264 but ";
				str += "produces smaller files, while retaining the same visual quality. Encoding time is greater but maintains 30fps at 1920x1080. Test with your system.\n\n";

				str += "Quality\n";
				str += "h264 constant rate factor CRF (0 > 51) : Low = 28, Medium = 23, High = 18. ";
				str += "High quality is effectively lossless, but will create a larger file. ";
				str += "Low quality will create a smaller file at the expense of quality. ";
				str += "Medium is the default, a balance between file size and quality.\n\n";

				str += "<a href=\"https://trac.ffmpeg.org/wiki/Chroma%20Subsampling\">Chroma subsampling</a>\n";
				str += "Low   : 4:2:0, chroma sampled horizontally and vertically at half rate. Most commonly used.\n";
				str += "Med  : 4:2:2, chroma sampled horizontally at half rate.\n";
				str += "High : 4:4:4, no chroma subsampling. Highest quality and default for h264 and h265.\n\n";

				str += "Preset\n";
				str += "h264 preset : Ultrafast, Superfast, Veryfast, Faster. ";
				str += "Presets affect encoding speed and compression ratio. ";
				str += "These are the presets necessary for real-time encoding. ";
				str += "Higher speed presets encode faster but produce progressively larger files. ";
				str += "If a slower preset is chosen to reduce file size, check that it does not ";
				str += "introduce hesitations or missed frames.\n\n";

				str += "Re-sending\n";
				str += "Send the received and adjusted texture. ";
				str += "Note that there may be latency compared to the received sender. ";
				str += "The dimensions are the same.\n\n";

				SpoutMessageBox(NULL, str.c_str(), "Options", MB_OK | MB_USERICON | MB_TOPMOST);
			}
			break;

		case IDC_OPTIONS_RESET:
		{
			extension = ".png";
			bPrompt = false;
			bShowFile = false;
			bDuration = false;
			SecondsDuration = 0L;
			bAudio = false;
			codec = 1;
			quality = 1;
			chroma = 2;
			preset = 0;
			imagetype = 0;
			bResend = false;
			strcpy_s(g_ReceiverName, 256, "Spout Receiver");
			SendMessage(hDlg, WM_INITDIALOG, 0, 0L);
		}
		break;

		case IDOK:
			bPrompt = false;
			bShowFile = false;
			bDuration = false;
			SecondsDuration = 0L;
			bAudio = false;
			bResend = false;
			codec = 1;
			quality = 1;
			chroma = 2;
			preset = 0;
			imagetype = 0;
			if (IsDlgButtonChecked(hDlg, IDC_PROMPT) == BST_CHECKED)
				bPrompt = true;
			if (IsDlgButtonChecked(hDlg, IDC_SHOWFILE) == BST_CHECKED)
				bShowFile = true;
			if (IsDlgButtonChecked(hDlg, IDC_DURATION) == BST_CHECKED)
				bDuration = true;
			GetDlgItemTextA(hDlg, IDC_SECONDS, (LPSTR)tmp, 256);
			SecondsDuration = atol(tmp);
			if (IsDlgButtonChecked(hDlg, IDC_AUDIO) == BST_CHECKED)
				bAudio = true;
			if (IsDlgButtonChecked(hDlg, IDC_X265) == BST_CHECKED)
				codec = 2;
			if (IsDlgButtonChecked(hDlg, IDC_X264) == BST_CHECKED)
				codec = 1;
			if (IsDlgButtonChecked(hDlg, IDC_QLOW) == BST_CHECKED)
				quality = 0;
			if (IsDlgButtonChecked(hDlg, IDC_QHIGH) == BST_CHECKED)
				quality = 2;
			if (IsDlgButtonChecked(hDlg, IDC_CLOW) == BST_CHECKED)
				chroma = 0;
			if (IsDlgButtonChecked(hDlg, IDC_CMED) == BST_CHECKED)
				chroma = 1;
			preset = presetindex;
			imagetype = imagetypeindex;
			extension = imagetypes[imagetype];
			if (IsDlgButtonChecked(hDlg, IDC_RESEND) == BST_CHECKED) bResend = true;
			GetDlgItemTextA(hDlg, IDC_RESEND_NAME, (LPSTR)g_ReceiverName, 256);
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



//
// Message handler for Adjustment options dialog
//
LRESULT CALLBACK UserAdjust(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str1[MAX_PATH]={};
	HWND hBar = NULL;
	LRESULT iPos = 0;
	float fValue = 0.0f;
	RECT wrect{};
	RECT drect{};
	int x = 0;
	int y = 0;
	bool bReopen = false;

	switch (message) {

	case WM_INITDIALOG:

		// Position to the left of the application window centre
		// so the effects can be seen, except for "Restore" and "Reset".
		// bReopen is initialized again when the dialog opens.
		if (!bReopen) {
			GetWindowRect(pThis->g_hWnd, &wrect);
			GetWindowRect(hDlg, &drect);
			x = wrect.left-(drect.right-drect.left)/2;
			// If too far left, centre on the window
			if (x < 0)
				x = wrect.left+((wrect.right-wrect.left)-(drect.right-drect.left))/2;
			y = wrect.top+((wrect.bottom-wrect.top)-(drect.bottom-drect.top))/2;
			SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE);
		}

		//
		// Set the scroll bar limits and text
		//

		// Brightness  -1 - 0 - 1 default 0
		// Range 2 Brightness*400 - 200
		hBar = GetDlgItem(hDlg, IDC_BRIGHTNESS);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)400);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)20);
		// -1 > +1   - 0 - 200
		// pos + 1 * 100
		iPos = (int)(Brightness+1*200.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Brightness);
		SetDlgItemTextA(hDlg, IDC_BRIGHTNESS_TEXT, (LPCSTR)str1);

		hBar = GetDlgItem(hDlg, IDC_CONTRAST);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)200);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Contrast*100.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Contrast);
		SetDlgItemTextA(hDlg, IDC_CONTRAST_TEXT, (LPCSTR)str1);

		// 0 > 4  - 0 - 400
		hBar = GetDlgItem(hDlg, IDC_SATURATION);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)400);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Saturation * 100.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Saturation);
		SetDlgItemTextA(hDlg, IDC_SATURATION_TEXT, (LPCSTR)str1);

		hBar = GetDlgItem(hDlg, IDC_GAMMA);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)200);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Gamma * 100.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Gamma);
		SetDlgItemTextA(hDlg, IDC_GAMMA_TEXT, (LPCSTR)str1);

		hBar = GetDlgItem(hDlg, IDC_TEMPERATURE);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)3500);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)9500);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Temp);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%4.0f", Temp);
		SetDlgItemTextA(hDlg, IDC_TEMPERATURE_TEXT, (LPCSTR)str1);

		hBar = GetDlgItem(hDlg, IDC_SHARPNESS);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)100);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Sharpness * 100.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Sharpness);
		SetDlgItemTextA(hDlg, IDC_SHARPNESS_TEXT, (LPCSTR)str1);

		hBar = GetDlgItem(hDlg, IDC_BLUR);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)400);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Blur * 100.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Blur);
		SetDlgItemTextA(hDlg, IDC_BLUR_TEXT, (LPCSTR)str1);

		hBar = GetDlgItem(hDlg, IDC_BLOOM);
		SendMessage(hBar, TBM_SETRANGEMIN, (WPARAM)1, (LPARAM)0);
		SendMessage(hBar, TBM_SETRANGEMAX, (WPARAM)1, (LPARAM)100);
		SendMessage(hBar, TBM_SETPAGESIZE, (WPARAM)1, (LPARAM)10);
		iPos = (int)(Bloom * 100.0f);
		SendMessage(hBar, TBM_SETPOS, (WPARAM)1, (LPARAM)iPos);
		sprintf_s(str1, 256, "%.3f", Bloom);
		SetDlgItemTextA(hDlg, IDC_BLOOM_TEXT, (LPCSTR)str1);

		// Sharpness width radio buttons
		// 3x3, 5x5, 7x7
		iPos = ((int)Sharpwidth-3)/2; // 0, 1, 2
		CheckRadioButton(hDlg, IDC_SHARPNESS_3x3, IDC_SHARPNESS_7x7, IDC_SHARPNESS_3x3+(int)iPos);

		// Adaptive sharpen checkbox
		if (bAdaptive)
			CheckDlgButton(hDlg, IDC_ADAPTIVE, BST_CHECKED);
		else
			CheckDlgButton(hDlg, IDC_ADAPTIVE, BST_UNCHECKED);

		// Option checkboxes
		if (bFlip)
			CheckDlgButton(hDlg, IDC_FLIP, BST_CHECKED);
		else
			CheckDlgButton(hDlg, IDC_FLIP, BST_UNCHECKED);
		if (bMirror)
			CheckDlgButton(hDlg, IDC_MIRROR, BST_CHECKED);
		else
			CheckDlgButton(hDlg, IDC_MIRROR, BST_UNCHECKED);
		if (bSwap)
			CheckDlgButton(hDlg, IDC_SWAP, BST_CHECKED);
		else
			CheckDlgButton(hDlg, IDC_SWAP, BST_UNCHECKED);

		return TRUE;

		// https://msdn.microsoft.com/en-us/library/windows/desktop/hh298416(v=vs.85).aspx
	case WM_HSCROLL:
		hBar = (HWND)lParam;
		if (hBar == GetDlgItem(hDlg, IDC_BRIGHTNESS)) {
			// 0 - 200 > -1 - +1
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos/200.0f)-1.0f;
			Brightness = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_BRIGHTNESS_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_CONTRAST)) {
			//  0 - 1 - 4 default 1
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos/100.0f);
			Contrast = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_CONTRAST_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_SATURATION)) {
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos)/100.0f;
			Saturation = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_SATURATION_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_GAMMA)) {
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos)/100.0f;
			Gamma = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_GAMMA_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_TEMPERATURE)) {
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos);
			Temp = fValue;
			sprintf_s(str1, 256, "%4.0f", fValue);
			SetDlgItemTextA(hDlg, IDC_TEMPERATURE_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_SHARPNESS)) {
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos)/100.0f;
			Sharpness = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_SHARPNESS_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_BLUR)) {
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos) / 100.0f;
			Blur = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_BLUR_TEXT, (LPCSTR)str1);
		}
		else if (hBar == GetDlgItem(hDlg, IDC_BLOOM)) {
			iPos = SendMessage(hBar, TBM_GETPOS, 0, 0);
			fValue = ((float)iPos) / 100.0f;
			Bloom = fValue;
			sprintf_s(str1, 256, "%.3f", fValue);
			SetDlgItemTextA(hDlg, IDC_BLOOM_TEXT, (LPCSTR)str1);
		}
		break;

	case WM_DESTROY:
		DestroyWindow(hwndAdjust);
		hwndAdjust = NULL;
		if (pThis->bFullScreen) {
			CURSORINFO info{};
			info.cbSize =sizeof(CURSORINFO);
			GetCursorInfo(&info);
			if (info.hCursor) ShowCursor(FALSE);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDC_SHARPNESS_3x3:
			Sharpwidth = 3.0;
			break;
		case IDC_SHARPNESS_5x5:
			Sharpwidth = 5.0;
			break;
		case IDC_SHARPNESS_7x7:
			Sharpwidth = 7.0;
			break;

		case IDC_ADAPTIVE:
			if (IsDlgButtonChecked(hDlg, IDC_ADAPTIVE) == BST_CHECKED)
				bAdaptive = true;
			else
				bAdaptive = false;
			break;

		case IDC_FLIP:
			if (IsDlgButtonChecked(hDlg, IDC_FLIP) == BST_CHECKED)
				bFlip = true;
			else
				bFlip = false;
			break;

		case IDC_MIRROR:
			if (IsDlgButtonChecked(hDlg, IDC_MIRROR) == BST_CHECKED)
				bMirror = true;
			else
				bMirror = false;
			break;

		case IDC_SWAP:
			if (IsDlgButtonChecked(hDlg, IDC_SWAP) == BST_CHECKED)
				bSwap = true;
			else
				bSwap = false;
			break;

		case IDC_RESTORE:
			Brightness = OldBrightness;
			Contrast   = OldContrast;
			Saturation = OldSaturation;
			Gamma      = OldGamma;
			Temp       = OldTemp;
			Blur       = OldBlur;
			Bloom      = OldBloom;
			Sharpness  = OldSharpness;
			Sharpwidth = OldSharpwidth;
			bAdaptive  = OldAdaptive;
			bFlip      = OldFlip;
			bMirror    = OldMirror;
			bSwap      = OldSwap;
			bReopen    = true;
			SendMessage(hDlg, WM_INITDIALOG, 0, 0L);
			break;

		case IDC_RESET:
			Brightness = 0.0; // -1 - 1 default 0
			Contrast   = 1.0; //  0 - 2 default 1
			Saturation = 1.0; //  0 - 4 default 1
			Gamma      = 1.0; //  0 - 2 default 1
			Temp       = 6500.0; // 3500 - 9500 default 6500 (daylight)
			Blur       = 0.0; //  0 - 4 default 0
			Bloom      = 0.0; //  0 - 1 default 0
			Sharpness  = 0.0; //  0 - 1 default 0
			Sharpwidth = 3.0; //  3,5,7 default 3
			bAdaptive  = false;
			bFlip      = false;
			bMirror    = false;
			bSwap      = false;
			bReopen    = true;
			SendMessage(hDlg, WM_INITDIALOG, 0, 0L);
			break;

		case IDOK:
			DestroyWindow(hwndAdjust);
			hwndAdjust = NULL;
			return TRUE;

		case IDCANCEL:
			Brightness = OldBrightness;
			Contrast   = OldContrast;
			Saturation = OldSaturation;
			Gamma      = OldGamma;
			Temp       = OldTemp;
			Blur       = OldBlur;
			Bloom      = OldBloom;
			Sharpness  = OldSharpness;
			Sharpwidth = OldSharpwidth;
			bAdaptive  = OldAdaptive;
			bFlip      = OldFlip;
			bMirror    = OldMirror;
			bSwap      = OldSwap;
			DestroyWindow(hwndAdjust);
			hwndAdjust = NULL;
			return TRUE;

		default:
			return FALSE;
		}
		break;

	}

	return FALSE;
}

#endif

// the end ...
