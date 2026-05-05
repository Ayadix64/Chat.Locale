#include "login.hpp"
#include "utils.hpp"
#include "networking.hpp"
#include "server.hpp"
#include "client.hpp"
#include <algorithm>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/post.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
std::vector<std::shared_ptr<connection>> cone;
//std::vector<DevInNetwork> dess;

std::atomic<unsigned long> server_count;






DevInNetwork::DevInNetwork(asio::io_context & io , ip::tcp::endpoint& ep)
{
	ping = (Packat*)malloc(PACKAT);
	skt =new ip::tcp::socket(io);
	endp = ip::tcp::endpoint(ep.address() , LISNT_PORT);
	this->io = &io;
	connect();
}

DevInNetwork::~DevInNetwork(){
	FREE(ping);
	if(skt->is_open()){
		closeSocket(*skt);
	}
	delete skt;
}

void DevInNetwork::connect(){

	//asio::post(*io,[&](){
	skt->async_connect(endp,[&](auto ec){
		is_open=false;
		//name=false;

		ping->Mgic = MAGIC;
		ping->TYPE = PING;
		//logMsgs(endp.address().to_string(), "");
		if(ec){
			delete this;
		}
		else if(skt->is_open()){
			//IsTargitRuning(*skt);
			skt->async_write_some(buffer((char*)ping,PACKAT),[&](std::error_code ec , size_t lg){
				if(!ec){
					skt->async_read_some(buffer((char*)ping, PACKAT) , [&](std::error_code ecc, size_t llg){
						if(!ecc && ping->TYPE==PONG){
							
							is_open=true;
							name="";
							for(int i = 0 ; i < ping->msgL ; i++){
								name.push_back(ping->data[i]);
							}
							logMsgs("FIND", name);
							FindHandler();
							closeSocket(*skt);
							delete this;
						}

					});
				}
			});

		}

	});
}

void DevInNetwork::FindHandler(){
	unsigned int con = addConection(endp,*io);
	cone[con]->name = this->name;
}


unsigned int addConection(ip::tcp::endpoint ep, io_context &io){
	unsigned int ret = -1;
	for(int i = 0 ; i < cone.size() ; i++){
		if(cone[i]->getAddress() == ep.address()){
			ret=i;
			break;
		}
	}
	if(ret==-1){
		int ID=0;
		for(int i = 0 ; i < cone.size() ; i++){
			if(cone[i]->getID() == ID)ID++;
		}
		ret = cone.size();
		cone.emplace_back( std::make_shared<connection>(ep,io,cone));
	}
	return ret;
}



unsigned int addConection(ip::tcp::socket &skt, io_context &io){
	unsigned int ret = -1;
	for(int i = 0 ; i < cone.size() ; i++){
		if(cone[i]->getAddress() == skt.remote_endpoint().address()){
			ret=i;
			break;
		}
	}
	if(ret==-1){
		int ID=0;
		for(int i = 0 ; i < cone.size() ; i++){
			if(cone[i]->getID() == ID)ID++;
		}
		ret = cone.size();
		//std::shared_ptr<connection> conc = std::make_shared<connection>(skt,io,ID);
		//cone.push_back(conc);
		cone.emplace_back(std::make_shared<connection>(skt.remote_endpoint(),io,cone));
		//conc->ping();
		//conc->readHandler();

	}
	return ret;
}

void addServer(ip::tcp::socket &skt, io_context &io ){
	unsigned int c = addConection(skt , io);
	
	new server(skt , cone[c],io );
	
	return ;
}


void ClientHandl(asio::ip::tcp::acceptor *accept , io_context* io){
	accept->async_accept(
			[=](std::error_code ec , asio::ip::tcp::socket socket)
			{
				if(!ec){
					std::cout<<"Cleint connect! @" << server_count << "\n";
					addServer(socket,*io); 


				}
				ClientHandl(accept,io);
			}
	);

	return;
}



bool IsTargitRuning(ip::tcp::socket & skt){
	std::error_code ec;
	Packat* ping = (Packat*)malloc( PACKAT );
	ping->Mgic=MAGIC;
	ping->TYPE=PING;

	skt.write_some(buffer((char*)ping , sizeof(Packat) ),ec);
	std::cout << "\n---Wating for pong---\n";
	//skt.wait(skt.wait_read , 100);
	usleep(10000);
	std::cout << "\nDone";
	skt.read_some(buffer((char*)ping  , sizeof(Packat)) , ec);
	if(ping->TYPE != PONG || ec){
		FREE(ping);
		return false;
	}
	std::string targetName = ping->data;
	std::cout << "\n" << "Device Name : "  ;
	for(int i = 0 ; i < 20 ; i++)std::cout<<ping->data[i];;
	FREE(ping);
	return true;
}



