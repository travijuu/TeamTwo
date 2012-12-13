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

#include "LocalDiscoveryController.h"
#include "MessageStructure.h"
#include "Logger.h"
#include "Converter.h"

LocalDiscoveryController::LocalDiscoveryController(std::string serverName,std::string headerName,int headerID, int timeInterval, std::string udpPort)
{
    this->serverName=serverName.substr(0,SERVER_NAME_SIZE);
    this->headerID=headerID;
    this->timeInterval=timeInterval;
    this->udpPort=udpPort;
    this->headerName=headerName;
    this->serverName=serverName;
    timer=new CallbackTimer(this);
    udpClient=new UdpClient();
    Logger::getSingleton()->addLine("LocalServerController initialized");
    Logger::getSingleton()->addLine("LocalServerController broadcast time interval: "+Converter::intToString(timeInterval)+" milliseconds");
    Logger::getSingleton()->addLine("LocalServerController broadcast udp port: "+udpPort);
}

void LocalDiscoveryController::startBroadcasting()
{
    Logger::getSingleton()->addLine("LocalServerController starting to broadcast...");
    timer->start(timeInterval);
}

void LocalDiscoveryController::stopBroadcasting()
{
    timer->stop();
}

void LocalDiscoveryController::timeElapsedEvent()
{
    LocalDiscoveryHeader_t header;
    strcpy(header.headerName,headerName.c_str());
    strcpy(header.serverName,serverName.c_str());
    header.headerID=this->headerID;
    udpClient->sendData("255.255.255.255",udpPort,(char*)&header,sizeof(LocalDiscoveryHeader_t));
}