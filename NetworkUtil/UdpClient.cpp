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

#include "UdpClient.h"

UdpClient::UdpClient()
{
#ifdef WINDOWS_PLATFORM
	WSAData wsa;
	if(WSAStartup(MAKEWORD(2,0),&wsa)!=0)
            std::cout<<"NetworkUtil: WSA ERROR!"<<std::endl;
#endif
        this->_socket_id=socket(AF_INET, SOCK_DGRAM,0);
}
#include <stdio.h>
bool UdpClient::sendData(std::string targetIp,std::string port,void* message,size_t message_size)
{
    bool result=false;
    struct addrinfo* info;
    getaddrinfo(targetIp.data(),port.data(),NULL,&info);

    if(targetIp.compare("255.255.255.255")==0)
    {
        int _bool=1;
        setsockopt(_socket_id,SOL_SOCKET,SO_BROADCAST,(char*)&_bool,sizeof(_bool));
    }

    if(_socket_id!=-1 && info!=NULL)
	{
		int sent=sendto(_socket_id,(char*)message,message_size,0,info->ai_addr,INET_ADDRSTRLEN);
		if(sent<message_size)
			std::cout<<"***********UDP SEND ERROR! "<<(message_size-sent)<<" bytes not sent\n";
        result=(bool)sent;
	}
    return result;
}