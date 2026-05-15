#ifndef CLEINT_HPP
#define CLEINT_HPP

#include "utils.hpp"
#include "networking.hpp"
using namespace asio;


class connection:
	public std::enable_shared_from_this<connection>

{
	std::vector <std::shared_ptr<connection>>* conectionBuf;
	
	asio::ip::tcp::endpoint endp;
	asio::ip::address adress;
	asio::io_context* io=nullptr;
	
	
	friend class server;	
	unsigned int serverOpnedFromeDestny = 0;

	
	unsigned long ID=0;
	unsigned long getUsebelID();//this .. do what is sounds it do
	unsigned int getVecPos(); 
	bool m_close = false;

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
	ip::tcp::endpoint getEndpoint();

	void sendMSG(std::string send_);
	bool is_open();

	unsigned int getID();
	
};


extern std::vector<std::shared_ptr<connection>> cone;

#endif
