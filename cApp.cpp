#include "cApp.h"
#include "login.hpp"
#include "networking.hpp"
#include "utils.hpp"
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <atomic>
#include <string>
#include <thread>
#include <unistd.h>
#include "soundIO.hpp"
#include "camera.hpp"
cApp::cApp(){

}

cApp::~cApp(){
	soundIOStop();
	CameraStop();
	networking_stop();
	logMsgs("CLOSING");
	return;
}
bool cApp::OnInit(){
		
	networking_init();	
	if(needLogin()){
		login = new cMainLogIn();
		login->Show();
	}else{
		
		updateLogs();
		soundIOInit();
		CameraStart();	
		frame1 = new cMain();
		frame1->Show();
		
	}
	return true;
}
