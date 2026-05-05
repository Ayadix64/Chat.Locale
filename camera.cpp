#include "camera.hpp"
#include "utils.hpp"
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <thread>
#include <unistd.h>





std::function<void(unsigned int width,unsigned int heigth, unsigned char* data )> cameraCapturingReq = [](auto,auto,auto){return;};



std::thread sendThread;
std::thread capturingThread;

std::atomic<bool> ThreadsSholdExit = false;

bool CameraHaveBeenChanged = false;
unsigned int CameraNumber = 0;

std::atomic<bool> HaveToSendeImage=false;

unsigned char* data = NULL;
std::atomic<unsigned int> h = 0 , w = 0;

cv::VideoCapture cam;

void SendingThreadFn(){
	while(!ThreadsSholdExit){
		if(data!=NULL && HaveToSendeImage){
			cameraCapturingReq(w,h,data);
			HaveToSendeImage=false;
		}
	}
	/* why we put a sending thread seperet frome a recording thread?            *
	 * you see , when it sends to a mulitpule devices in the network , we       *
	 * expect a deley of sending, wich sends the old frame for evry device      *
	 * wich may , in a defrent condetions makes a sound deley frome the image   *
	 * so, this , even thig of the machine is not capebele of sending at hiegth *
	 * frame rate, we will have the most acurute frame.                         */
	return;
}


void CapturingThrFn(){
	cv::Mat buffer;
	cv::Mat frame;
	while (!ThreadsSholdExit) {
		if(cam.isOpened()){
			cam.read(buffer);

			cv::cvtColor(buffer, frame,cv::COLOR_BGR2RGB);

			if(CameraHaveBeenChanged){
				if(data!=NULL){
					free(data);
				}

				data=NULL;
				h=frame.rows;
				w=frame.cols;
				data = (unsigned char*)malloc(h*w*3);
				memset(data, 0, h*w*3);

				CameraHaveBeenChanged=false;
			}
			if(data!=NULL){
				memcpy(data, frame.data, h*w*3);
				HaveToSendeImage=true;
			}

		}
	}

}

void CameraStart(){
	cam.open(CameraNumber);
	
	if(cam.isOpened()){
		CameraHaveBeenChanged=true;
		ThreadsSholdExit=false;

		capturingThread = std::thread(CapturingThrFn);
		sendThread = std::thread(SendingThreadFn);
	}else {
		logMsgsErr("CANT OPEN CAMERA #" + std::to_string(CameraNumber) + (!CameraNumber ? ", aka. The Main One, my be you dont have a camera?" : ""));
	}

}

void CameraStart(unsigned int Cn){
	CameraNumber=Cn;
	CameraStart();
}

void SetCamera(unsigned int ca){
	if(ca!=CameraNumber){
		cam.open(ca);
		if(cam.isOpened()){
			CameraNumber=ca;
			CameraHaveBeenChanged=true;
		}
		else {
			logMsgsErr("CAN NOT OPEN CAMERA #" + std::to_string(ca));
			cam.open(CameraNumber);
		}
	}

}


void CameraBlankout(){
	if(cam.isOpened()){
		cam.release();
	}
	if(data!=NULL){
		memset(data, 0, h*w*3);
	}
	return;
}

void CameraReOpen(){
	cam.open(CameraNumber);
	if(cam.isOpened()){
		logMsgs("CAMERA #"+std::to_string(CameraNumber) , "secusful reopend");
	}else{
		logMsgsErr("when reopning camera #" + std::to_string(CameraNumber) + (!CameraNumber ? " aka. defult Camera":""));
	}
	return;
}



void CameraStop(){
	ThreadsSholdExit = true;
	if(sendThread.joinable()){
		sendThread.join();
	}
	if(capturingThread.joinable()){
		capturingThread.join();
	}
	if(cam.isOpened()){
		cam.release();
	}
	logMsgs("Camera Closed Secsusfuly");
	return;
}
