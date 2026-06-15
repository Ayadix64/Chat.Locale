#include "login.hpp"
#include <algorithm>
std::string user_name="";
std::string donwloadF="";

std::string networkDiscoverBase="192.168.1.0";
std::string networkDiscoverEnd="192.168.1.255";


bool g_isTacher = false;

bool needLogin(){
	bool need = therIsAfile(USER_CONFIG);
	std::string name;
	need &= readConf(USER_CONFIG, "name", name);
	return !need;
}

void updateLogs(){
	
	readConf(USER_CONFIG, "name", user_name);
	readConf(USER_CONFIG, "download", donwloadF);
	if(!readConf(USER_CONFIG, "netD_base", networkDiscoverBase)){
		networkDiscoverBase="192.168.1.0";
	}
	if(!readConf(USER_CONFIG, "netD_end", networkDiscoverEnd)){
		networkDiscoverEnd="192.168.1.254";
	}
	std::string tch ;
	if(readConf(USER_CONFIG, "teacher", tch) && tch == "yes"){
		g_isTacher = true;
	}


	return;	

}
void logIn(std::string name , bool teacher){
	//std::string name;	
	//std::cout << "\nHe , you have to log in : ";
	//std::cout << "\ninter a name : ";
	//std::cin>>name;

	writeConf(USER_CONFIG,"name" , name);
	writeConf(USER_CONFIG,"download" , "./");
	
	writeConf(USER_CONFIG,"netD_base","192.168.1.0");
	writeConf(USER_CONFIG,"netD_end","192.168.1.255");
	if(teacher){
		writeConf(USER_CONFIG,"teacher","yes");
	}	

	//std::cout << "\nthank you.";
	updateLogs();
	return;
}



void setLogs(std::string name , std::string donwloadFolder, std::string netdesStart , std::string netdesEnd , bool aTeacher)
{
	writeConf(USER_CONFIG,"name" , name);
	writeConf(USER_CONFIG,"download" , donwloadFolder);
	
	writeConf(USER_CONFIG,"netD_base",netdesStart);
	writeConf(USER_CONFIG,"netD_end",netdesEnd);
	if(aTeacher){
		writeConf(USER_CONFIG,"teacher","yes");
	}else {
		writeConf(USER_CONFIG,"teacher","no");

	}
	updateLogs();
	return;
}

