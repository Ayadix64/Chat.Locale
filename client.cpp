#include "client.hpp"
#include "networking.hpp"
#include <lz4.h>
connection::connection (asio::ip::tcp::endpoint endp , io_context &io ,std::vector <std::shared_ptr<connection>>& coneBuf)
{
	this->io = &io;
	
	this->adress = endp.address();
	
	this->conectionBuf = &coneBuf;
	
	this->ID=getUsebelID();
	
	
	return;

}



	
connection::connection(ip::tcp::socket& skt , io_context &io , std::vector <std::shared_ptr<connection>>& coneBuf){
	this->io = &io;
	this->endp = skt.remote_endpoint();
	this->adress = skt.remote_endpoint().address();
	this->conectionBuf = &coneBuf;
	this->ID=getUsebelID();
}	
	


connection::~connection(){
	delete io;
}




void connection::sendFile(std::string fileP)
{
	if(m_close)return;
	std::thread([=](){
		std::filesystem::path fp(fileP);
		std::ifstream ifl(fileP ,std::ios::in|std::ios::app|std::ios::ate);
		const unsigned int fileS = ifl.tellg();
		//logMsgs("MSFILE SIZE", std::to_string(fileS));
		std::string fileName = fp.filename();
			
		char* mlc = (char*)malloc(fileS);
		memset(mlc, 0, fileS);

					
		if(!ifl.is_open()){
			logMsgs("NON FOUND FILE", fileName);
			ifl.close();
			FREE(mlc);
			return;
		}
		
		ifl.seekg(0);
		ifl.read(mlc, fileS);

		
		error_code ec;
		ip::tcp::socket sk(*io);
		sk.connect(ip::tcp::endpoint(this->adress, LISNT_PORT) , ec);
		if(ec || !sk.is_open()){
			logMsgs("ERROR SENDING FILE", ec.message());
			FREE(mlc);
			return ;
		}
		Packat ms;
		FileMs fms;
		
		memset(&ms , 0, sizeof(ms ));
		memset(&fms, 0, sizeof(fms));
			
		unsigned int packsNeed = (fileS + std::min((unsigned int)fileName.size(),(unsigned int)sizeof(fms.data)) + sizeof(fms.data) -1)/sizeof(fms.data);
		ms.msgL = fileS + packsNeed * (sizeof(ms) - sizeof(fms.data));

		ms.Mgic = MAGIC;
		ms.TYPE = MSFILE; 					//TODO:Remeber to change it to FILE type or somthing!
		
		fms.fileNameL=std::min(fileName.size(),sizeof(fms.data));
		fms.partN = 0;
		fms.dataSize =  fileS + fileName.size();
			
		if(fileName.size() > sizeof(fms.data)){
			logMsgs("WARNING", "file name is more than " + std::to_string(sizeof(fms.data)) + " charcture long , it will be cherenked");
		}

		unsigned int dataWroten = 0;
		memcpy(fms.data, fileName.c_str(), std::min((unsigned int)sizeof(fms.data) , (unsigned int)fileName.size()));
		
		if(fileName.size() < sizeof(fms.data)){
			unsigned int write = std::min((unsigned int)sizeof(fms.data) , (unsigned int)(fileS+fileName.size())) - fileName.size();
			memcpy((char*)(fms.data+fileName.size()), mlc, write);
			dataWroten+=write;
			//ifl.read((char*)(fms.data + fileName.size()), dataWroten);
			
		}
		Packat ready{.TYPE=READY,.Mgic=MAGIC,.msgL=0};


		memcpy(ms.data, &fms, std::min(sizeof(ms.data) , sizeof(fms)));
		bool ret = false;
		sk.write_some(buffer(&ms,PACKAT) , ec );
		if(ec){
			logMsgs("ERROR SENDING FILE", ec.message());
			closeSocket(sk);
			FREE(mlc);
			return ;
		}
		sk.wait(sk.wait_write);

		for(int i = 1; i < packsNeed ; i++){
			memset(fms.data, 0, sizeof(fms.data));
			memset(ms.data, 0, sizeof(ms.data));

			fms.partN=i;
			fms.fileNameL=0;
			
			unsigned int writ = std::min((unsigned int)(fileS - dataWroten) , (unsigned int)sizeof(fms.data));
				
			memcpy(fms.data, (char*)(mlc+dataWroten), writ);
			memcpy(ms.data, &fms, std::min(sizeof(ms.data) , sizeof(fms)));
			
			//logMsgs("WAITING", std::to_string(writ));
			sk.write_some(buffer(&ms,PACKAT));
			dataWroten+=writ;
			if(ec){
				logMsgs("ERROR SENDING FILE", ec.message());
				closeSocket(sk);
				FREE(mlc);
				return ;
			}

			sk.wait(sk.wait_write);
		}
		
		ifl.close();
		FREE(mlc);
		
		return ;
			
	}).detach();
}


