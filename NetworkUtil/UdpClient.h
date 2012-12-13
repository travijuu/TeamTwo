/*
	Copyright (C) 2011-2012  Alican Sekerefe 

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

#ifndef UDPCLIENT
#define UDPCLIENT

#include <iostream>

#ifdef WINDOWS_PLATFORM

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#else

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#endif

/*	UdpClient

	This class is responsible for sending UDP packets
	to the given IPv4 address and port. It works platform
	independent and does not utilize from multithreading.

*/

class UdpClient
{
public:
	//constructor of the class. creates a socket.
    UdpClient();
	//sends data to the given ip address and port. 
    bool sendData(std::string targetIp,std::string port,void* message,size_t size);
private:
    int _socket_id;
};



#endif