#ifndef CAMERA
#define CAMERA

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <stdio.h>


extern std::function<void(unsigned int width,unsigned int heigth, unsigned char* data )> cameraCapturingReq;

void CameraStart();

void CameraStart(unsigned int Cn);

void SetCamera(unsigned int ca);

void CameraStop();

void CameraBlankout();
void CameraReOpen();


#endif
