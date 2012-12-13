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

#ifndef UDPLISTENER_H
#define	UDPLISTENER_H

#include <iostream>
#include <boost/thread.hpp>
#include <string.h>
#include <vector>
#include "UdpCallbackInterface.h"

#ifdef WINDOWS_PLATFORM

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

#pragma comment(lib, "Ws2_32.lib")

#else

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#endif

/*	UdpListener

	This class is responsible for providing a platform
	independent UDP port listener with the feature of
	multithreading. for multithreading, it utilizes from
	boost::threads. UdpListener is based on publish-subscribe
	design pattern. Subscriber must inherit UdpCallbackInterface
	in order to use it. Subscriber enable its own callback method.


*/

class UdpListener
{
public:
	//constructor of the UdpListener. UDP port to be listened is passed.
	//second parameter indicates that whether the ipv4 broadcast (255.255.255.255)
	//is going to be used or not.
	UdpListener(std::string port,bool receiveBroadcasts=false,std::string multicastIP="NULL");
	//starts listening to the given port
    void startListening();
	//terminates listening operations and deletes the socket
    void stop();
	//returns the message that was added to the message queue (depcerated)
    void* getMessage();
	//enables callback system.
    void enableCallback(UdpCallbackInterface* callbackObject);
	//disables callback system. incoming messages are added to a message queue
    void disableCallback();
private:
	//terminates listening. called by the stop()
	void terminateConnection();

    int socket_id;
    int port;
    UdpCallbackInterface* callbackObject;
    bool callbackSet;
    bool receiveBroadcasts;
    bool running;
    struct addrinfo* info;
    struct addrinfo hint;
    boost::thread* rthread;
    void receiveMessages();
    int createSocket();
    std::vector<void*> messageVector;
	std::string multicastIP;
	bool multicastMode;
};

#endif

