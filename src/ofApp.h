/*

	Spout OpenFrameworks Demo program

	Copyright (C) 2020-2024 Lynn Jarvis.

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#pragma once

// To avoid Openframeworks warning
// "Boost.Config is older than your compiler version"
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#include "ofMain.h"
#include "resource.h"
#include "commdlg.h"


#include "Addons\ofxSkybox\ofxSkyBox.h" // Skybox addon
#include "Addons\ofxWinMenu\ofxWinMenu.h" // Windows menu addon
#include "..\apps\SpoutGL\Spout.h" // Common for receiver and sender
#include "SpoutRecord.h" // FFmpeg recording
#include "SpoutShaders.h" // Compute shaders

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void exit();
		void mousePressed(int x, int y, int button);
		void keyPressed(int key);
		void windowResized(int w, int h);

		ofxWinMenu* menu{};
		ofTrueTypeFont myFont{};

		// For about box
		int adapterIndex; // Current graphics adapter index
		char adapterName[256]; // Current graphics adapter name
		int spoutVersion = 0;
		HINSTANCE g_hInstance;
		HWND g_hWnd;

		Spout sender; // For either sender or receiver re-send
		char senderName[256]{};
		unsigned int senderWidth = 0; // Dimensions of the sender can be independent
		unsigned int senderHeight = 0; // of the application window if using an fbo
		double g_SenderFps = 0.0; // For fps display averaging
		ofFbo myFbo{}; // For texture sharing
		GLint glFormat = GL_RGBA; // Sender OpenGL texture format

		bool bTopmost = false;
		bool bFullScreen = false;
		bool bPreview = false;
		bool bShowInfo = true;

		// These are all for restoring from full screen
		RECT windowRect{}; // Window rectangle
		RECT clientRect{}; // Client rectangle
		int nonFullScreenX = 0; // Window position
		int nonFullScreenY = 0;
		unsigned int AddX, AddY = 0; // adjustment to client rect for reset of window size
		HWND g_hwndForeground = NULL; // foreground window before setting topmost
		HWND g_hwndTopmost = NULL; // topmost window before setting full screen
		HMENU g_hMenu = NULL; // Original menu
		DWORD g_dwStyle = 0; // Original style

		// Common functions
		bool LoadWindowsFont(ofTrueTypeFont& font, std::string name, int size);
		void appMenuFunction(std::string title, bool bChecked);
		void DrawString(std::string str, int posx, int posy);
		
#ifdef BUILDRECEIVER

		Spout receiver;
		char receiverName[256]{}; // Re-sending name

		ofTexture myTexture; // Receiving texture
		spoutShaders shaders; // For image adjust
		ofImage myImage; // For image load
		void doFullScreen(bool bEnable, bool PreviewMode);

		// Recording
		spoutRecord recorder;
		char g_Initfile[MAX_PATH]={0};
		std::string g_FFmpegPath;
		std::string g_FFmpegCodec;
		std::string g_OutputFile;
		std::string g_OutputImage;
		bool bRecording = false; // Recording status
		double StartRecordingTime = 0;
		double ElapsedRecordingTime = 0.0f;
		bool StartRecording(bool prompt = false);
		void StopRecording();
		std::string EnterFileName(int type);
		void WriteInitFile(const char* initfile);
		void ReadInitFile(const char* initfile);
		void SaveImageFile(std::string savepath);

#else
		ofxSkyBox skybox{};
		ofEasyCam easycam{};
		ofImage myBoxImage{}; // Image for the sender cube
		bool EnterSenderName(char* name, const char* caption);
		bool SelectSenderFormat();
		float rotX = 0;
		float rotY = 0;
		bool LoadSkyboxImages();
		bool LoadResourceImage(ofImage& image, int index);
		ofImage skyImage[6]; // Images for skybox cubemap
#endif


};