void dicover(io_context &io , std::string Astart , std::string Aend ){

	try{
		ip::tcp::endpoint endp(ip::make_address_v4(Astart) , LISNT_PORT);

		unsigned int start = ip::make_address_v4(Astart).to_uint();
		unsigned int end = ip::make_address_v4(Aend).to_uint();
		if(end<start) {
			logMsgsErr("netdiscovering end is less than netdicovring start");
			return;
		}

		for(int i = start  ; i <  end ; i++){
			endp.address(ip::make_address_v4(i));
			//std::cout << "\n"<< endp.address() << "--" << endp.port();
			try{
				//std::shared_ptr<DevInNetwork> d = std::make_shared<DevInNetwork>(io,endp);
				//dess.push_back(d);
				//dess.emplace_back(std::make_shared<DevInNetwork>(io,endp));//never make the chorno angree,i main, if he see this code, i dont think that this will make any change

				new DevInNetwork(io,endp);
			}catch(std::system_error ec){
				//probaly main that we cant connect to
			}


		}
	}catch(std::error_code ec){
		logMsgsErr(ec.message());
	}
	return;
}





unsigned int getConPos(unsigned int ID){
	unsigned int ret = -1;
	for(int i = 0 ; i < cone.size() ; i++){
		if(cone[i]->getID() == ID){
			ret=i;
			break;
		}
	}
	return ret;
}


void closeSocket(ip::tcp::socket& skt){
	if(!skt.is_open())return;
	error_code ec;
	skt.shutdown(skt.shutdown_both , ec);
	if(ec){
		logMsgs("ERORR CLOSING SOCKET", ec.message() + ", closing any way...");
		
	}
	if(ec!=asio::error::not_connected){
		skt.close(ec);
	}
	
}





asio::io_context* serverContext = NULL;
asio::io_context* mainContext = NULL;

asio::ip::tcp::acceptor* acceptor = NULL;

std::thread serverThread;

std::atomic<unsigned int> threadsAreOn = 0;

void networking_init(){
	updateLogs();
	serverContext=new io_context();
	mainContext = new io_context();
	
	
	acceptor = new ip::tcp::acceptor(*serverContext,asio::ip::tcp::endpoint(asio::ip::tcp::v4() , LISNT_PORT));//accept conections
	
	const unsigned int threadsC = std::thread::hardware_concurrency();
	threadsAreOn=std::max(threadsC,(unsigned int)2);	
	

	serverThread = std::thread([&](){
		auto w1 = make_work_guard(*serverContext);
		logMsgs("SERVER THREAD STARTED");
		serverContext->run();
		threadsAreOn--;
		logMsgs("SERVER THREAD CLOSED");
		return ;
	});
	

	if(threadsC>1){
		for(unsigned int i = 0 ; i < threadsC-1 ; i++ ){
			std::thread([&](){
				logMsgs("MAIN_CONTEXT THREAD #"+std::to_string(i) + " --> OPEN");
				auto w2 = make_work_guard(*mainContext);
				mainContext->run();
				threadsAreOn--;
				logMsgs("MAIN_CONTEXT THREAD #"+std::to_string(i) + " --> CLOSE");
				return;
			}).detach();
		}
	}else {
		std::thread([&](){
			auto w2 = make_work_guard(*mainContext);
			mainContext->run();
			threadsAreOn--;
			return ;
		}).detach();
	}
	ClientHandl(acceptor, mainContext);
		
	dicover(*mainContext, networkDiscoverBase, networkDiscoverEnd);

	return;
}

void networking_stop(){
	if(acceptor->is_open()){
		acceptor->close();
	}
	for(int i = 0 ; i < cone.size() ; i++){
		cone[i]->sendClose();
	}
	cone.clear();	
	serverContext->stop();
	
	if(serverThread.joinable()){
		serverThread.join();
	}

	mainContext->stop();

	unsigned int start_waiting = clock()/(CLOCKS_PER_SEC*1000);
	const unsigned int timesup = 1000;
	while (threadsAreOn && (clock()/(CLOCKS_PER_SEC*1000) - start_waiting) < timesup) {}	

	delete serverContext;
	delete mainContext;
	delete acceptor;
	return;
}
