/*
	Copyright (C) 2011-2012 Alican Sekerefe

	TeamTwo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TeamTwo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Contact: projectteamtwo@gmail.com
*/


#include "TcpConnector.h"

typedef struct DataHeader
{
    int size;
    char data[1024*512];
}DataHeader_t;


TcpConnector::TcpConnector(std::string port)
{
#ifdef WINDOWS_PLATFORM
    WSAData wsa;
    if (WSAStartup(MAKEWORD(2, 0), &wsa) != 0)
        std::cout << "NetworkUtil: WSA ERROR!" << std::endl;
#endif

    running = callbackSet = customCallbackEnabled = false;
    localSocket = -1;
    listenMode = true;
    struct addrinfo hint;
    memset(&hint, 0, sizeof hint);
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_INET;
    hint.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port.data(), &hint, &info);
    athread = rthread = NULL;
    callbackObject = NULL;
}

TcpConnector::TcpConnector(std::string address,std::string port)
{
    running=callbackSet=customCallbackEnabled=listenMode=false;
    ip=address;
    localSocket = -1;
    getaddrinfo(address.data(), port.data(), NULL, &info);
    athread = rthread = NULL;
    callbackObject = NULL;
}

void TcpConnector::enableCallback(TcpCallbackInterface* callbackObject)
{
    this->callbackObject=callbackObject;
    callbackSet=true;
}

void TcpConnector::enableCustomCallback(TcpCallbackInterface* callbackObject, newMessageCallbackMethod nmCallback,connectionTerminatedCallbackMethod ctCallback)
{
    enableCallback(callbackObject);
    this->newMessageCallback=nmCallback;
    this->connectionTerminatedCallback=ctCallback;
    customCallbackEnabled=true;
}

void TcpConnector::disableCallback()
{
    callbackSet=false;
}

int TcpConnector::disableTcpBuffering(int socket)
{
    int x=1;
    int res=setsockopt(socket,IPPROTO_TCP,TCP_NODELAY,(char*)&x,sizeof(int));
    if(res<0)
    {
        int error=errno;
        printf("*******sockopterror: %s\n",strerror(error));
    }
    return res;
}

int TcpConnector::createSocket()
{
    return socket(info->ai_family,info->ai_socktype,info->ai_protocol);
}

void TcpConnector::startListening()
{
    if(localSocket==-1)
    {
        int _socket=createSocket();
        if(_socket>-1)
        {
            int bindResult=bind(_socket,info->ai_addr,info->ai_addrlen);
            if(bindResult>-1)
            {
                int listenResult=listen(_socket,maxQueueNumber);
                if(listenResult>-1)
                {
                    running=true;
                    athread=new boost::thread(&TcpConnector::acceptRequests,this);
                    localSocket=_socket;
                }
                else
                    std::cout<<"listening failed!\n";
            }
            else
                std::cout<<"bind error!\n";
        }
        else
            std::cout<<"cannot create socket!\n";
    }
    else
        std::cout<<"listening already started!\n";
}

bool TcpConnector::connectToHost()
{
    bool connected=false;
    int _socket;
    if(localSocket==-1)
    {
        _socket=createSocket();
        if(_socket!=-1)
        {
            int connectionResult=connect(_socket,info->ai_addr,info->ai_addrlen);
            if(connectionResult<0)
                std::cout<<"connection failed!\n";
            else
            {
                rthread=new boost::thread(&TcpConnector::receiveMessages,this,_socket);
                connected=running=true;
                localSocket=_socket;
            }
        }
        else
            std::cout<<"socket cannot be created!\n";
    }
    else
    {
        std::cout<<"Already connected!\n";
    }
    return connected;
}



bool TcpConnector::sendString(std::string message)
{
    char* mess;
    strcpy(mess,message.data());
    bool result=_send(mess,message.length());
    return result;
}

bool TcpConnector::_send(void* data,size_t size)
{
    bool result;
    if(!listenMode)
    {
        DataHeader_t* header=new DataHeader_t;
        header->size=size;
        memcpy(header->data, data, size);
        int sentBytes = send(localSocket, (char*) header, size + 4, 0);
		if(sentBytes<size+4)
			std::cout<<"**********TCP SEND ERROR! "<<(size+4)-sentBytes<<" not sent!\n";
        result = (bool)sentBytes;
        delete header;
    }
    else
        result=false;
    return result;
}

