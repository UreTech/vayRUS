#include "network.h"

#include<WinSock2.h>
#include<string>

#include<../EngineCore.h>

namespace UreTechEngine {
    class EngineConsole;
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
        EngineConsole::consoleError(std::string("(Network Socket): Failed to WSAStartup! ERROR CODE:") + std::to_string(result), EngineConsole::ERROR_FATAL);

    }

    sock=socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        EngineConsole::consoleError(std::string("(Network Socket): Failed to open socket! ERROR CODE:")+ std::to_string(sock), EngineConsole::ERROR_FATAL);
    }

    // Non-blocking mod ayarlanýyor(GPT kodu bu) // düzeltilmeli bloklamýyo ama accept edemiyor
    u_long mode = 1;
    if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR) {
        EngineConsole::consoleError(std::string("(Network Socket): Non blocking socket error!") + std::to_string(sock), EngineConsole::ERROR_FATAL);
    }

    EngineConsole::consoleError("Socket: Server Socket Started!", EngineConsole::INFO_NORMAL);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // automatically be filled with current host's IP address
    serv_addr.sin_port = htons(port);
    int bind_Err;
    if (bind_Err = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        EngineConsole::consoleError(std::string("(Network Socket): Failed to bind! ERROR CODE:") + std::to_string(bind_Err), EngineConsole::ERROR_FATAL);
    }

    if (listen(sock, 5)==SOCKET_ERROR) {
        EngineConsole::consoleError(std::string("(Network Socket): Failed to listen!"), EngineConsole::ERROR_FATAL);
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
        EngineConsole::consoleError(std::string("(Network Socket): Failed to WSAStartup! ERROR CODE:") + std::to_string(result), EngineConsole::ERROR_FATAL);

    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        EngineConsole::consoleError(std::string("(Network Socket): Failed to open socket! ERROR CODE:") + std::to_string(sock), EngineConsole::ERROR_FATAL);
    }

    EngineConsole::consoleError("Socket: Client Socket Started!", EngineConsole::INFO_NORMAL);

    UreTechEngineClass::getEngine()->isServer = false;
    UreTechEngineClass::getEngine()->isInServer = true;

    if (invalidIP) {
        EngineConsole::consoleError("(Network Socket): Server is not available!",EngineConsole::ERROR_NORMAL);
    }
    else {
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            EngineConsole::consoleError(std::string("(Network Socket): Can not connect to server!"), EngineConsole::ERROR_NORMAL);
        }
    }
}

void UreTechEngine::networkSystem::setToConnectIPAddr(std::string _ip, std::string _port)
{
    serv_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, _ip.c_str(), &serv_addr.sin_addr) <= 0) { 
        EngineConsole::consoleError(std::string("(Network Socket): Invalid ip addres!"), EngineConsole::ERROR_NORMAL);
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
    EngineConsole::consoleError(std::string("(DEBUG): return:")+std::to_string(n), EngineConsole::INFO_NORMAL);
    if (n >= 0) {
        memcpy(replicating_func_dat_i, buffer, sizeof(replicating_func_dat_size));
        if (n != sizeof(networkReplicationStruct)) {
            EngineConsole::consoleError(std::string("(Network Socket): Package is not valid! !="+ std::to_string(sizeof(networkReplicationStruct)) +" bytes"), EngineConsole::ERROR_NORMAL);
        }
    }
   // EngineERROR::consoleError("(DEBUG): enter message", EngineERROR::INFO_NORMAL);
    //fgets(buffer, 255, stdin);
    memcpy(buffer, &replicating_func_dat_o, sizeof(replicating_func_dat_size));

    n = send(sock, buffer, sizeof(replicating_func_dat_size), 0);
    EngineConsole::consoleError(std::string(buffer), EngineConsole::ERROR_ERROR);
    if (n < 0) {
        EngineConsole::consoleError(std::string("(Network Socket): Can not write to server!"), EngineConsole::ERROR_NORMAL);
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
            EngineConsole::consoleError(std::string("(DEBUG): return:") + std::to_string(n), EngineConsole::INFO_NORMAL);
            EngineConsole::consoleError(std::string("(Network Socket): Read to socket:") + std::string(buffer), EngineConsole::INFO_NORMAL);
            if (n != 255) {
                EngineConsole::consoleError(std::string("(Network Socket): Package is not valid! <13 bytes"), EngineConsole::ERROR_NORMAL);
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

