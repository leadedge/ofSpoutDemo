# ofSpoutDemo
Spout Openframeworks Demo program

This program is intended as an example of using Spout library functions and using additional features such as creating a menu using the [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) addon, the "SpoutRecord" class for recording video with FFmpeg and the "SpoutShaders" class for high speed, live image adjustment using compute shaders.

The receiver includes image capture, video recording, image adjustment and dialogs to manage them. 16 bit and floating point textures can be received from applications that support them and multipe image types can be saved, including [High dynamic range](https://paulbourke.net/dataformats/pic/index.html) for 32bit float textures.

## Binaries

The latest [Release](https://github.com/leadedge/ofSpoutDemo/releases) contains sender and receiver applications. FFmpeg.exe is additionally required. Refer to the individial folder readme.txt files.

## Addons

The project uses [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) addon and the [ofxSkybox](https://github.com/kalwalt/ofxSkyBox) addon created by kalwalt alias Walter Perdan. The source for both of these is modified and included with the project.

## Building the project

Refer to "BUILD.md" for building with either Visual Studio or MSBuild.




