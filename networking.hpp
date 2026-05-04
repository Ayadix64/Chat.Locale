#ifndef LOCAL_PLAN_NETWORKING
#define LOCAL_PLAN_NETWORKING


#include "login.hpp"
#include <algorithm>
#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/address_v4.hpp>
#include <asio/ip/address_v6.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/post.hpp>
#include <asio/registered_buffer.hpp>
#include <asio/steady_timer.hpp>
#include <asio/strand.hpp>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

#include "utils.hpp"
using namespace asio;



#define LISNT_PORT 49152//40152


#define PING    0
#define PONG    1
#define MESSAGE 2
#define MSFILE  3
#define READY   4
#define IMAGE   5
#define SOUND   6


#define CLOSE 0xffff


#define MAGIC 0xBADDF00DB0B

#define PACKAT 0x10000


#define PACK_HS 16// Message hader size sizeof int*2+long = long+long = 8+8 = 16 , maths

void closeSocket(ip::tcp::socket& skt);


////////////////////////////////////////////////////////////////////////


struct Packat {
		unsigned int  TYPE  = PING       ;
		unsigned long Mgic  = MAGIC      ;
		unsigned int  msgL  = 0          ;
		char data [PACKAT-PACK_HS];
}__attribute__((packed)) ;

struct Message{
	unsigned int packN = 0;
	unsigned int msgl  = 0;
	char msg         [PACKAT-PACK_HS-8]      ;

};
struct FileMs {//it livs in Message::MES

		unsigned int fileNameL = 0       ;
		unsigned int partN     = 0       ;
		unsigned int dataSize  = 0       ;

		char data [PACKAT-PACK_HS-12] ;
}__attribute__((packed)) ;

struct ImageMs {//it livs in Message::MES
		unsigned int packN     = 0       ;
		unsigned int ImgWidht  = 0       ;
		unsigned int ImgHight  = 0       ;

		char data [PACKAT-PACK_HS-12]    ;
}__attribute__((packed)) ;


struct SoundMs {//it livs in Message::MES
		unsigned int packN     = 0         ;
		unsigned int Size      = 0         ;//this one is float , so size*sizeof(float) , iow : devide the size to 4

		float data [(PACKAT-PACK_HS-8)/4]  ;
}__attribute__((packed)) ;





struct Image {
	unsigned int ImgWidht  = 0       ;
	unsigned int ImgHight  = 0       ;
	char* imgBitmap                  ;

};






extern std::function<void(Image& img,unsigned int ID)> imageHandlingReq;
extern std::function<void(float* sound,unsigned int Size,unsigned int ID)> soundHandlerRequast;
extern std::function<void(char* msg , unsigned int msLng, unsigned int ID)> readMsgHandlerRequast;
extern std::function<void(unsigned int ID)> closeHandlerRequast;
extern std::function<void(std::string fileName,unsigned int size,unsigned int ID)> fileHandlerRequastComplite ;


void networking_init();
void networking_stop();


unsigned int addConection(ip::tcp::endpoint ep, io_context &io);



unsigned int addConection(ip::tcp::socket &skt, io_context &io );

void addServer(ip::tcp::socket &skt, io_context &io);
void ClientHandl(asio::ip::tcp::acceptor *accept , io_context* io );




void dicover(io_context &io , std::string Astart , std::string Aend );


unsigned int getConPos(unsigned int ID);





class DevInNetwork :public std::enable_shared_from_this<DevInNetwork>
{
	ip::tcp::socket *skt;
	ip::tcp::endpoint endp;
	io_context * io;
	Packat* ping;


	public:
	bool is_open = false;
	std::string name ;

	void FindHandler();

	DevInNetwork(asio::io_context & io , ip::tcp::endpoint& ep);
	~DevInNetwork();

	void connect();

};

#endif
