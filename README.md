# ofSpoutDemo
Spout Openframeworks Demo program

Note that this program is not the same as the demo programs provided with the Spout distribution and is intended as an example of using Spout library functions and creating a menu using the [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) addon. The receiver also includes image capture as well as video recording functions using [SpoutRecorder](https://github.com/leadedge/SpoutRecorder).

## Binaries

The latest [Release](https://github.com/leadedge/ofSpoutDemo/releases) contains all files required. Refer to the individial folder readme files.

## Receiver

### File menu
* Save image\
Saves images of the required name and type (jpg, png, bmp etc). Images are saved in bin\data\captures.

### View menu
* Open capture folder
* Open video folder\
Opens file explorer to show captured images and recorder videos. Videos are saved in bin\data\videos.

### Help > Options
Opens a dialog with capture and recording options.

Capture
* Image type - png or tif\
Tif images can be produced instead of default png if required and may be useful if the sender is producing 16 bit texture format. The format is shown by the on-screen display or further details are in the "SpoutPanel" sender selection dialog.

Recording
* Codec - mpeg4 or x264\
x264 codec can be used instead of default Mpeg4. Compatibility and quality may be improved, although the encoding speed may be reduced slightly. To check, click on the SpoutRecoder taskbar icon while recording. FFmpeg encoding speed is shown in the console window. You should see a speed of 1.0 if the encoding is keeping pace with the input frame rate. Minimize the recorder console when done.
* Pixel format - RGB or RGBA\
Pixel data is RBGA by default. This provides maximum epeed for capture from video memory. This option can be selected if RGB pixel type is required for compatibility with other software.
* Audio - record with system audio\
Audio from the system audio device (speakers) is recorded along with the video using a [virtual audio device](https://github.com/rdp/virtual-audio-capture-grabber-device) developed by Roger Pack. The audio filters must be registered. Refer to the "bin\data\audio" folder for details.
* File name - prompt for video file name\
Videos are normally saved using the sender name to bin>data>videos and over-written if the file exists. Select this option if another destination or video file name is required.\
* HELP button\
Shows details of the options.

### Window menu
* Show on top\
Make the window topmost.
* Preview\
Remove menu, caption and borders and keep the sender display topmost and at the position of the window.
* Full screen\
Full screen display on the monitor wher the program was executed.
* Show info\
Show or hide the on-screen display.

## Building the project

The project requires the files from the Spout SDK "beta" branch as detailed below.

Uses the [ofxSkybox](https://github.com/kalwalt/ofxSkyBox) addon\
Created by kalwalt alias Walter Perdan

Uses the [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) addon.

The source for both of these is modified and included with the project.

### Setup required for For Visual Studio 2022 and Openframeworks 11.2
  
  1) Download [Openframeworks for Windows and Visual Studio](https://openframeworks.cc/download)
  2) Unzip the files to get a folder containing Openframeworks\
       e.g. "of_v0.11.2_vs2017_release.zip"
  3) Copy the folder to a convenient location and rename it as you wish\
       e.g. somefolder\OPENFRAMEWORKS11
  4) Download the ["beta"](https://github.com/leadedge/Spout2/tree/beta) branch of the Spout SDK
  5) Copy the "SpoutGL" folder into the Openframeworks "apps" folder
  6) Download this repository
  7) Copy the "ofSpoutDemo" folder and contents to the Openframeworks "apps\myApps" folder.
  
You will then have a folder structure as follows :
  
OPENFRAMEWORKS11\
&nbsp;&nbsp;&nbsp;&nbsp;addons\
&nbsp;&nbsp;&nbsp;&nbsp;examples\
&nbsp;&nbsp;&nbsp;&nbsp;apps\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SpoutGL <- the Spout SDK source files\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;myApps\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;emptyExample <- Openframeworks example template\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ofSpoutDemo <- the ofSpoutDemo project folder\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ofSpoutDemo.sln\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ofSpoutDemo.vcxproj\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ofSpoutDemo.vcxproj.filters\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;src <- source files\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Addons <- Modifed source of addons\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ofxSkyBox\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ofxWinMenu\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;bin <- executable and required dlls\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Data <- application data\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fonts\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;images\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;shaders
                 
Open "ofSpoutDemo.sln" with Visual Studio 2022 and set to "Release"\
 
The first time you use Openframeworks you have to compile the library\
RH click on "openframeworksLib" - Build\
&nbsp;&nbsp;&nbsp;&nbsp;when it is compiled :\
RH click on "ofSpoutDemo" - Build\
&nbsp;&nbsp;&nbsp;&nbsp;ofSpoutDemo.exe will be in the "bin" folder\
Rename it as required. For example : ofSpoutSender.exe or ofSpoutReceiver.exe

### Build options :

You can build the project either as a sender or receiver\
Find in ofApp.h :\
&nbsp;&nbsp;&nbsp;// Change to create a sender or a receiver\
&nbsp;&nbsp;&nbsp;// #define BUILDRECEIVER

### Modifications

The code may contain functions which are not documented. As many comments as possible are included in the source files.



