#include "server.hpp"
#include "login.hpp"
#include "utils.hpp"
#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/impl/config.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/post.hpp>
#include <asio/registered_buffer.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <ios>
#include <iostream>
#include <string>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <lz4.h>


std::function<void(Image &img,unsigned int ID)> imageHandlingReq = [](auto,auto){return;};
std::function<void(float* sound,unsigned int Size,unsigned int ID)> soundHandlerRequast = [](auto,auto,auto){return;};;
std::function<void(char* msg , unsigned int msLng, unsigned int ID)> readMsgHandlerRequast = [](char* ms , unsigned int l, auto ){
	ConsoleCl(30);
	for(int i = 0 ; i < l ; i++){
		std::cout<<ms[i];
	}
	resetConsolCl();
};
std::function<void(unsigned int ID)> closeHandlerRequast = [](unsigned int ID){
	for(int i  =  0 ; i < cone.size() ; i++){
		if(cone[i]->getID() == ID){
			logMsgs("CLOSING CONCETION", " @ " + cone[i]->getEndpoint().address().to_string() + " aka."+cone[i]->name);
			break;
		}
	}
	return ;
};

std::function<int(std::string fileName,unsigned int size,unsigned int ID)> fileHandlerRequast = 
[](auto fn,auto s,auto ID){
	logMsgs(cone[getConPos(ID)]->name+" SENDE A FILE", fn+" @"+std::to_string(s)+"byte");
	return 1;
};


std::function<void(std::string fileName,unsigned int size,unsigned int ID)> fileHandlerRequastComplite =
[](auto fn,auto s,auto ID){
	logMsgs(cone[getConPos(ID)]->name+" SENDE A FILE", fn+" @"+std::to_string(s)+"byte");
	std::cout<<"\n";
	for(int ii = 0 ; ii < fn.size()+2 ; ii++){
		std::cout<<"*";
	}
	std::cout<<"\n*"<<fn<<"*" << " @" << s<<"\n";
	for(int ii = 0 ; ii < fn.size()+2 ; ii++){
		std::cout<<"*";
	}
	std::cout<<"\n";
	return 1;
};







bool server::MsgIsIt(unsigned int a){
	return data->TYPE == a & data->Mgic ==MAGIC;
}

void server::pingHandler(){
	error_code ec;	
	Packat msg ;

	msg.TYPE =PONG;
	msg.Mgic =MAGIC;
	msg.msgL = user_name.size();
	memset(msg.data, 0, sizeof(msg.data));
	memcpy(msg.data, user_name.c_str(), (size_t)std::min((int)user_name.size() , PACKAT));
	try{
			
		if(skt->is_open() && !ec){
			skt->write_some(buffer(&msg , PACKAT/*user_name.size() +PACK_HS*/),ec);
			if(!ec){
				skt->wait(skt->wait_write);

			}
		}
		else if(ec){
			logMsgs("ERORR IN PING HANDLING", ec.message());
		}else{
			logMsgs("ERORR IN PING", "socket  not open during the writing");
		}
		
	}catch (std::system_error syser){
		logMsgs("ERORR IN PING", syser.what());
	};
	return;
}
void server::FileHandler(){
	FileMs * msfile = (FileMs*)data->data;
	if(!msfile->partN){

		fileName.erase();
		for(unsigned int i = 0 ; i < msfile->fileNameL ; i++){
			fileName.push_back(msfile->data[i]);
			
		}
		
		fileNS = fileName.size();
		fileS = msfile->dataSize;
		/**/

		fileName.insert(0,donwloadF+"/");
		mlc = (char*)malloc(msfile->dataSize);
		file.open(fileName );
		
		//file.write((char*)(msfile->data+msfile->fileNameL), std::min((unsigned int)(sizeof(FileMs::data)-msfile->fileNameL) , msfile->dataSize-msfile->fileNameL));
		
		ptr=0;
		for(int i = msfile->fileNameL ; i < std::min((unsigned int)sizeof(FileMs::data) , fileS);i++,ptr++){
			mlc[ptr] = msfile->data[i];
		}

		//file.close();
	}
	
	else {
		//file.open(fileName , std::ios::app );
		//file.write(msfile->data, std::min(msfile->dataSize , (unsigned int)((msfile->partN+1)*sizeof(FileMs::data) - msfile->partN*sizeof(FileMs::data))) );
		
		for(int i = 0 ; 
			i <  std::min(fileS  , (unsigned int)((msfile->partN+1)*sizeof(FileMs::data))) -msfile->partN*sizeof(FileMs::data);
			i++,ptr++){
			mlc[ptr] = msfile->data[i];
		}
		//logMsgs(std::to_string(ptr), "");
		//file.close();
									
		
		
	}
	if(ptr >= msfile->dataSize - fileNS ){
		//logMsgs("WRITING FILE ...", "");
		file.seekp(0);
		file.write(mlc, ptr);
		fileHandlerRequastComplite(fileName,fileS,this->conction->getID());		
		
		fileName="";
		if(mlc!=nullptr){
			FREE(mlc);
			mlc=nullptr;
		}
		ptr=0;
		fileNS=0;
		fileS=0;
		file.close();
		return;
	}

	return;


}



