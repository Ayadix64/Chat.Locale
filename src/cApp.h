#ifndef CAPP
#define CAPP
#include <wx/wx.h>
#include "utils.hpp"
#include "networking.hpp"
#include "cMain.h"


class cApp : public wxApp
{
private:
	cMainLogIn* login = nullptr;
public:
	cMain* frame1 = nullptr;
	
	cApp();
	~cApp();
	virtual bool OnInit();
};
#endif
