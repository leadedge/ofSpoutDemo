# ofSpoutDemo
Spout OpenFrameworks Demo program

Please note that this program is designed to test Spout development code and is not the same as the demo programs provided with the Spout distribution. Nevertheless it could also be useful for other purposes.

The project requires the files from the Spout SDK "develop" branch as detailed below.

Uses the [ofxSkybox](https://github.com/kalwalt/ofxSkyBox) addon\
Created by kalwalt alias Walter Perdan

Uses the [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) addon.

The source for both of these is modified and included with the project.

### Setup required for For Visual Studio 2017 and Openframeworks 11.
  
  1) Download [Openframeworks for Windows and Visual Studio](https://openframeworks.cc/download)
  2) Unzip the files to get a folder containing Openframeworks\
       e.g. "of_v0.11.0_vs2017_release"
  3) Copy the folder to a convenient location and rename it as you wish\
       e.g. somefolder\OPENFRAMEWORKS11
  4) Download the ["develop"](https://github.com/leadedge/Spout2/tree/develop) branch of the Spout SDK
  5) Copy the entire "SpoutSDK" folder into the Openframeworks "apps" folder
  6) Download this repository
  7) Copy the "ofSpoutDemo" folder and contents to the Openframeworks "apps\myApps" folder.
  
You will then have a folder structure as follows :
  
OPENFRAMEWORKS11\
&nbsp;&nbsp;&nbsp;&nbsp;addons\
&nbsp;&nbsp;&nbsp;&nbsp;examples\
&nbsp;&nbsp;&nbsp;&nbsp;apps\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SpoutSDK <- the Spout SDK source files\
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
                 
Open "ofSpoutDemo.sln" with Visual Studio 2017 and set to "Release"\
 
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

The code may contain experimental functions which are not documented\
As many comments as possible are included in the source files.


