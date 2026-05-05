#ifndef SERVER_HPP
#define SERVER_HPP

/////////////////////////////////////////////////////////////////////////
#include "utils.hpp"
#include "client.hpp"
using namespace asio;

class server
: public std::enable_shared_from_this<server>
{
public:
	std::shared_ptr<connection> conction;
	
	ip::tcp::socket*skt;
	Packat *data=nullptr;
	Packat *resevedData=nullptr;
	unsigned int dataPtr = 0;

	std::string Messag_;
	bool isOpen = false;//is socket open
	
	std::string fileName;
	std::ofstream file;//file pfstrime
	char* mlc =nullptr;//file buffer
	unsigned int ptr = 0;//file ptr
	unsigned int fileNS = 0;//file name size
	unsigned int fileS =0;	//file Size
	
	char* ImgB =nullptr;//image buffer
	unsigned int Iptr;//image buffer ptr
	unsigned int ISize;//image size
	
	unsigned int imageWidth=0;//as it sounds
	unsigned int imageHeight=0;

	float* SondB =nullptr;//sound buffer
	unsigned int Sptr;//sound buffer ptr
	unsigned int SSize;//sound size
	
	char* msgBuf=nullptr;
	unsigned int msPtr  ;
	unsigned int msSize ;

	io_context* io;	

	bool MsgIsIt(unsigned int type);
	
	void CloseHandler     ();
	void pingHandler      ();
	void MessageHandler   ();
	void FileHandler      ();
	void ImageHandler     ();
	void SondeHandler     ();
public:
	unsigned int n_flags = 0;
	void readHandler      ();
	
	void close            ();
	
public:
	server(ip::tcp::socket& skt__ , std::shared_ptr<connection>& con , io_context& io);
	
	~server();



};


extern std::atomic<unsigned long> server_count;

#endif
