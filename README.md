# Chat.Locale
<img src="Picturs/p1.png"></img>

A cross-platform multi-threading peer-to-peer chatting app programmed in C++

This app is still in alpha, also known as prototype, and is not stable

### It uses:
- ASIO as the network library
- wxWidgets as the GUI library
- OpenCV for camera capturing
- PortAudio for audio capturing
- LZ4 as a comprising methode for camera sending

### It can:
- video/audio chating
- Send text messages
- Send files

### Futurs
- end to end messaging
- runs localy at your machine
- audio/image lz4 comprassing
- cross platform (linux/macos/bsd's/windows)
- multi threading uses


The code itself is in **ALPHA** stage, but I'm still developing it to make it as stable as possible

## Installing
- First of all, download the source file. For sure, you will have source and download zip up there
To the cool terminal guys
``` bash
git clone https://github.com/chamseddine2009-06/Chat.Locale.git
cd Chat.Locale
```
# Install the Dependencies
### Linux
- For Arch systems:
``` bash
  sudo pacman -Sy asio wxwidgets-gtk3 opencv portaudio
```
- For Debian/Ubuntu systems:
``` bash
sudo apt update
sudo apt-get install libwxgtk3.0-gtk3-dev libopencv-dev portaudio19-dev asio-dev
```
- If you have another distro, you will probably have the same libraries with the same or different naming
### BSD systems
``` bash
sudo pkg install opencv portaudio wx32-gtk3 asio
```
You will probably have the *pkg* package manager for every BSD distro, so this may run on every one

### macOS
``` bash
brew install wxwidgets opencv portaudio asio
```
Probably something like that

### Windows
Try this:
``` bash
vcpkg install wxwidgets:x64-windows opencv:x64-windows portaudio:x64-windows asio:x64-windows
```
I don't know. It's literally been a year since the last time I developed on Windows, even though I'm pretty sure that my code is supported on Windows

### For those it didn't work for
I hope you know how to compile your libraries.
Here are some useful links:
- wxWidgets website: https://wxwidgets.org/
- ASIO website: https://think-async.com/Asio/ (yes, for those who know, use the non-boost one)
- PortAudio GitHub: https://github.com/PortAudio/portaudio
- OpenCV GitHub: https://github.com/opencv/opencv
- A coffee

## Compiling
Note: you need CMake ready on your machine
``` bash
cd Chat.Locale
mkdir build
cd build
cmake ..
make
#make -j to who have multi threading systems
```
And you will have a *main* file in your folder to go.

## After installing
1- you will need to specefy your device range in the network descovry up in the satings and press "Update Satings"(or in the user.conf file)
2- then restart the app
## Notes&Bugs:
- after evry satings changing , you may need to restart the app (working on it)
- somtimes network discovring shows blank text (somtime)
## Future Plane
- re writing the gui side with a defrent library (sorry wxWidgets fans, but my brain cant geet the parncipile, i am more in low level to get this)
- add a good text message gui