void connection::sendImage(unsigned int hight , unsigned int width , unsigned char* iData){//this and the sound sender , already in ther owne thread, no need for creat a other
	bool cont = false;
	if(m_close)return;
		unsigned int hi = hight, wi= width;
		
		unsigned int Size = LZ4_compressBound(hi*wi*3);//R8G8B8


		char* imgData = (char*)malloc(Size);
		//memcpy(imgData, iData, Size);	
		
		Size = LZ4_compress_default((const char*)iData, (char*)imgData, (int)hi*wi*3, (int)Size);
		
		cont=true;
		error_code ec;
		ip::tcp::socket sk(*io);
		sk.connect(ip::tcp::endpoint(this->adress, LISNT_PORT) , ec);
		
		//sk.connect(ip::tcp::endpoint(this->adress, LISNT_PORT),ec);
		if(ec || !sk.is_open()){
			logMsgs("ERROR SENDING IMAGE", ec.message());
			if(imgData!=nullptr){
				FREE(imgData);
				imgData=nullptr;
			}
			closeSocket(sk);
			return ;
		}
		Packat ms;
		ImageMs ims;
		
		memset(&ms , 0, sizeof(ms ));
		memset(&ims, 0, sizeof(ims));
		


		unsigned int packsNeed = (Size + sizeof(ims.data) -1)/sizeof(ims.data);
		ms.msgL = Size + packsNeed * (sizeof(ms) - sizeof(ims.data));

		ms.Mgic = MAGIC;
		ms.TYPE = IMAGE;
		
		ims.ImgHight=hi;
		ims.ImgWidht=wi;
		ims.DataSize=Size;
		ims.packN = 0;	

		unsigned int dataWroten = 0;
		
		Packat ready{.TYPE=READY,.Mgic=MAGIC,.msgL=0};
		//if(imgData==nullptr)return;
		memcpy(ims.data, imgData, std::min(Size,(unsigned int)sizeof(ims.data)));
		dataWroten+=	std::min(Size,(unsigned int)sizeof(ims.data));
		memcpy(ms.data, &ims, std::min(sizeof(ms.data) , sizeof(ims)));
		
		bool ret = false;
		sk.write_some(buffer(&ms,PACKAT) , ec);
		if(ec){
			logMsgs("ERROR SENDING IMAGE", ec.message());
			closeSocket(sk);
			if(imgData!=nullptr){
				FREE(imgData);
				imgData=nullptr;
			}
			return ;
		}
		

		for(int i = 1; i < packsNeed  && imgData!=nullptr; i++){
			memset(ims.data, 0, sizeof(ims.data));
			memset(ms.data, 0, sizeof(ms.data));

			ims.packN=i;
			unsigned int writ = std::min((unsigned int)(Size - dataWroten) , (unsigned int)sizeof(ims.data));
				
			memcpy(ims.data, (char*)(imgData+dataWroten), writ);
			memcpy(ms.data, &ims, std::min(sizeof(ms.data) , sizeof(ims)));
			
			sk.write_some(buffer(&ms,PACKAT),ec);
			if(ec){
				logMsgs("ERROR SENDING IMAGE", ec.message());
				closeSocket(sk);
				if(imgData!=nullptr){
					FREE(imgData);
					imgData=nullptr;
				}
				return ;
			}
			sk.wait(sk.wait_write);
			dataWroten+=writ;
			
		}
		if(imgData!=nullptr){
			FREE(imgData);
			imgData=nullptr;
		}
		cont=true;
		return ;

}

