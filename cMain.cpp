#include <asio/ip/address_v4.hpp>
#include <wx/wx.h>
#include <cstring>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unistd.h>

#include <wx/filedlg.h>
#include <wx/gtk/filedlg.h>
#include <wx/string.h>
#include <wx/timer.h>
#include <wx/types.h>

#include <cstdarg>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
#include <wx/scrolwin.h>
#include <wx/vscroll.h>
#include <wx/gtk/statbmp.h>
#include <wx/image.h>

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/gtk/bitmap.h>
#include <wx/math.h>

#include <wx/chartype.h>
#include <wx/hashmap.h>

#include <wx/gtk/stattext.h>
#include <wx/valtext.h>
#include <wx/font.h>
#include <wx/gtk/font.h>
#include <wx/gtk/frame.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/event.h>

#include <wx/gdicmn.h>
#include <wx/gtk/textctrl.h>
#include <wx/gtk/listbox.h>
#include <wx/gtk/button.h>

#include <opencv2/opencv.hpp>

#include "cMain.h"
#include "login.hpp"
#include "soundIO.hpp"
#include "utils.hpp"
#include <cstdio>
#include "screenCapturing.hpp"
#include "cApp.h"
#include "camera.hpp"
#include "client.hpp"
wxBEGIN_EVENT_TABLE(cMain, wxFrame)
//	EVT_BUTTON(1001, cMain::OnButtonClicked)
wxEND_EVENT_TABLE()



#define START_DEV_BUT_ID 10000
wxBitmap Mat2Bitmap(cv::Mat& frame){
	cv::Mat normalRGB;
	cv::cvtColor(frame, normalRGB, cv::COLOR_BGR2RGB);
	wxImage image(normalRGB.cols , normalRGB.rows , normalRGB.data,true);
	image.Mirror();
	return wxBitmap(image);
}