void server::ImageHandler(){
	ImageMs * msImg = (ImageMs*)data->data;
	if(!msImg->packN){
		ISize = msImg->DataSize;
		ImgB = (char*) malloc(ISize);
		imageWidth=msImg->ImgWidht;
		imageHeight=msImg->ImgHight;
		Iptr=0;
		for(int i = 0 ; i < std::min((unsigned int)sizeof(ImageMs::data) , ISize);i++,Iptr++){
			ImgB[Iptr] = msImg->data[Iptr];
		}
	}
	
	else {
		
		for(int i = 0 ; 
			i <  std::min( ISize  , (unsigned int)( (msImg->packN+1)*sizeof(ImageMs::data) ) ) - msImg->packN*sizeof(ImageMs::data);
			i++,Iptr++){
			ImgB[Iptr] = msImg->data[i];
		}
		
	}
	if(Iptr >= ISize){
	
		Image img;
		img.ImgHight = this->imageHeight;
		img.ImgWidht = this->imageWidth;
		img.imgBitmap = (char*)malloc(this->imageWidth*this->imageHeight*3);
		LZ4_decompress_safe(ImgB, img.imgBitmap, ISize, this->imageHeight*this->imageWidth*3);	
		imageHandlingReq(img,conction->ID);
		FREE(img.imgBitmap)
		
		if(ImgB!=nullptr){
			FREE(ImgB);
			ImgB=nullptr;
		}
		ISize=0;
		imageWidth=0;
		imageHeight=0;
		Iptr=0;
		return;

	}
	return;
}
void server::SondeHandler(){
	SoundMs * msSound = (SoundMs*)data->data;
	if(!msSound->packN){
		SSize = msSound->Size / 4;
		SondB = (float*) malloc(SSize*4);
		Sptr=0;
		for(int i = 0 ; i < std::min((unsigned int)sizeof(SoundMs::data)/4 , SSize);i++,Sptr++){
			SondB[Sptr] = msSound->data[Sptr];
		}
	}
	
	else {
		
		for(int i = 0 ; 
			i <  std::min( SSize, (unsigned int)( (msSound->packN+1)*sizeof(SoundMs::data)/4 ) ) - msSound->packN*sizeof(SoundMs::data)/4;
			i++,Sptr++){
			SondB[Sptr] = msSound->data[i];
		}
		
	}
	if(Sptr >= SSize){
	
		soundHandlerRequast(SondB,SSize,conction->ID);
		if(SondB!=nullptr){
			FREE(SondB);
			SondB=nullptr;
		}
		SSize=0;
		Sptr=0;

	}

	return;
}

void server::MessageHandler(){
	
	Message * msg = (Message*)data->data;
	if(!msg->packN){
		msSize = msg->msgl;
		msgBuf = (char*) malloc(msSize);
		msPtr=0;
		for(int i = 0 ; i < std::min((unsigned int)sizeof(Message::msg) , msSize);i++,msPtr++){
			msgBuf[msPtr] = msg->msg[msPtr];
		}
	}
	
	else {
		
		for(int i = 0 ; 
			i <  std::min( msSize, (unsigned int)( (msg->packN+1)*sizeof(Message::msg) ) ) - msg->packN*sizeof(Message::msg);
			i++,msPtr++){
			msgBuf[msPtr] = msg->msg[i];
		}
		
	}
	if(msPtr >= msSize){
		readMsgHandlerRequast(msgBuf,msSize,this->conction->ID);		

		if(msgBuf!=nullptr){
			FREE(msgBuf);
			msgBuf=nullptr;
		}
		msSize=0;
		msPtr=0;

	}
	return;

}