void connection::sendSound(float* data__ , unsigned int ln){//NOTE:if somthing break , this is mybe the cuse
	bool conti = false;
		if(m_close)return;
		unsigned int Size = ln*4;
		float* data = (float*)malloc(Size);	
		memcpy(data, data__, Size);
		conti = true;	
			
		error_code ec;
		ip::tcp::socket sk(*io);
		
		sk.connect(ip::tcp::endpoint(this->adress, LISNT_PORT) , ec);
		if(ec || !sk.is_open()){
			logMsgs("ERROR SENDING SOUND", ec.message());
			closeSocket(sk);
			if(data!=nullptr){
				FREE(data);
				data=nullptr;
			}
			return ;
		}
		Packat ms;
		SoundMs sms; //some remember a thing rigth now
		
		memset(&ms , 0, sizeof(ms ));
		memset(&sms, 0, sizeof(sms));
			
		unsigned int packsNeed = (Size + sizeof(sms.data) -1)/sizeof(sms.data);
		ms.msgL = Size + packsNeed * (sizeof(ms) - sizeof(sms.data));

		ms.Mgic = MAGIC;
		ms.TYPE = SOUND;
		
		sms.Size = Size;
		sms.packN = 0;	

		unsigned int dataWroten = 0;
		
		Packat ready{.TYPE=READY,.Mgic=MAGIC,.msgL=0};
		if(data==nullptr)return;	
		memcpy(sms.data, data, std::min(Size,(unsigned int)sizeof(sms.data)));
		dataWroten+=	std::min(Size,(unsigned int)sizeof(sms.data));
		memcpy(ms.data, &sms, std::min(sizeof(ms.data) , sizeof(sms)));
		bool ret = false;
		sk.write_some(buffer(&ms,PACKAT) , ec);
		if(ec){
			logMsgs("ERROR SENDING SOUND", ec.message());
			closeSocket(sk);
			if(data!=nullptr){
				FREE(data);
				data=nullptr;
			}
			return ;
		}
		//sk.wait(sk.wait_write);

		for(int i = 1; i < packsNeed  && data!=nullptr; i++){
			memset(sms.data, 0, sizeof(sms.data));
			memset(ms.data, 0, sizeof(ms.data));

			sms.packN=i;
			unsigned int writ = std::min((unsigned int)(Size - dataWroten) , (unsigned int)sizeof(sms.data));
				
			memcpy(sms.data, (char*)(data+dataWroten), writ);
			memcpy(ms.data, &sms, std::min(sizeof(ms.data) , sizeof(sms)));
			
			ready.TYPE=0;
			
			sk.write_some(buffer(&ms,PACKAT),ec);
			
			dataWroten+=writ;
			if(ec){
				logMsgs("ERROR SENDING SOUND", ec.message());
				closeSocket(sk);
				if(data!=nullptr){
					FREE(data);
					data=nullptr;
				}
				return ;
			}
			sk.wait(sk.wait_write);
		
		}
		if(data!=nullptr){
			FREE(data);
			data=nullptr;
		}
		
		closeSocket(sk);
		return ;
}


void connection::sendClose(){
	ip::tcp::socket sk(*io);

	error_code ec;
	Packat msg ;
	msg.TYPE =CLOSE;
	msg.Mgic =MAGIC;
	msg.msgL = 0;
	bool ret = false;
	sk.connect(ip::tcp::endpoint(adress , LISNT_PORT) , ec);
	if(ec){
		logMsgsErr(ec.message());
		ret=true;
	}
	if(ret||ec){
		return ;;
	}
	sk.write_some(buffer(&msg , PACKAT) ,ec);
	if(ec){
		logMsgsErr(ec.message());
		ret=true;
	}
	sk.wait(sk.wait_write);
	closeSocket(sk);
	return;
}


void connection::Close(){
	unsigned int pos = getVecPos();
	if(pos!=-1){
		m_close=true;
		io->stop();
		conectionBuf->erase(conectionBuf->begin() + pos);
	}else{
		logMsgsErr("CONCTION OBJECT , nevr found his selfe :(");
	}
	io->stop();
	return;
}

