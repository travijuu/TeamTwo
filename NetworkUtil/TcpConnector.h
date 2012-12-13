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

#ifndef TCPCONNECTOR_H
#define TCPCONNECTOR_H

#include <string.h>
#include <iostream>
#include <map>
#include <vector>
#include "TcpCallbackInterface.h"
#include <stdio.h>
#include <boost/thread.hpp>

#ifdef WINDOWS_PLATFORM


#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#else

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#endif

/* TcpConnector
 *
 * This is a cross-platform TCP networking library that
 * enables its user to perform both tcp listen and connect
 * type of operations using c sockets. boost::thread is
 * utilized for providing a non-blocking event-based structure.
 *
 */



//function pointer definition for message receving callback method. by default it is not used.
typedef void (TcpCallbackInterface::*newMessageCallbackMethod)(std::string ipAddress,int connectionIdentifier,char* message,size_t messageSize);
//function pointer definition for connection terminated event's callbacback method. by default it is not used.
typedef void (TcpCallbackInterface::*connectionTerminatedCallbackMethod)(int socket);




class TcpConnector
{
public:
	//first constructor of the class. Used for port listening operations
    TcpConnector(std::string port);
	//second constructor of the class. Used for establishing connections
	//and listening messages from the created socket
    TcpConnector(std::string address,std::string port);
	//starts listening. Needs to be used in the server mode.
    void startListening();
	//connects to the host. Used in the connection(TCP client) mode.
    bool connectToHost();
	//sends data to the given socket. This method must be used in the
	//server mode since it can have more than 1 sockets.
    bool sendData(int socket,void* data,size_t size);
	//sends data over the established tcp connection. must be used in the
	//client mode.
    bool sendData(void* data,size_t size);
	//sends a string (deprecated)
    bool sendString(std::string message);
	//stops listening/accepting operations and terminates the established connection
    void stop();
	//returns the message that was added to the message queue
    void* getMessage();
	//enables message callback system
    void enableCallback(TcpCallbackInterface* callbackObject);
	//enables custom callback system by taking a TcpCallbackInterface object and the
	//function pointer to the callback method that is going to be used.
    void enableCustomCallback(TcpCallbackInterface* callbackObject,newMessageCallbackMethod nmCallback,connectionTerminatedCallbackMethod ctCallback);
	//disables callback system
	void disableCallback();
	//termiantes connection by the given socket identifier
    void terminateConnection(int socket);

private:
	//disables nagle algorithm by disabling tcp buffering
    int disableTcpBuffering(int socket);
	//returns the ip address of the given socket identifier
    std::string getAddress(int socketId);
	//accepts incoming connections. can only be used in the server mode
    void* acceptRequests();
	//starts receiving messages from the established TCP connection by the given socket
    void* receiveMessages(int clientSocket);
	//sends a message over the established socket. must be used in the client mode
    bool _send(void* data,size_t size);
	//creates and returns the identifier of the created socket
    int createSocket();

    newMessageCallbackMethod newMessageCallback;
    connectionTerminatedCallbackMethod connectionTerminatedCallback;
    int localSocket;
    struct addrinfo* info;
    static const int maxQueueNumber=0;
    std::string ip;
    int port;
    bool running;
    boost::thread* athread;
    boost::thread* rthread;
    std::map<int,sockaddr_storage> clientInfo;
    std::vector<void*> messageVector;
    TcpCallbackInterface* callbackObject;
    bool callbackSet;
    bool listenMode;
    bool customCallbackEnabled;
};

#endif