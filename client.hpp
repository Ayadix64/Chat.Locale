#ifndef CLEINT_HPP
#define CLEINT_HPP

#include "utils.hpp"
#include "networking.hpp"
using namespace asio;


class connection:
	public std::enable_shared_from_this<connection>

{
	std::vector <std::shared_ptr<connection>>* conectionBuf;
	
	asio::ip::address adress;
	asio::io_context* io=nullptr;
	
	
	friend class server;	
	unsigned int serverOpnedFromeDestny = 0;

	
	unsigned long ID=0;
	unsigned long getUsebelID();//this .. do what is sounds it do
	unsigned int getVecPos(); 
	std::atomic<bool> m_close = false;

	std::atomic<unsigned int> m_operationOpend=0;

public:
	std::string name;
	ip::address getAddress();
	
	
	connection(asio::ip::tcp::endpoint endp , io_context &io,std::vector <std::shared_ptr<connection>>& coneBuf);
	
	connection(ip::tcp::socket& skt , io_context &io,std::vector <std::shared_ptr<connection>>& coneBuf);
		
	~connection();	
		

	void sendFile(std::string fileP);
	void sendImage(unsigned int hight , unsigned int width , unsigned char* imgData);
	void sendSound(float* snd, unsigned int ln);

	void ping();
	void sendPong();	
	void sendClose();
	void Close();

	void sendMSG(std::string send_);
	bool is_open();

	unsigned int getID();
	
};



connection*  Conection(unsigned long ID);
unsigned int getConPos(unsigned long ID);

unsigned int addConection(ip::tcp::socket &skt, io_context &io);
unsigned int addConection(ip::tcp::endpoint ep, io_context &io);

unsigned int  ConctionsMatrixSize();
unsigned long GetConectionMatrixID(unsigned int pos);
void 	      GetConectionName(std::string& name, unsigned int pos);
connection*   GetConectionIn(unsigned int pos);

#endif
