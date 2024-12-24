#include "server.hpp"
#include "client.hpp"



int main(int argc, char *argv[])
{
	if(argc != 3){
		std::cerr<<RED<<"Usage: ./server <port> <Password>"<<WHI<<std::endl;
		_exit(1);
	}
	Server ser;
	std::cout<<YEL<<"-------------> 			SERVER			<-------------"<<WHI<<std::endl;
	try{
		signal(SIGINT, Server::SignalHandler); //-> catch the signal (ctrl + c)
		signal(SIGQUIT, Server::SignalHandler); //-> catch the signal (ctrl + \)
		int port = atoi(argv[1]);
		ser.ServerInit(port, argv[2]); //-> start the server
	}
	catch(const std::exception& e){
		ser.closeFDS(); //-> close the file descriptors
		std::cerr << e.what() << std::endl;
	}
	 std::cout<<BLUE<<"------------->        Server  Closed        <------------- "<<BLUE<<std::endl;
}

//run as client   localhost : 127.0.0.1  port : 4444 
