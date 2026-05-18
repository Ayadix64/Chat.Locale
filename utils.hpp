#ifndef UTILS
#define UTILS
#include <iostream>
#include <string>
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#define WINDOW_WIDTH_CONFIG  "window_w"
#define WINDOW_HEIGTH_CONFIG "window_h"
#define SOUND_ALLOW_CONFIG  "AllowSendSound"
#define VIDEO_ALLOW_CONFIG  "AllowSendVideo"



#define FREE(x)\
	if(x==NULL){\
		std::cout<<"FREE: "<<#x<<" IT IS A BAD ONE ! (=NULL)\n";\
	}else {\
		free(x);\
		x=NULL;\
	}


static void logMsgs(std::string type , std::string msg=""){
	std::cout<<"[" << type << "] " << msg <<".\n";
	return;
}
static void logMsgsErr(std::string msg){
	logMsgs("ERORR", msg);
	return;
}

static void printptr(void* ptr , size_t size){
	for(int i = 0 ; i  < size ; i++){
		std::cout<<((char*)ptr)[i];
	}
	return;
}

static void ConsoleCl(unsigned int c){
	std::cout<<"\x1B["+std::to_string(c)+"m";
	return;
}
static void resetConsolCl(){
	std::cout<<"\x1B[0m\n";
	return;
}

typedef  struct Vec2ui{unsigned int x=0, y=0;} Vec2ui;



#endif
