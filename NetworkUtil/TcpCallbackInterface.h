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

#ifndef TCPCALLBACKINTERFACE_H
#define TCPCALLBACKINTERFACE_H
#include <iostream>

/* Tcp Callback Interface
 *
 * The callback interface structure of
 * TcpConnector class which calls the
 * related callback methods when the
 * defined event occurs.
 */


class TcpCallbackInterface
{
public:
    //called by the TcpConnector when a TCP message that was declared in the correct format has been received
    //it provides the IP address and the socket number of the client, the message and its size.
    virtual void tcpMessageReceivedEvent(std::string ipAddress,int connectionIdentifier,char* message,size_t messageSize){};
    //called by the TcpConnector when a TCP connection has been terminated from the other side. Socket number
    //is provided for notifying which client has terminated the connection.
    virtual void connectionTerminatedEvent(int socket){};
};



#endif