bool TcpConnector::sendData(int socket,void* data,size_t size)
{
    DataHeader_t* header = new DataHeader_t;
    header->size = size;
    memcpy(header->data, data, size);
    int sentBytes = send(socket, (char*) header, size + 4, 0);
	if(sentBytes<size+4)
		std::cout<<"**********TCP SEND ERROR! "<<(size+4)-sentBytes<<" not sent!\n";
    bool result = (bool)sentBytes;
    delete header;
    return result;
}

bool TcpConnector::sendData(void* data,size_t size)
{
    return (bool)_send(data,size);
}

void TcpConnector::stop()
{
    running = false;
    shutdown(localSocket, 2);
    if (rthread != NULL && boost::this_thread::get_id() != rthread->get_id())
        rthread->join();
    localSocket = -1;
}

std::string TcpConnector::getAddress(int socketId)
{
    struct sockaddr_storage info=clientInfo[socketId];
    char addrbuffer[INET_ADDRSTRLEN];
    inet_ntop(info.ss_family,&(((sockaddr_in*)&info)->sin_addr),addrbuffer,INET_ADDRSTRLEN);
    return std::string(addrbuffer);
}

void* TcpConnector::acceptRequests()
{
    TcpConnector* self=this;
    struct sockaddr_storage cInfo;
    int clientSocket;
    socklen_t addrSize=sizeof(sockaddr_storage);
    while(localSocket!=-1 && running)
    {
        clientSocket=accept(self->localSocket,(sockaddr*)&cInfo,&addrSize);
        if(clientSocket==-1)
            continue;
        self->clientInfo[clientSocket]=cInfo;
        rthread=new boost::thread(&TcpConnector::receiveMessages,this,clientSocket);
    }
	return NULL;
}

void* TcpConnector::receiveMessages(int clientSocket)
{
    char buffer[1024*512];
    char headerBuffer[sizeof(int)];
    char* message;
    int bytes;
    int remainingBytes;
    struct sockaddr_in* info=(struct sockaddr_in*)(&clientInfo[clientSocket]);
    std::string ipAddress=inet_ntoa(info->sin_addr);
    while(localSocket!=-1 && running)
    {
        bytes=recv(clientSocket,headerBuffer,sizeof(headerBuffer),0);
        int* size=(int*)headerBuffer;
        if(bytes<=0)
        {
            terminateConnection(clientSocket);
            if(callbackSet)
            {
                if(!customCallbackEnabled)
                    callbackObject->connectionTerminatedEvent(clientSocket);
                else
                    (callbackObject->*connectionTerminatedCallback)(clientSocket);
            }
            break;
        }
        else if(running)
        {
            remainingBytes=*size;
            message=new char[*size];
            while(remainingBytes!=0 || (bytes!=0 && remainingBytes>0))
            {
                bytes=recv(clientSocket,message+(*size-remainingBytes),remainingBytes,0);
                remainingBytes-=bytes;
            }
            if (bytes == 0)
            {
                terminateConnection(clientSocket);
                if (callbackSet)
                {
                    if (!customCallbackEnabled)
                        callbackObject->connectionTerminatedEvent(clientSocket);
                    else
                        (callbackObject->*connectionTerminatedCallback)(clientSocket);
                }
                break;
            }
            else if(callbackSet && callbackObject!=NULL)
            {
                if(!customCallbackEnabled)
                    callbackObject->tcpMessageReceivedEvent(ipAddress,clientSocket,message,bytes);
                else
                    (callbackObject->*newMessageCallback)(ipAddress,clientSocket,message,bytes);
            }
            else
                messageVector.push_back(message);
        }
    }
	return NULL;
}

void* TcpConnector::getMessage()
{
    void* message=NULL;
    if(messageVector.size()>0)
    {
        message=messageVector.at(0);
        messageVector.erase(messageVector.begin());
    }
	return message;
}

void TcpConnector::terminateConnection(int socket)
{
    if(!listenMode)
    {
        localSocket=-1;
        running=false;
    }
#ifdef WINDOWS_PLATFORM
	shutdown(socket,2);
#else
    close(socket);
#endif
}