# Plan.Locale
<img src="Picturs/p1.png"></img>

A cross platform multi-threading pear to pear chating app was programed in C++

this  app still on alpha, aka prototype, aka not stabel

it uses:
- ASIO as the network library
- wxWidgets as the gui library
- OpenCV for camera capturing
- PortAudio for Audio capturing

it can:
- share camera video / audio
- sende mesages 
- sende files

*AND No Server betwen them, you device is a server and a cleint in the same time*

the code it selfe is expermentale, but i still devolping it to make it as stabel as posible

tbh, what do you think, i speed run makeing it for a competition, and it is probably the worst code that i ever made

## Instaling
-first of all , download the sorce file, for shore, you will have source and download zip up ther
to the cool terminal guys
``` bash
git clone https://github.com/chamseddine2009-06/Plan.Locale.git
cd Plan.Locale
```
# Install the Dependences
### Linux
- for arch systems:
``` bash
  sudo pacman -Sy asio wxwidgets-gtk3 opencv portaudio
```
- for Debian/Ubuntu systems:
``` bash
sudo apt update
sudo apt-get install libwxgtk3.0-gtk3-dev libopencv-dev portaudio19-dev asio-dev
```
- if you have a other destro , you will probably have the sime librarys in the same or a defrent naming
### BSD systems
``` bash
sudo pkg install opencv portaudio wx32-gtk3 asio
```
you will probably have the *pkg* pckage manager for evry BSD destro, so this may run on evry one
### MacOS
``` bash
brew install wxwidgets opencv portaudio asio
```
probably somthing like that

### Windows
try this:
``` bash
vcpkg install wxwidgets:x64-windows opencv:x64-windows portaudio:x64-windows asio:x64-windows
```
i dont know , i have letrly a years since the laste time that i develope in windows, even thog that i am pretty shore that my code is sepurted in windows
### To who it didnt work
i hope that you know how to compile your librarys.
this is a usefule links
- wxWidgets webset: https://wxwidgets.org/
- ASIO webset: https://think-async.com/Asio/ (yes to who knows , use the un-boot one)
- PortAudio github: https://github.com/PortAudio/portaudio
- OpenCV github : https://github.com/opencv/opencv
- a coffe
## Compiling
note : you need cmake ready on your machine
``` bash
cd Plan.Lovcal
mkdir build
cd build
cmake ..
```
and you will have a *main* file in your folder to go.
