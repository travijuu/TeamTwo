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

#include "UdpListener.h" 

UdpListener::UdpListener(std::string port,bool receiveBroadcasts,std::string multicastIP)
{
#ifdef WINDOWS_PLATFORM
	WSAData wsa;
	if(WSAStartup(MAKEWORD(2,0),&wsa)!=0)
	{
		std::cout<<"NetworkUtil: WSA ERROR!"<<std::endl;
		exit(1);
	}
#endif
	if(multicastIP!="NULL")
	{
		this->multicastIP=multicastIP;
		multicastMode=true;
	}
	else
		multicastMode=false;
    this->receiveBroadcasts=receiveBroadcasts;
    callbackSet=false;
    socket_id=-1;
    memset(&hint,0,sizeof hint);
    hint.ai_socktype=SOCK_DGRAM;
    hint.ai_family=AF_INET;
    hint.ai_flags=AI_PASSIVE;
    getaddrinfo(NULL,port.data(),&hint,&info);
    running=false;
}

void UdpListener::enableCallback(UdpCallbackInterface* callbackObject)
{
    this->callbackObject=callbackObject;
    callbackSet=true;
}

void UdpListener::disableCallback()
{
    callbackSet=false;
}
int UdpListener::createSocket()
{
    int _socketid=socket(info->ai_family,info->ai_socktype,info->ai_protocol);
    if(_socketid==-1)
    {
        std::cout<<"Socket Error!\n";
        return -1;
    }

    if(receiveBroadcasts)
    {
        int _bool=1;
        setsockopt(_socketid,SOL_SOCKET,SO_BROADCAST,(char*)&_bool,sizeof(_bool));
    }
	else if(multicastMode)
	{std::cout<<"multi\n"<<multicastIP<<std::endl;
		struct ip_mreq multicastGrRequest;
		multicastGrRequest.imr_multiaddr.S_un.S_addr=inet_addr(multicastIP.c_str());
		multicastGrRequest.imr_interface.S_un.S_addr=htonl(INADDR_ANY);
		if(setsockopt(_socketid,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&multicastGrRequest,sizeof(ip_mreq))<0)
			std::cout<<"patlar\n";
	}

    return _socketid;
}

void UdpListener::startListening()
{
	if(socket_id==-1)
	{
		if((socket_id=createSocket())==-1 && !running)
			std::cout<<"Socket creation error or UdpListener is already running\n";
		else
		{
			int bindResult=bind(socket_id,info->ai_addr,info->ai_addrlen);
			if(bindResult<0)
				std::cout<<"Socket bind error!\n";
			else
			{
				running=true;
				rthread=new boost::thread(&UdpListener::receiveMessages,this);
			}
		}
	}
	else
		std::cout<<"Socket is still open!\n";
}

void UdpListener::receiveMessages()
{
    struct sockaddr_storage incomingInfo;
    socklen_t len=INET_ADDRSTRLEN;
    char buffer[1024];
    char* message;
    int bytes;
    while(running)
    {
        bytes=recvfrom(socket_id,buffer,sizeof(buffer),0,(struct sockaddr *)&incomingInfo,&len);
        if(bytes==-1)
            continue;
        
        if(bytes==0)
		{
			terminateConnection();
			break;
		}
        
        else if(running)
        {
            std::string ipAddress=inet_ntoa(((struct sockaddr_in*)&incomingInfo)->sin_addr);
            message=new char[bytes];
            memcpy(message,buffer,bytes);
            if(callbackSet && callbackObject!=NULL)
                callbackObject->udpMessageReceivedEvent(ipAddress,message,bytes);
            else
            {
                messageVector.push_back(message);
            }
        }
        else
        {
			terminateConnection();
            break;
        }
    }
}

void* UdpListener::getMessage()
{
    void* message=NULL;
    if(messageVector.size()>0)
    {
        message=messageVector.at(0);
        messageVector.erase(messageVector.begin());
    }
    return message;
}

void UdpListener::stop()
{
	terminateConnection();
}

void UdpListener::terminateConnection()
{
	running=false;
#ifdef WINDOWS_PLATFORM
	closesocket(socket_id);
#else
	close(socket_id);
#endif
	socket_id=-1;
}