void connection::ping(){
		ip::tcp::socket sk(*io);
	
		error_code ec;
		Packat msg ;
		msg.TYPE =PING;
		msg.Mgic =MAGIC;
		msg.msgL = 0;
		bool ret = false;
		sk.connect(ip::tcp::endpoint(adress , LISNT_PORT) , ec);
		if(ec){
			logMsgsErr(ec.message());
			ret=true;
		}
		if(ret||ec){
			return ;;
		}
		sk.write_some(buffer(&msg , PACKAT) ,ec);
		if(ec){
			logMsgsErr(ec.message());
			ret=true;
		}
		if(ret || ec){
			return ;;
		}
		sk.wait(sk.wait_read);

		sk.read_some(buffer(&msg,PACKAT),ec);
		if(ec){
			logMsgsErr(ec.message());
			ret=true;
		}
		if(ret || ec){
			return ;;
		}

		name.clear();
		for(int i = 0 ; i < msg.msgL ; i++){
			name.push_back(msg.data[i]);
		}
		closeSocket(sk);
}

ip::tcp::endpoint connection::getEndpoint(){
	return endp;
}

ip::address connection::getAddress(){
	return this->adress;
}


void connection::sendMSG(std::string send_){
	if(!send_.size())return;
	bool ret = false;
		Packat msg;

		Message ms ;
		
		std::string send = send_;
		
		error_code ec;
		ip::tcp::socket sk(*io);
		sk.connect(ip::tcp::endpoint(this->adress, LISNT_PORT), ec);
		if(ec){
			logMsgs("ERORR SENDING", ec.message());
			return ;
		}
		
		msg.TYPE=MESSAGE;
		msg.msgL=send.size();
		msg.Mgic=MAGIC;
		ms.msgl=send.size();
		unsigned int packN =(send.size()+sizeof(Message::msg)-1)/sizeof(Message::msg);
		
		for(int i = 0 ; i < packN  && sk.is_open(); i++ ){
			
			ms.packN = i;
			ms.msgl=send.size();
			unsigned int copyL = std::min(send.size() , (i+1)*sizeof(Message::msg));
			//memset(msg.data, 0, sizeof(msg.data));
			
			memcpy(ms.msg, (char*)(send.c_str() + i * sizeof(Message::msg)),  copyL - i*sizeof(Message::msg) );
			memcpy(msg.data, &ms,std::min(sizeof(msg.data),sizeof(ms.msg)));
			//for(int i =0 ; i < sizeof(ms->msg) ; i++)std::cout<<ms->msg[i];
			try{
			
				sk.write_some(buffer(&msg,PACKAT) ,ec);
				if(ec){
					logMsgs("ERORR SENDING", ec.message());
					closeSocket(sk);
					return ;
				}
				//sk.wait(sk.wait_write);
				
			}catch (system_error err){
				logMsgsErr(err.what());
			}
		}
		closeSocket(sk);
		return ;

	//});
}


void connection::sendPong(){
	Packat Pong;
	unsigned int sendS = std::min(sizeof(Packat::data) , user_name.size());
	Pong.msgL = sendS;
	Pong.Mgic=MAGIC;
	memcpy(Pong.data, user_name.c_str(), sendS);//it will be beter , if we send a warning her
	std::error_code ec;
	ip::tcp::socket sk(*io);
	std::error_code e;
	sk.connect(ip::tcp::endpoint(adress,LISNT_PORT), e);
	if(e){
		logMsgs("ERORR SENDING PONG", e.message());
		closeSocket(sk);
		return;
	}
	sk.write_some(buffer(&Pong,PACKAT),e);
	if(e){
		logMsgsErr(e.message());
		closeSocket(sk);
		return;
	}
	sk.wait(sk.wait_write);
	closeSocket(sk);
	return;
	
}
bool connection::is_open(){
	return true;//idk, this is a old one
}


unsigned int connection::getID(){
	return ID;
}
unsigned int connection::getUsebelID(){
	unsigned int nID=0;
	for(int i = 0 ; i < this->conectionBuf->size() ; i++){
		if(this->conectionBuf->at(i)->ID == nID)nID++;
	}
	return nID;
}

unsigned int connection::getVecPos(){
	unsigned int ret = -1;
	for(int i = 0 ; i < this->conectionBuf->size() ; i++){
		unsigned int fID = this->conectionBuf->at(i)->ID;
		if(fID==this->ID){
			ret=fID;
			break;
		}
	}
	return ret;

}

