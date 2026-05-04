#ifndef CMAIN
#define CMAIN
#include <wx/wx.h>
#include <memory_resource>

#include <vector>
#include <wx/gtk/checkbox.h>
#include <wx/gtk/frame.h>
#include <wx/types.h>
#include <wx/gtk/statbmp.h>
#include <wx/vscroll.h>

#include <thread>
#include <wx/gtk/bitmap.h>

#include <wx/event.h>
#include <wx/gtk/button.h>
#include <wx/gtk/listbox.h>
#include <wx/gtk/textctrl.h>

#include <opencv2/opencv.hpp>



struct Msgs{
	//unsigned int message = 0;
	unsigned int ID      = 0;
	bool isUserIsTheSender=false;
	std::string sendr;
	bool isItAfile=false;
	std::string mesage;//if it is a file , this will be the file name.

};//i run out of names




class cCanvas : public wxVScrolledWindow{
public:
	cCanvas(wxWindow* parent);
	~cCanvas();
	void SetRowH(unsigned int r);
private:
	unsigned int rowH = 20;
	virtual wxCoord OnGetRowHeight(size_t n)const;
//	virtual wxCoord OnGetColumnWidth(size_t col)const;

	wxDECLARE_EVENT_TABLE();

};



class Satings : public wxFrame
{
public:

	bool* isOpen ;
	wxTextCtrl* nameTb;
	wxTextCtrl* downloadFTb;
	wxTextCtrl* netdSTb;
	wxTextCtrl* netdETb;
	wxCheckBox* teacher;

	wxButton* updateSatings;

	Satings(bool& isopen);
	~Satings();
	void OnUpdateSatings(wxCommandEvent& evt);

};



class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

	Satings* satingswindow = nullptr;
	bool satingsOpen = false;

	std::vector<std::vector<Msgs>> mesages;
	
	wxStaticBitmap  *imageScreen ;

	int CapturingCamera = 0;
	bool stopCapturing = false;
	bool allowRecording = true;


	std::vector<wxButton*> btns;
	bool StopFinding = false;

	bool muteSpekers = false;
	bool allowSoundRecording = true;

	wxPanel * Menupanel     ;
	wxPanel * Body          ;
	wxPanel * Intrcating    ;
	wxPanel * screenVe      ;
	wxPanel * DevicesBuPanel;

	wxSizer * rootSizer     ;
	wxSizer * MenusSizer    ;
	wxSizer * BodyS         ;
	wxSizer * IntractSizer  ;
	wxSizer * DevSiz        ;
	wxSizer * butonsS       ;

	cCanvas * buttons       ;
	unsigned int CleintDevID=-1;
	wxTextCtrl* msgTB;


	wxButton* bt;
	unsigned int ConectionsNumber= 0;

	wxTextCtrl *devTarg;

	wxTimer* timer=nullptr;


	void OnDevConButon(wxCommandEvent& evt);
	void OnTextBoxPressEnter(wxCommandEvent& evt);
	void OnSendFileButton(wxCommandEvent& evt);
	void OnSatingsButton(wxCommandEvent& evt);
	void OnIpMan(wxCommandEvent& evt);
	wxDECLARE_EVENT_TABLE();

	void OnButtonClicked(wxCommandEvent& evnt);
};


class cMainLogIn : public wxFrame
{
public:
	cMainLogIn();
	~cMainLogIn();
	cMain* workwindow;
	wxCheckBox * teacherBox;
	//wxButton* btn1 = nullptr;
	//wxButton* btn2 = nullptr;
	wxTextCtrl *textBox = nullptr;
	//wxListBox* listbox  = nullptr;
	wxButton * btn1 = nullptr;
	wxButton* btn2 = nullptr;
		//wxDECLARE_EVENT_TABLE();

	void OnButtonClicked(wxCommandEvent& evnt);
};




#endif
