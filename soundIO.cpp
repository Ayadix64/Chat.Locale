#include <cstdlib>
#include <cstring>
#include <functional>
#include <thread>
#define SOUND_IO_CPP
#include "soundIO.hpp"
#include "utils.hpp"
#include "networking.hpp"
#include <portaudio.h>


#define SAMPEL_RATE 44100
#define CHANEL_NUMBER 2
#define FRAME_PER_BUFFUER (PACKAT - PACK_HS - 8 )/16



std::vector<float> dataToPlay; 

int devRec ;
int devOut ;

PaStream* inputStream;
PaStream* OutputStream;

PaStreamParameters inputParm;
PaStreamParameters outputParm;

PaStreamParameters fakeIPar;

float* g_soundRecorded = nullptr;
std::atomic<bool> g_soundHaveBeenRecorde= false;
std::atomic<bool> g_soundThreadSholdStop = false;

std::thread g_ReacordingThread;



void soundThreadFunction(){
	while(!g_soundThreadSholdStop){
		if(g_soundRecorded == nullptr || !g_soundHaveBeenRecorde){continue;}
		readSoundFromMicrophoneHandler(g_soundRecorded,FRAME_PER_BUFFUER*CHANEL_NUMBER);
		g_soundHaveBeenRecorde=false;

	}
}


std::function<void(float* r , unsigned int lng)> readSoundFromMicrophoneHandler = [](auto,auto){return ;};


#define SEND_MAX_DES 0.00 //2


static int paCallBackReadFromMocrophone(const void* inBuf , void* outBuf , unsigned long framesPerBuf  ,
		const PaStreamCallbackTimeInfo* timeInfo , PaStreamCallbackFlags statusFlag , void* userData)
{
	float maxDes = 0.0;
	for(int i = 0 ; i < framesPerBuf*2 ; i++){
		if(((float*)inBuf)[i] >= maxDes){
			maxDes=((float*)inBuf)[i];
		}
	}
	
	if(maxDes >= SEND_MAX_DES){
		if(g_soundRecorded == nullptr){
			g_soundRecorded = (float*)malloc(FRAME_PER_BUFFUER*CHANEL_NUMBER*sizeof(float));
		}
		memcpy(g_soundRecorded, inBuf, FRAME_PER_BUFFUER*CHANEL_NUMBER*sizeof(float));
		g_soundHaveBeenRecorde=true;
	}
	return 0;
}

static int paCallBackOutSound(const void* inBuf , void* outBuf , unsigned long framesPerBuf  ,
		const PaStreamCallbackTimeInfo* timeInfo , PaStreamCallbackFlags statusFlag , void* userData)
{
	float* out = (float*)outBuf;
	for(int i = 0 ; i < framesPerBuf*2; i++){
		if(i < dataToPlay.size()){
			out[i]=dataToPlay[i];
		}else {
			out[i]=0.0;
		}
	}
	if(framesPerBuf*2 >= dataToPlay.size()){
		dataToPlay.clear();

	}else {
		dataToPlay.erase(dataToPlay.begin() , framesPerBuf*2+dataToPlay.begin());
	}
	return 0;
}


void PAErr(PaError err){
	if(err!=paNoError){
		logMsgs("PORT AUDIO ERORR",Pa_GetErrorText(err));
	}
	return;
}


void soundIOInit(){
	dataToPlay.reserve(FRAME_PER_BUFFUER);
	PAErr(Pa_Initialize());
	devRec = Pa_GetDefaultInputDevice();
	devOut = Pa_GetDefaultOutputDevice();
	memset(&inputParm , 0 , sizeof(inputParm));
	memset(&outputParm , 0 , sizeof(outputParm));
	MicrophoneInit();
	SpeakersInit();
	g_ReacordingThread = std::thread(soundThreadFunction);	

}



void soundIOStop(){
	if(devOut!=-1){
		PAErr(Pa_StopStream(OutputStream));
		PAErr(Pa_CloseStream(OutputStream));
	}
	if(devRec!=-1){
		PAErr(Pa_StopStream(inputStream));
		PAErr(Pa_CloseStream(inputStream));
	}
	g_soundThreadSholdStop=true;
	if(g_ReacordingThread.joinable()){
		g_ReacordingThread.join();
	}
	
	PAErr(Pa_Terminate());
	logMsgs("SOUND IO CLOSED");
}


void MicrophoneInit(){	
	if(devRec == -1){
		logMsgsErr("No defult microphone detected");
		return;
	}
	inputParm.channelCount = 2;
	inputParm.device = devRec;
	inputParm.hostApiSpecificStreamInfo=NULL;
	inputParm.sampleFormat = paFloat32;
	inputParm.suggestedLatency=Pa_GetDeviceInfo(devRec)->defaultLowInputLatency;
	
	outputParm.channelCount = 2;
	outputParm.device = devOut;
	outputParm.hostApiSpecificStreamInfo=NULL;
	outputParm.sampleFormat = paFloat32;
	outputParm.suggestedLatency=Pa_GetDeviceInfo(devRec)->defaultLowInputLatency;
		
	PAErr(Pa_OpenStream(&inputStream, &inputParm, &outputParm, SAMPEL_RATE, FRAME_PER_BUFFUER, paNoFlag,paCallBackReadFromMocrophone, NULL));

	PAErr(Pa_StartStream(inputStream));

}
void SpeakersInit(){
	if(devOut == -1){
		logMsgsErr("No defult sound output (aka. spekers) detected");
		return;
	}
	outputParm.channelCount = 2;
	outputParm.device = devOut;
	outputParm.hostApiSpecificStreamInfo=NULL;
	outputParm.sampleFormat = paFloat32;
	outputParm.suggestedLatency=Pa_GetDeviceInfo(devRec)->defaultLowInputLatency;
	PAErr(Pa_OpenStream(&OutputStream, nullptr, &outputParm, SAMPEL_RATE, FRAME_PER_BUFFUER, paNoFlag,paCallBackOutSound, NULL));

	PAErr(Pa_StartStream(OutputStream));
}