void server::close(){
	if(isOpen){
		isOpen=false;
		delete skt;

		if(SondB!=nullptr){
		FREE(SondB);
		SondB=nullptr;
		}
		if(mlc!=nullptr){
		FREE(mlc);
		mlc=nullptr;
		}
		if(ImgB!=nullptr){
		FREE(ImgB);
		ImgB=nullptr;
		}
		if(Messag_.size())Messag_.erase();
		if(this->data!=nullptr){
		FREE(this->data);
		this->data=nullptr;
		}
		if(this->resevedData!=nullptr){
		FREE(this->resevedData);
		this->resevedData=nullptr;
		}
		this->conction->serverOpnedFromeDestny--;
	}

	return;
}

void server::CloseHandler(){
	closeHandlerRequast(conction->ID);
	
	this->conction->Close();//kill your master
	
	this->close();//and then , kill your selfe	
	delete this;
	return;	
}


void server::readHandler(){
	try{
		
		asio::async_read(*skt,buffer(data,PACKAT) , [=](error_code ec, size_t leng){
			if(!ec){
				
				if(skt->is_open()){
					if(MsgIsIt(PING)) { // is it ping? if is it send pong
						logMsgs("PING");
						pingHandler();
					}else if(MsgIsIt(PONG)){
						logMsgs("PONG");
						conction->name.clear();
						for(int i =0 ; i < data->msgL ; i++ ){
							conction->name.push_back(data->data[i]);
						}
					}

					else if (MsgIsIt(MESSAGE)){
						MessageHandler();
					}
					else if(MsgIsIt(MSFILE)){
						FileHandler();
					}else if (MsgIsIt(IMAGE)) {
						//logMsgs("GET A IMAGE");
						ImageHandler();
					}else if (MsgIsIt(SOUND)) {
						SondeHandler();
					}else if(MsgIsIt(CLOSE)){
						//logMsgs("Dead for your father", "");
						CloseHandler();//kill your famely
						return ;
					}					
				}
				
			}else{
				if(ec.value() == asio::error::eof){
					//logMsgs("END-DELETING", "Entry #"+std::to_string(entry));
					
					closeSocket(*skt);
					
					
				}else if(ec.value() == asio::error::operation_aborted){
					logMsgs("SERVER FATEL ERROR --OA--", ec.message());
				}
				else {
					logMsgs("SERVER FATEL ERROR", ec.message());
					closeSocket(*skt);	
				}
				if(isOpen){	
					delete skt;
					isOpen=false;
					if(SondB!=nullptr){
						FREE(SondB);
						SondB=nullptr;
					}
					if(mlc!=nullptr){
						FREE(mlc);
						mlc=nullptr;
					}
					if(ImgB!=nullptr){
						FREE(ImgB);
						ImgB=nullptr;
					}
					if(Messag_.size())Messag_.erase();
					if(this->data!=nullptr){
						FREE(this->data);
						this->data=nullptr;
					}
					if(this->resevedData!=nullptr){
						FREE(this->resevedData);
						this->resevedData=nullptr;
					}
					this->conction->serverOpnedFromeDestny--;
					
					delete this;
					//io->stop();
					//delete this;
				}
				return ;
			}
			dataPtr=0;
			readHandler();

		});
	}catch (std::system_error ec){
		logMsgs("SYSTEM ERROR", ec.what());
	};
}



server::server(ip::tcp::socket &skt , std::shared_ptr<connection>& con , io_context& io ){

	this->conction = con;
	isOpen=true;
	data=(Packat*)malloc(PACKAT);
	server_count++;	
	this->conction->serverOpnedFromeDestny++;
	
	unsigned int fd = skt.release();
	
	this->skt=new ip::tcp::socket(*this->conction->io);
	this->skt->assign(ip::tcp::v4() , fd);
	readHandler();
}

server::~server(){
	if(isOpen){
		
		if(SondB!=nullptr){
			FREE(SondB);
			SondB=nullptr;
		}
		if(mlc!=nullptr){
			FREE(mlc);
			mlc=nullptr;
		}
		if(ImgB!=nullptr){
			FREE(ImgB);
			ImgB=nullptr;
		}
		if(Messag_.size())Messag_.erase();
		if(this->data!=nullptr){
			FREE(this->data);
			this->data=nullptr;
		}
		if(this->resevedData!=nullptr){
			FREE(this->resevedData);
			this->resevedData=nullptr;
		}
		closeSocket(*skt);
		dataPtr=0;
		Messag_.erase();
		this->conction->serverOpnedFromeDestny--;
		//FREE(skt);
		delete skt;
		isOpen=false;
		//io->stop();
	}
	if(server_count){
		server_count--;
	}else {
		logMsgs("UN-EXPECTED ERORR","server count = 0 and servers are not");
	}
	return;
}
