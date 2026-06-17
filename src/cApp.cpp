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
	logMsgs("CLOSING");
	return;
}
bool cApp::OnInit(){
	if(needLogin()){
		login = new cMainLogIn();
		login->Show();
	}else{
		frame1 = new cMain();
		frame1->Show();
		
	}
	return true;
}