cMain::cMain() : wxFrame(nullptr,wxID_ANY , "Chat.Locale" , wxDefaultPosition , wxSize(1200,800)){
	
	satingswindow = new Satings(satingsOpen);
	//Menus***********************************************************************
	wxFont headLineF(wxFontInfo(wxSize(0,36)).Bold());
	wxFont dicriptionF(wxFontInfo(wxSize(0,18)).Bold());
	
	Menupanel = new wxPanel(this, wxID_ANY,wxDefaultPosition , wxDefaultSize);
	
	rootSizer =new wxBoxSizer(wxVERTICAL);

	MenusSizer = new wxBoxSizer(wxHORIZONTAL);
	
	updateLogs();
	wxStaticText * userHi = new 
		wxStaticText(Menupanel,wxID_ANY,(wxString)(std::string)("Hi, "+ std::string(g_isTacher?"Professor ":"") + user_name) , wxPoint(20,20) , wxSize(200,-1)); 
	wxButton* satings = new wxButton(Menupanel,wxID_ANY ,  wxString::FromUTF8("\u2699") , wxDefaultPosition , wxSize(40,40) , wxBORDER_NONE);
	satings->SetFont(headLineF);
	satings->Bind(wxEVT_COMMAND_BUTTON_CLICKED,&cMain::OnSatingsButton,this);

	userHi->SetFont(headLineF);

	MenusSizer->AddSpacer(20);
	MenusSizer->Add(userHi , 1, wxEXPAND|wxALL);
	MenusSizer->Add(satings);
	Menupanel->SetSizerAndFit(MenusSizer);
	MenusSizer->Layout();
	
	rootSizer->Add(Menupanel , 0 ,wxEXPAND|wxALL);
	//Body***********************************************************************
	BodyS = new wxBoxSizer(wxHORIZONTAL);
	Body = new wxPanel(this);
	//screen Panel ///////////////////////////////////////////
	Intrcating = new wxPanel(Body);//dcreen vew + messaging
	IntractSizer = new wxBoxSizer(wxVERTICAL);
	
	wxPanel *screenVe = new wxPanel(Intrcating);
	wxInitAllImageHandlers();
	
		
	wxBitmap bitmap("empty.png" , wxBITMAP_TYPE_PNG);

	imageScreen = new wxStaticBitmap(screenVe ,wxID_ANY, bitmap , wxPoint(0,0) );
	imageScreen->SetParent(screenVe);
	
	IntractSizer->Add(screenVe , 2 , wxEXPAND|wxALL,10);
	



	wxPanel* mesaging = new wxPanel(Intrcating);
	wxSizer * mesageSzr = new wxBoxSizer(wxVERTICAL);
	
	wxPanel* mesages = new wxPanel(mesaging);	
	
	wxScrolledWindow* msgScroler = new wxScrolledWindow(mesages);
	wxSizer* msgScrollSizer =new wxBoxSizer(wxVERTICAL);
	
	msgScroler->SetScrollRate(5, 5);




	msgScroler->SetSizer(msgScrollSizer);
	
	
	mesageSzr->Add(mesages ,2, wxEXPAND|wxALL,5);
	
	wxPanel* sender = new wxPanel(mesaging);
	wxSizer* senderSizer= new wxBoxSizer(wxHORIZONTAL);
	msgTB = new wxTextCtrl(sender,wxID_ANY , "" , wxPoint(0,30),wxSize(300,-1) , wxTE_PROCESS_ENTER);


	msgTB->Connect(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&cMain::OnTextBoxPressEnter, NULL,this);
	senderSizer->Add(msgTB,1,wxEXPAND|wxLEFT|wxRIGHT);

	wxButton* fileSender =  new wxButton(sender,wxID_ANY ,  "+" , wxDefaultPosition , wxSize(40,40) );
	fileSender->SetFont(headLineF);
	fileSender->Bind(wxEVT_COMMAND_BUTTON_CLICKED,&cMain::OnSendFileButton,this);

	senderSizer->Add(fileSender,0,wxLEFT,20);
	sender->SetSizer(senderSizer);
	senderSizer->Layout();

	mesageSzr->Add(sender,0,wxEXPAND|wxLEFT|wxRIGHT);	
	mesaging->SetSizer(mesageSzr);
	mesageSzr->Layout();
	
	IntractSizer->Add(mesaging, 5 , wxEXPAND|wxALL,10);





	Intrcating->SetSizerAndFit(IntractSizer);
	IntractSizer->Layout();
	
	BodyS->Add(Intrcating,5,wxEXPAND);
	

	DevicesBuPanel = new wxPanel(Body);
 	DevSiz = new wxBoxSizer(wxVERTICAL);
	
	buttons = new cCanvas(DevicesBuPanel);
	butonsS = new wxBoxSizer(wxVERTICAL);

	buttons->SetSizer(butonsS);
	butonsS->Layout();
	
	buttons->FitInside();
	


	DevSiz->Add(buttons ,1,  wxALIGN_RIGHT|wxRIGHT , 10);
	
	DevicesBuPanel->SetSizer(DevSiz);

	DevSiz->Layout();

	BodyS->Add(DevicesBuPanel, wxRIGHT|wxEXPAND , 10);
	
	Body->SetSizerAndFit(BodyS);
	BodyS->Layout();
	imageHandlingReq = [&](Image& img,unsigned int id){
		
		if(id==CleintDevID){
			char* imageBitMapReseved = (char*)malloc(img.ImgHight*img.ImgWidht*3);
			memcpy(imageBitMapReseved, img.imgBitmap, img.ImgHight*img.ImgWidht*3);
			std::shared_ptr<wxImage> image = std::make_shared<wxImage>(img.ImgWidht,img.ImgHight,(uchar*)imageBitMapReseved , true);
			CallAfter([this,image,imageBitMapReseved](){
				imageScreen->SetBitmap(wxBitmap(*image));
				free(imageBitMapReseved);
			});
		}
		return ;		
		
	};
	rootSizer->Add(Body ,1, wxEXPAND);
		


	this->SetSizer(rootSizer);
	rootSizer->Layout();
	
	



	std::vector<wxStaticText*> v_mesagesStaticText;
	static unsigned int messaH = 0;
	readMsgHandlerRequast=[&](char* ms , unsigned int lng, unsigned int ID){
		std::string msg;
		//msg.reserve(lng);
		for(int i = 0 ; i < lng ; i++){
			msg.push_back(ms[i]);
		}
		if(ID==CleintDevID && getConPos(ID)!=-1){
			CallAfter([=](){
				std::string popout = cone[getConPos(ID)]->name;
				popout.append(" , send : \n");
				popout.append(msg);
				wxMessageBox(popout);
			});
		}

	};


	//Set up Capturing thread ////////////////////////////////////////////////////////////////////////
	
	cameraCapturingReq = [&](unsigned int w , unsigned int h , unsigned char* data){
		//logMsgs("SENDING IMAGE");
		if(CleintDevID!=-1 && getConPos(this->CleintDevID)!=-1 && !g_isTacher){
			cone[getConPos(CleintDevID)]->sendImage(h, w,data);	
		}else if(g_isTacher){
			for(int i = 0 ; i < cone.size() ; i++){
				if( i < cone.size() && cone[i]->getID() != -1){
					cone[i]->sendImage(h, w, data);
				}
				return ;
			}
			
		}

	};
	
	timer = new wxTimer(this);

	
	Bind(wxEVT_TIMER,[&](wxEvent &e){
		
		for( ; ConectionsNumber<cone.size() ; ConectionsNumber++){
				
				
				bt= new wxButton(buttons,START_DEV_BUT_ID+ConectionsNumber,(wxString)cone[ConectionsNumber]->name , wxDefaultPosition , wxDefaultSize);
				btns.push_back(bt);
				btns[ConectionsNumber]->Bind(wxEVT_COMMAND_BUTTON_CLICKED,&cMain::OnDevConButon,this);
				butonsS->Add(btns[ConectionsNumber],1,wxTOP,5);
				buttons->SetRowH(btns[ConectionsNumber]->m_height +5);
				buttons->SetRowCount(ConectionsNumber);
				//buttons->FitInside();	
				//butonsS->Layout();
				//rootSizer->Layout();
				this->Layout();
				this->Refresh();
				this->mesages.resize(ConectionsNumber+1);
				
		}
		for(int i = 0 ; i < ConectionsNumber && i < cone.size() ; i++ ){
			if(btns[i]->GetLabel() != cone[i]->name){
				btns[i]->SetLabel(cone[i]->name);
				btns[i]->Refresh();
			}
		}
		
		e.Skip();
		/*|/| in our app, the cone mutrix dosnt been freed (just if the cleint send CLOSE), |\|*/
		/*|\| even thogh that is in a side a terrabel memory mangment                       |/|*/
		/*|/| but this is hulpful , say the user get conected by some one                   |\|*/
		/*|\| out of the dicover rang , and disconect, it will be helpful                   |/|*/
		/*|/| that is save the conectionn, so we dont have to do a discovring again         |\|*/


	},timer->GetId());
	timer->Start(100);




	soundHandlerRequast = [&](float* data, unsigned int Size , unsigned int ID){
		//std::cout<<"Sond have been araved\n";
		if(ID==this->CleintDevID && !this->muteSpekers && getConPos(this->CleintDevID)!=-1 ){
			//dataToPlay.reserve(Size);
			for(unsigned int i = 0 ; i < Size; i++){
				dataToPlay.push_back(data[i]);
			}

		}
		return ;
	};
	
	readSoundFromMicrophoneHandler = [&](float* r , unsigned int bufferL){
		if(!g_isTacher && this->CleintDevID != -1 && this->allowSoundRecording && getConPos(this->CleintDevID)!=-1){
			cone[getConPos(this->CleintDevID)]->sendSound(r, bufferL);

		}else if(g_isTacher && allowSoundRecording){
			float* cpAu =(float*) malloc(bufferL*4);	
			memcpy(cpAu, r, bufferL*4);
			std::thread([=](){
				for(int i = 0 ; i < cone.size() ; i++){
					unsigned int id = cone[i]->getID();
					if(i < cone.size() && id==cone[i]->getID() && getConPos(id)!=-1){
						cone[i]->sendSound(cpAu, bufferL);
					}
				}
				
				free(cpAu);
			}).detach();
		}
		return ;
	};
	closeHandlerRequast = [&](unsigned int id){
		if(id==this->CleintDevID)
		{
			CleintDevID=-1;
		}
		if(btns.size()){
			
			
			if(this->ConectionsNumber){
				this->ConectionsNumber--;
			}

			for(int i = 0 ; i < btns.size()-1  && i < cone.size(); i++){
				btns[i]->SetLabel(cone[i]->name);//we update buttons, instade of erase , 
				btns[i]->Refresh();
			}
			butonsS->Show(btns[btns.size()-1],false);
			btns.pop_back();
			//butonsS->Detach(btns[btns.size()-1]);
			//butonsS->Hide(btns[btns.size()-1]);
			//btns[btns.size()-1]->Show(false);
			
			buttons->SetRowCount(ConectionsNumber);	
			this->Layout();
			this->Refresh();
			
		}
		
		return ;
	};
	
	fileHandlerRequastComplite = [&](std::string fn , unsigned int size , unsigned int ID){
		CallAfter([=](){
			if(getConPos(ID)==-1)return ;
			std::string msg = cone[getConPos(ID)]->name;
			msg .append(" send file : \n\"");
			msg.append(fn);
			msg.append( "\" - " + std::to_string(size) + " byte");
			wxMessageBox((wxString::FromUTF8(msg.c_str())) );

		});
		return ;
	};	
	
}


