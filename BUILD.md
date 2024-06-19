## Building the project

Although this project is open source, it is dependent on Openframeworks and Visual Studio. 
If you familiar with Visual Studio, the project solution file "ofSpoutDemo.sln" can be used to build both Openframeworks and the project.

However, the project can also be built without the Visual Studio IDE using "MSBuild" with only one line from a command window. The following document shows how to do this without requiring any previous knowledge of Visual Studio or Openframeworks.

### Openframeworks

#### Download

- Download from https://openframeworks.cc/download/
- Choose download openFrameworks for "visual studio"
- Save the zip file to a convenient location for the build.
- Right click on the zip file and "Extract all".
- Remove "of_v0.12.0_vs_release" from the end of the extract path.<br>
  This is a very large file and it can take some time to download.

Finally you will have a new folder "of_v0.12.0_vs_release" with several sub-folders.
The important one is "apps"
<pre>
apps
  myApps
    emptyExample
</pre>
	  
"myApps" is where the projects are to be located.<br>
"emptyExample" is an example project to start with.

## The "ofSpoutDemo" project

### Download the project

- Go to the root of the repository https://github.com/leadedge/ofSpoutDemo
- Click the green "Code" button
- Scroll down and click "Download Zip"
- If the browser allows you to choose a destination, download it to<br>
 the Openframeworks project folder created above "of_v0.12.0_vs_release\apps\myApps"
- If not, wait for the download to complete and move the zip file to that folder.
- Right click and "Extract all"
- Before extracting, remove "ofSpoutDemo-master" from the destination folder name.<br>
This will create a folder "ofSpoutDemo-master" under "myApps".
- Remove "-master" from the folder name to create "ofSpoutDemo".
- Now there will be a new project folder under "myApps".

<pre>
  apps
    myApps
      emptyExample
      ofSpoutDemo
 </pre>

Now that all the code is in place, it's ready for building with either Visual Studio or MSBuild.

 ## Visual Studio
 
If you have Visual Studio installed, open "ofSpoutDemo.sln" and set to "Release".
The first time you use Openframeworks you have to compile the library.
Right click on "openframeworksLib" in the project explorer and "Build".
Now you can build the project. Refer to the MSBuild instructions below for building a sender or receiver.

   
## MSBuild

If you are not familiar with Visual Studio and do not have it installed, you can still build the project using MSBuild to automate the process.

### Download

Download from :

https://visualstudio.microsoft.com/visual-cpp-build-tools/

Click "Download Build Tools" and save the file (vs_BuildTools.exe).

### Installation

- Run "vs_BuildTools.exe"
- "Visual Studio Installer" will appear but this will<br>only install the Visual Studio Build Tools.
- Click "Continue" for the getting started message and wait for completion.
- Click on "Desktop development with C++" at top left in the installation window.
- Then click "Install" at bottom right and wait for completion of the installation.

### Environment path

Confirm the location of MSBuild.exe at :

C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin

MsBuild is run from the command line from a console window. Create an Environment Variable with this path so that you don't have to enter the full path all the time.

- In the search window enter "Environment" and choose<br>"Edit the system environment variables"
- Click the "Environment variables" button.
- In the top pane you will see the "Path" variable.
- Double click this to bring up the "Edit environment variable" window.
- Click "New" and enter the path as found above. Most likely it will be :

C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin

- Click OK to enter the new path and OK again to exit.

Now MSbuild is ready to run from a console window without having to enter the full path.


## Build the Openframeworks library

The Visual Studio project for Openframeworks is "openframeworksLib.vcxproj"<br>
and is located in :

of_v0.12.0_vs_release\libs\openFrameworksCompiled\project\vs\

- In File Explorer left click on the address bar to highlight this location
- Type cmd to replace the highlighted text and press enter.<br>
A command window will open in that folder.
- Now type in or copy/paste :

MSBuild openframeworksLib.vcxproj -p:Configuration=RELEASE -p:Platform=x64

- Enter to build.

You might see warnings, but the important thing after the build is complete is "0 Errors".
Now the Openframeworks library is compiled.

	  
## Build the demo project

In File Explorer move to the project folder within Openframeworks :

"of_v0.12.0_vs_release\apps\myApps\ofSpoutDemo" 

- As before, left click on the address bar to highlight the location
- Type "cmd" to replace the highlighted text and enter.
- Type in or copy/paste :

MSBuild ofSpoutDemo.sln -p:Configuration=RELEASE -p:Platform=x64

- Press enter to build.

The build should succeed with 0 Warnings and 0 Errors.<br>
You will find "ofSpoutDemo.exe" in the "ofSpoutDemo" folder.<br>
This is a sender, rename the file to "SpoutSender.exe".

### Building a receiver

- Find the file "of_v0.12.0_vs_release\apps\myApps\ofSpoutDemo\src\resource.h"
- Open the file with a text editor<br>(Right click and choose "Edit" to use Windows Notepad).
- Find the line : // #define BUILDRECEIVER
- Remove the preceding comments to give : #define BUILDRECEIVER.
- Save the file and exit Notepad.
- The command window is already open in the project folder.
- Use the same command :<br>

MSBuild ofSpoutDemo.sln -p:Configuration=RELEASE -p:Platform=x64

- Press Enter to build.

Rename "ofSpoutDemo.exe" in the "ofSpoutDemo" folder to "SpoutReceiver.exe"

The project build is now complete.

