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

#ifndef LOCALDISCOVERYCONTROLLER_H
#define	LOCALDISCOVERYCONTROLLER_H

#include "UdpClient.h"
#include "CallbackTimer.h"
#include "CallbackTimerInterface.h"

/*	LocalDiscoveryController

	The purpose of this class is to enable
	TTLSDP protocol on the device. System 
	simply broadcasts itself on the network
	so that the client device can detect it.
	TTLSDP also advertises server's name for
	identification purposes.
*/

class LocalDiscoveryController:public CallbackTimerInterface
{

public:
	//constructor. takes the server's name, TTLSDP header name & id, broadcast
	//time interval and the UDP port that is going to be used for broadcasting.
    LocalDiscoveryController(std::string serverName,std::string headerName,int headerID, int timeInterval, std::string udpPort);
    //starts broadcasting on the subnet
	void startBroadcasting();
	//stops broadcasting on the subnet
    void stopBroadcasting();
	//CallbackTimer's event method
    void timeElapsedEvent();
private:
    std::string serverName;
    std::string headerName;
    int headerID;
    UdpClient* udpClient;
    std::string udpPort;
    int timeInterval;
    CallbackTimer* timer;

};

#endif	/* LOCALDISCOVERYCONTROLLER_H */

