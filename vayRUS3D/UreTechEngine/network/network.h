//no license available
//ÜreTech Engine Network System header file

#ifndef network_h
#define network_h
#include<WinSock2.h>
#include <WS2tcpip.h>
#include<string>
#include<map>
#include <functional>

#include"../content/sourceContent/networkStructs/defaultNetworkStructs.h"

#include"../utils/Array.hpp"



namespace UreTechEngine {

	typedef char func_name[20];
	typedef char ent_name[30];

	class networkSystem {
	public:
		struct clientData {
			sockaddr_in clientAddr;
			int sock;
		};


		std::map<std::string, std::function<void(char*)>> rep_func_map;
		std::map<std::string, void*> rep_func_inputs;
		static networkSystem* c_NetInst;
		static networkSystem* getNetworkSystem();

		void* replicating_func_dat_o; 
		void* replicating_func_dat_i;
		size_t replicating_func_dat_size;

		bool invalidIP = true;

		void startServer();
		void connectToServer();
		void setToConnectIPAddr(std::string _ip,std::string _port);
		void disconnectToServer();
		void stopServer();
		void connectionRequest();
		void sendRecvToServer();
		void sendRecvToClient();
	private:
		size_t gamePageDataSize;
		char* gamePagePtr = nullptr;
		int sock,newsockfd;
		UreTechEngine::Array clientSocks;
	
		//int port=33660;
		int port = 80;
		sockaddr_in serv_addr, cli_addr;
		socklen_t clilen;
		networkSystem();
		~networkSystem();
	};
}
#endif // !network_h