cMain::~cMain(){
	soundIOStop();
	CameraStop();
	if(timer){
		timer->Stop();
	}
	/*this->stopCapturing=true;
	this->StopFinding = true;
	this->allowSoundRecording=false;*/
	networking_stop();
	return;
}



void cMain:: OnButtonClicked(wxCommandEvent& evnt){
	//listbox->AppendString(textBox->GetValue());
	
	wxMessageBox("BUTTON CLICKED");
	evnt.Skip();
}




void cMain::OnDevConButon(wxCommandEvent&evt){
	
	CleintDevID = cone[evt.GetId() - START_DEV_BUT_ID]->getID();
	//cone[CleintDevID]->sendFile("../CMakeLists.txt");
	

}

void cMain::OnIpMan(wxCommandEvent& evt){
	/*std::string conect_to ;
	conect_to = (std::string)devTarg->GetValue().c_str();
	ip::tcp::endpoint endp(ip::make_address_v4(conect_to) , LISNT_PORT);
	unsigned int coneN =  addConection(endp,contextOUT);
	cone[coneN]->ping();
	
	evt.Skip();
	std::cout<<"RRRRRRRRRRRRRR\n\n" << conect_to<<"\n";
	*/
}




void cMain::OnTextBoxPressEnter(wxCommandEvent & evt){
	if(this->CleintDevID!=-1){
		cone[this->CleintDevID]->sendMSG((std::string)msgTB->GetValue());
		Msgs msg {this->CleintDevID,true,"",false,(std::string)msgTB->GetValue()};
		mesages[CleintDevID].push_back(msg);
		msgTB->Clear();
	}else {
		wxMessageBox("ERR: Sending to nowhere");
	}
	evt.Skip();

}




