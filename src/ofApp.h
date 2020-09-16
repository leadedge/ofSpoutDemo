/*

	Spout OpenFrameworks Demo program

	Copyright (C) 2020 Lynn Jarvis.

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

#include "ofMain.h"
#include "..\apps\SpoutSDK\Spout.h"
#include "Addons\ofxSkybox\ofxSkyBox.h" // Skybox addon
#include "Addons\ofxWinMenu\ofxWinMenu.h" // Windows menu addon
#include "Nvapi\SpoutOptimus.h" // NVIDIA profile settings
#include "resource.h"

// Change to create a sender or a receiver
#define BUILDRECEIVER

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void exit();
		void mousePressed(int x, int y, int button);
		void keyPressed(int key);
		void windowResized(int w, int h);

#ifdef BUILDRECEIVER
		SpoutReceiver receiver;
		ofTexture myTexture; // Receiving texture
#else
		SpoutSender sender;
#endif
		bool bInitialized = false; // TODO
		char senderName[256];  // Sender name
		unsigned int senderWidth; // Dimensions of the sender can be independent
		unsigned int senderHeight; // of the application window if using an fbo
		bool bMemoryMode; // Memory or texture share user selected
		int shareMode; // Share mode selected by the user (0, 1, 2 : texture,memory,auto)

		ofImage myBoxImage;    // Image for the 3D demo
		ofFbo myFbo;           // For texture sharing
		float rotX, rotY;

		bool bTopmost;
		bool bFullScreen;
		bool bShowInfo;

		// These are all for restoring from full screen
		RECT windowRect; // Window rectangle
		RECT clientRect; // Client rectangle
		int nonFullScreenX; // Window position
		int nonFullScreenY;
		unsigned int AddX, AddY; // adjustment to client rect for reset of window size
		HWND g_hwndForeground; // foreground window before setting topmost
		HWND g_hwndTopmost; // topmost window before setting full screen
		HMENU g_hMenu; // Original menu
		DWORD g_dwStyle; // Original style
		
		void appMenuFunction(string title, bool bChecked);
		void doFullScreen(bool bFullscreen);
		bool EnterSenderName(char *SenderName);
		void SelectAdapter();

		// for diagnostics
		nVidia g_NvApi;
		bool DoDiagnostics();
		bool GLDXready(bool bDX9);
		bool CheckForDirectX9c();
		void trim(char* s);

		int g_ThreadedOptimization = 0;
		bool bLaptop = false; // NVAPI detection of a laptop
		bool bIntegrated = false; // Laptop using integrated GPU
		int NvidiaMode = -1; // Optimus graphics mode
		unsigned int NvidiaDriverVersion = 0; // Driver Version
		char NvidiaBuildBranchString[128];
		unsigned int DriverPrimary = 0;
		unsigned int DriverSecondary = 0;

		ofTrueTypeFont myFont;
		ofxWinMenu * menu;
		ofxSkyBox skybox;
		ofEasyCam easycam;

};
