# ofSpoutDemo
Spout Openframeworks Demo program

This program is intended as an example of using Spout library functions and using additional features such as creating a menu using the [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) addon, the "SpoutRecord" class for recording video with FFmpeg and the "SpoutShaders" class for high speed, live image adjustment using compute shaders.

The receiver includes image capture, video recording, image adjustment and dialogs to manage them. 16 bit and floating point textures can be received from applications that support them and multipe image types can be saved, including [High dynamic range](https://paulbourke.net/dataformats/pic/index.html) for 32bit float textures.

## Binaries

The latest [Release](https://github.com/leadedge/ofSpoutDemo/releases) contains sender and receiver applications. FFmpeg.exe is additionally required. Refer to the individial folder readme.txt files.

## Sender

### File menu
* Sender name\
Enter a name for the sender instead of the default "Spout Sender".
* Sender format\
Select the OpenGL format for the sender.

### Window menu
* Show on top\
Make the window topmost.
* Show info\
Show or hide the on-screen display.

## Receiver

### File menu
* Save image\
Enter a file name to capture with an extension of the required type (jpg, png, bmp etc).\
Default folder is "bin\data\captures".
* Save video\
Enter a file name to record.\
Default folder is "bin\data\videos".

### View menu
#### Adjust
Live image adjustment
#### Options
Opens a dialog with capture and recording options.
* Image type\
Multiple image types are supported. See "Help" for details.
* File save\
Open a "File Save" dialog to select the name and destination folder for the video file for F1 record.
* File name\
Show file information after video recording or image capture.
* Duration\
Record for a fixed amount of time. Enter the number of seconds to record.
* Audio - record with system audio\
Audio from the system audio device (speakers) is recorded along with the video using a [virtual audio device](https://github.com/rdp/virtual-audio-capture-grabber-device) developed by Roger Pack. The audio filters must be registered. Refer to the "bin\data\audio" folder for details.
* Codec - mpeg4 or h264\
h264 codec can be used instead of default Mpeg4.
* Quality - h264 quality\
h264 constant rate factor CRF (0 > 51) : low = 28, medium = 23, high = 18.
* Preset - h264 preset\
ultrafast, superfast, veryfast, faster
* Re-send\
Enable re-send of the received and processed texture\
Enter the name for the sender. Different from the received sender.
* HELP button\
Shows details of the options.
#### Capture folder / Video folder
Opens file explorer to show captured images and recorded videos.

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

### Setup required for For Visual Studio 2022 and Openframeworks
  
  1) Download [Openframeworks for Windows and Visual Studio](https://openframeworks.cc/download)
  2) Download for the Visual Studio. For example, this could be "of_v0.12.0_vs_release.zip"
  4) Unzip the files to get a folder containing Openframeworks
  5) Copy the folder to a convenient location and rename it as you wish\
       e.g. somefolder\OPENFRAMEWORKS120
  6) Download the ["beta"](https://github.com/leadedge/Spout2/tree/beta) branch of the Spout SDK
  7) Copy the "SpoutGL" folder into the Openframeworks "apps" folder
  8) Download this repository
  9) Copy the "ofSpoutDemo" folder and contents to the Openframeworks "apps\myApps" folder.
  
You will then have a folder structure as follows :
  
OPENFRAMEWORKS120\
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
                 
Open "ofSpoutDemo.sln" with Visual Studio 2022 and set to "Release".
 
The first time you use Openframeworks you have to compile the library.\
RH click on "openframeworksLib" - Build\
&nbsp;&nbsp;&nbsp;&nbsp;when it is compiled :\
You can build the project either as a sender or receiver\
Find in resource.h :\
&nbsp;&nbsp;&nbsp;// Enable this define to create a receiver rather than a sender\
&nbsp;&nbsp;&nbsp;// #define BUILDRECEIVER\
Remove the preceding comment to buld a receiver rather than a sender.\
RH click on "ofSpoutDemo" - Build\
&nbsp;&nbsp;&nbsp;&nbsp;ofSpoutDemo.exe will be in the "bin" folder.\
Rename it as required. For example : ofSpoutSender.exe or ofSpoutReceiver.exe

### Modifications

The code may contain functions that are not documented. As many comments as possible are included in the source files.




