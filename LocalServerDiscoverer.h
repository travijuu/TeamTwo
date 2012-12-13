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

#ifndef LOCALSERVERDISCOVERER_H
#define	LOCALSERVERDISCOVERER_H

#include "NetworkUtil/UdpListener.h"
#include "Util/CallbackTimer.h"
#include "Util/CallbackTimerInterface.h"
#include "NetworkUtil/MessageStructure.h"
#include "LocalServerDiscovererInterface.h"

/* Local Server Discoverer
 *
 * This class is used for detecting local
 * server that are running in the same
 * local area network using Local Server
 * Controller class. It simply listens
 * the IPV4 broadcast address. CallbackTimer
 * and UdpListener is used. A Callback structure
 * is utilized to provide an event-based
 * mechanism.
 */

class LocalServerDiscoverer:public CallbackTimerInterface,public UdpCallbackInterface
{

public:
    //constructor. takes a UDP port, header name of the discovery protocol its id and a callback object.
    LocalServerDiscoverer(std::string port,std::string headerName,int headerID,LocalServerDiscovererInterface* callbackObject);
    //listens the given UDP port for timeOutMilliseconds
    void findServer(int timeoutMilliseconds);
    //callback method for UdpListener. it's called when a message has been received
    void udpMessageReceivedEvent(std::string ipAddress,char* message, size_t size);
    //callback method for CallbackTimerInterface. called when the given time has elapsed
    void timeElapsedEvent();
private:
    //initializes the LocalServerDiscoverer
    void initialize();

    LocalServerDiscovererInterface* callbackObject;
    CallbackTimer* timer;
    UdpListener* udpListener;
    std::string discoveryPort;
    std::string headerName;
    int headerID;
    int timeoutMilliseconds;
    bool searching;
    bool found;
    std::string serverIpAddress;
    std::string serverName;
    std::map<std::string,std::string> serverList;
};

#endif	/* LOCALSERVERDISCOVERER_H */