void cMain::OnSendFileButton(wxCommandEvent& evt){
	evt.Skip();
	if(CleintDevID == -1){
		wxMessageBox("ERR: Sending file to nowhere");
		return;
	}
	wxFileDialog fdg(this , "Send File" , "","",".* File (*.*)|*.*",wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if(fdg.ShowModal()==wxID_OK){
		cone[this->CleintDevID]->sendFile((std::string)fdg.GetPath().ToUTF8().data());
	}
	return;
}

void cMain::OnSatingsButton(wxCommandEvent& evt){
	if(!this->satingsOpen){
		satingswindow=new Satings(satingsOpen);
	}
	if(!satingswindow->IsShown()){
		satingswindow->Show();
		satingsOpen=true;
	}else{
		satingswindow->Hide();
		satingsOpen=false;
	}
	return;

}












/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////







cMainLogIn::cMainLogIn() : wxFrame(nullptr, wxID_ANY, "LOG IN" , wxDefaultPosition , wxSize(600,800)){
	wxFont headLineF(wxFontInfo(wxSize(0,36)).Bold());
	wxFont dicriptionF(wxFontInfo(wxSize(0,18)).Bold());

	//wxPanel* panel = new wxPanel(this);
	
	wxSizer* rootS = new wxBoxSizer(wxVERTICAL);

	wxStaticText* welcome = new wxStaticText(this , wxID_ANY , "Hello Ther!\nYou need to log in",wxPoint(0,22) , wxSize(-1,-1) , wxALIGN_CENTER_HORIZONTAL);
	
	welcome->SetFont(headLineF);
	
	rootS->Add(welcome ,0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM , 10);

	wxStaticText* discrip = new wxStaticText(this, wxID_ANY , "Enter the name that evry one in your network will see\na unicke name or your persenel name is rocomended." ,wxPoint(0,22) , wxSize(500,-1) , wxALIGN_CENTER_HORIZONTAL);
	rootS->Add(discrip,0,wxALIGN_CENTER_HORIZONTAL|wxALL , 10);
	
	
	textBox = new wxTextCtrl(this , wxID_ANY , "" , wxPoint(0,30),wxSize(300,-1) , wxALIGN_CENTER_HORIZONTAL);
	rootS->AddSpacer(50);
	rootS->AddStretchSpacer(1);
	rootS->Add(textBox ,0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM, 20);
	rootS->AddStretchSpacer(1);
	
	teacherBox = new wxCheckBox(this,wxID_ANY,"i am a teacher");
	rootS->Add(teacherBox,0,wxALL|wxALIGN_LEFT,10);

	btn1 = new wxButton(this, wxID_ANY , "Next");
	btn1->Bind(wxEVT_COMMAND_BUTTON_CLICKED,&cMainLogIn::OnButtonClicked,this);
	rootS->Add(btn1,0,wxALIGN_RIGHT|wxRIGHT|wxBOTTOM,20);
	
	this->SetSizerAndFit(rootS);


	rootS->Layout();
	
}
cMainLogIn::~cMainLogIn(){

}

void cMainLogIn::OnButtonClicked(wxCommandEvent& evnt){
	if(textBox->GetValue().size()){
		logIn((std::string)textBox->GetValue() , teacherBox->GetValue());
		updateLogs();
		evnt.Skip();
		workwindow = new cMain();
		this->Close();
		soundIOInit();	
		workwindow->Show();
	}else {
		wxMessageBox("Name is requared");
	}
	
	
}

wxBEGIN_EVENT_TABLE(cCanvas, wxVScrolledWindow)
wxEND_EVENT_TABLE()


cCanvas::cCanvas(wxWindow* parent):wxVScrolledWindow(parent,wxID_ANY)
{
	SetRowCount(10);
}

cCanvas::~cCanvas(){

}

void cCanvas::SetRowH(unsigned int r){
	rowH=r;
	RefreshAll();
	Refresh();
}
wxCoord cCanvas::OnGetRowHeight(size_t row)const
{
	wxASSERT( row < GetRowCount() );
	return wxCoord(rowH);	
}


//////////////////////////////////////////////////////////////////////////////////////////////


Satings::Satings(bool& isopen): wxFrame(nullptr, wxID_ANY, "Chat.Locale Satings" , wxDefaultPosition , wxSize(600,800))
{
	wxSizer* rootS = new wxBoxSizer(wxVERTICAL);
	wxFont satingsF(wxFontInfo(wxSize(0,18)).Bold());
	
	wxSizer* nameSs = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* nameText = new wxStaticText(this,wxID_ANY,"device name :");
	nameText->SetFont(satingsF);
	nameSs->Add(nameText , 0 , wxALL|wxALIGN_CENTER_VERTICAL,10);
	nameTb = new wxTextCtrl(this,wxID_ANY);
	nameTb->SetValue((wxString)user_name);
	nameSs->Add(nameTb,1,wxALL,10);
	rootS->Add(nameSs ,0 ,wxEXPAND); 
	

	wxSizer* dowS = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* downladText = new wxStaticText(this,wxID_ANY,"Download Foldare :");
	downladText->SetFont(satingsF);
	dowS->Add(downladText , 0 , wxALL|wxALIGN_CENTER_VERTICAL,10);
	downloadFTb = new wxTextCtrl(this,wxID_ANY);
	downloadFTb->SetValue((wxString)donwloadF); 
	dowS->Add(downloadFTb,1,wxALL|wxALIGN_CENTER_VERTICAL,10);
	rootS->Add(dowS,1,wxEXPAND);


	wxSizer* netSs = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* netSText = new wxStaticText(this,wxID_ANY,"network ip dicovring base :");
	netSText->SetFont(satingsF);
	netSs->Add(netSText , 0 , wxALL|wxALIGN_CENTER_VERTICAL,10);
	netdSTb = new wxTextCtrl(this,wxID_ANY);
	netdSTb->SetValue((wxString)networkDiscoverBase); 
	netSs->Add(netdSTb,1,wxALL|wxALIGN_CENTER_VERTICAL,10);
	rootS->Add(netSs,1,wxEXPAND);

	wxSizer* netES = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* netEText = new wxStaticText(this,wxID_ANY,"network ip discovring end  :");
	netEText->SetFont(satingsF);
	netES->Add(netEText , 0 , wxALL|wxALIGN_CENTER_VERTICAL,10);
	netdETb = new wxTextCtrl(this,wxID_ANY);
	netdETb->SetValue((wxString)networkDiscoverEnd);
	netES->Add(netdETb,1,wxALL|wxALIGN_CENTER_VERTICAL,10);
	rootS->Add(netES,1,wxEXPAND);
	

	teacher = new wxCheckBox(this,wxID_ANY,"i am a teacher");
	teacher->SetValue(g_isTacher);
	
	rootS->Add(teacher,1,wxALL,10);
	

	updateSatings=new wxButton(this,wxID_ANY,"Update Satings");
	updateSatings->Bind(wxEVT_COMMAND_BUTTON_CLICKED,&Satings::OnUpdateSatings , this);	
	rootS->Add(updateSatings,0,wxALL|wxALIGN_CENTER_HORIZONTAL,10);


	this->isOpen = &isopen;
	isopen = true;



	rootS->Layout();
	this->SetSizerAndFit(rootS);
	this->Layout();
	return;		
}

Satings::~Satings()
{
	*this->isOpen=false;
}

void Satings::OnUpdateSatings(wxCommandEvent & evt){
	
	setLogs((std::string)nameTb->GetValue(), (std::string)downloadFTb->GetValue(), (std::string)netdSTb->GetValue(), (std::string)netdETb->GetValue(), teacher->GetValue());
	evt.Skip();
}
