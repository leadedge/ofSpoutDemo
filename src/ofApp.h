/*

	Spout OpenFrameworks Demo program

	Copyright (C) 2020-2021 Lynn Jarvis.

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

#define BUILDRECEIVER

#include "ofMain.h"
#include "Addons\ofxSkybox\ofxSkyBox.h" // Skybox addon
#include "Addons\ofxWinMenu\ofxWinMenu.h" // Windows menu addon
#include "resource.h"

#ifdef BUILDRECEIVER
#include "..\apps\SpoutGL\SpoutReceiver.h"
#else
#include "..\apps\SpoutGL\SpoutSender.h"
#endif

// For about box graphics adapter display
static int adapterIndex; // Current graphics adapter index
static char adapterName[256]; // Current graphics adapter name

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

		char senderName[256];  // Sender name
		unsigned int senderWidth; // Dimensions of the sender can be independent
		unsigned int senderHeight; // of the application window if using an fbo

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
		bool EnterSenderName(char *SenderName, char *caption);

		ofTrueTypeFont myFont;
		ofxWinMenu * menu;
		ofxSkyBox skybox;
		ofEasyCam easycam;

};
