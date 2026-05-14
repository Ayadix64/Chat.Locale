#ifndef  CONFIG
#define  CONFIG 
#include <algorithm>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

int readConf(std::string file , std::string conf , std::string& readTo);

bool therIsAfile(std::string file);

bool IsTherConfig(std::string file, std::string conf);
int writeConf(std::string file , std::string conf , std::string valu);

#endif
