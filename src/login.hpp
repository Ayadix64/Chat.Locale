#ifndef LOGIN
#define LOGIN
#include <iostream>
#include <string>
#include "config.hpp"

#define USER_CONFIG "user.conf"

extern std::string user_name ;
extern std::string donwloadF ;

extern std::string networkDiscoverBase ;
extern std::string networkDiscoverEnd  ;

extern bool g_isTacher;

bool needLogin();

void updateLogs();
void logIn(std::string name,bool isATeacher);
void setLogs(std::string name , std::string donwloadFolder, std::string netdesStart , std::string netdesEnd , bool aTeacher);
#endif
