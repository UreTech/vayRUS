#include "network.h"
#include<WinSock2.h>
#include"../UreTechEngine/utils/errOut.h"
#include"../EngineBase.h"
#include<string>

namespace UreTechEngine {
    class EngineERROR;
}

UreTechEngine::networkSystem* UreTechEngine::networkSystem::c_NetInst = nullptr;

UreTechEngine::networkSystem* UreTechEngine::networkSystem::getNetworkSystem()
{
    if (c_NetInst == nullptr) {
        c_NetInst = new networkSystem();
        return c_NetInst;
    }
    else {
        return c_NetInst;
    }
    return nullptr;
}

void UreTechEngine::networkSystem::startServer()
{
    WSADATA wsaData;
    int result;
    // Winsock baþlatma
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        EngineERROR::consoleError(std::string("(Network Socket): Failed to WSAStartup! ERROR CODE:") + std::to_string(result), EngineERROR::ERROR_FATAL);

    }

    sock=socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        EngineERROR::consoleError(std::string("(Network Socket): Failed to open socket! ERROR CODE:")+ std::to_string(sock), EngineERROR::ERROR_FATAL);
    }

    // Non-blocking mod ayarlanýyor(GPT kodu bu) // düzeltilmeli bloklamýyo ama accept edemiyor
    u_long mode = 1;
    if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR) {
        EngineERROR::consoleError(std::string("(Network Socket): Non blocking socket error!") + std::to_string(sock), EngineERROR::ERROR_FATAL);
    }

    EngineERROR::consoleError("Socket: Server Socket Started!", EngineERROR::INFO_NORMAL);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // automatically be filled with current host's IP address
    serv_addr.sin_port = htons(port);
    int bind_Err;
    if (bind_Err = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        EngineERROR::consoleError(std::string("(Network Socket): Failed to bind! ERROR CODE:") + std::to_string(bind_Err), EngineERROR::ERROR_FATAL);
    }

    if (listen(sock, 5)==SOCKET_ERROR) {
        EngineERROR::consoleError(std::string("(Network Socket): Failed to listen!"), EngineERROR::ERROR_FATAL);
    }
    clilen = sizeof(cli_addr);  
    UreTechEngineClass::getEngine()->isServer = true;
    UreTechEngineClass::getEngine()->isInServer = true;
}

void UreTechEngine::networkSystem::connectToServer()
{
    WSADATA wsaData;
    int result;
    // Winsock baþlatma
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        EngineERROR::consoleError(std::string("(Network Socket): Failed to WSAStartup! ERROR CODE:") + std::to_string(result), EngineERROR::ERROR_FATAL);

    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        EngineERROR::consoleError(std::string("(Network Socket): Failed to open socket! ERROR CODE:") + std::to_string(sock), EngineERROR::ERROR_FATAL);
    }

    EngineERROR::consoleError("Socket: Client Socket Started!", EngineERROR::INFO_NORMAL);

    UreTechEngineClass::getEngine()->isServer = false;
    UreTechEngineClass::getEngine()->isInServer = true;

    if (invalidIP) {
        EngineERROR::consoleError("(Network Socket): Server is not available!",EngineERROR::ERROR_NORMAL);
    }
    else {
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            EngineERROR::consoleError(std::string("(Network Socket): Can not connect to server!"), EngineERROR::ERROR_NORMAL);
        }
    }
}

void UreTechEngine::networkSystem::setToConnectIPAddr(std::string _ip, std::string _port)
{
    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, _ip.c_str(), &serv_addr.sin_addr) <= 0) { 
        EngineERROR::consoleError(std::string("(Network Socket): Invalid ip addres!"), EngineERROR::ERROR_NORMAL);
        invalidIP = true;
    }
    else {
        invalidIP = false;
    }
    int portNo;
    sscanf(_port.c_str(), "%i", &portNo);
    serv_addr.sin_port = htons(portNo);
}

void UreTechEngine::networkSystem::disconnectToServer()
{
    closesocket(sock);
}

void UreTechEngine::networkSystem::stopServer()
{
    closesocket(newsockfd);
    closesocket(sock);
}

void UreTechEngine::networkSystem::connectionRequest()
{
    newsockfd = accept(sock, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd == INVALID_SOCKET) {
        int errorCode = WSAGetLastError();
        if (errorCode != WSAEWOULDBLOCK) {
            printf("Accept failed with error code: %d\n", errorCode);
        }
        else {
            //EngineERROR::consoleError("(Network Socket): Connection waiting...", EngineERROR::INFO_NORMAL);
        }
    }
    else {
        printf("server: got connection from %s port %d\n",
        inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        clientData data;
        data.clientAddr = cli_addr;
        data.sock = newsockfd;
        clientSocks.addElement(&data);
    }

}

void UreTechEngine::networkSystem::sendRecvToServer()
{
    char buffer[sizeof(replicating_func_dat_size)];
    int n;

    n = recv(sock, buffer, sizeof(replicating_func_dat_size), 0);
    EngineERROR::consoleError(std::string("(DEBUG): return:")+std::to_string(n), EngineERROR::INFO_NORMAL);
    if (n >= 0) {
        memcpy(replicating_func_dat_i, buffer, sizeof(replicating_func_dat_size));
        if (n != sizeof(networkReplicationStruct)) {
            EngineERROR::consoleError(std::string("(Network Socket): Package is not valid! !="+ std::to_string(sizeof(networkReplicationStruct)) +" bytes"), EngineERROR::ERROR_NORMAL);
        }
    }
   // EngineERROR::consoleError("(DEBUG): enter message", EngineERROR::INFO_NORMAL);
    //fgets(buffer, 255, stdin);
    memcpy(buffer, &replicating_func_dat_o, sizeof(replicating_func_dat_size));

    n = send(sock, buffer, sizeof(replicating_func_dat_size), 0);
    EngineERROR::consoleError(std::string(buffer), EngineERROR::ERROR_ERROR);
    if (n < 0) {
        EngineERROR::consoleError(std::string("(Network Socket): Can not write to server!"), EngineERROR::ERROR_NORMAL);
    }
}

void UreTechEngine::networkSystem::sendRecvToClient()
{
    char buffer[sizeof(replicating_func_dat_size)];
    for (int i = 0; i < clientSocks.size(); i++) {

        // This send() function sends the 13 bytes of the string to the new socket
        clientData* toSendSock = (clientData*)clientSocks.getIndex(i);
        send(toSendSock->sock, (char*)replicating_func_dat_o, sizeof(replicating_func_dat_size), 0);

        int n;
        n = recv(toSendSock->sock, (char*)replicating_func_dat_i, sizeof(replicating_func_dat_size), 0);

        if (n >= 0) {
            EngineERROR::consoleError(std::string("(DEBUG): return:") + std::to_string(n), EngineERROR::INFO_NORMAL);
            EngineERROR::consoleError(std::string("(Network Socket): Read to socket:") + std::string(buffer), EngineERROR::INFO_NORMAL);
            if (n != 255) {
                EngineERROR::consoleError(std::string("(Network Socket): Package is not valid! <13 bytes"), EngineERROR::ERROR_NORMAL);
            }
        }
        /*if (n < 0) {
            EngineERROR::consoleError(std::string("(Network Socket): Failed read socket"), EngineERROR::WARN_CAN_CAUSE_ERROR);
        }*/
    }
}

UreTechEngine::networkSystem::networkSystem()
{
    clientSocks.setElemSize(sizeof(clientData));
}

