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

#include <map>

#include "LocalServerDiscoverer.h"
#include "Util/Logger.h"
#include "Util/Converter.h"

LocalServerDiscoverer::LocalServerDiscoverer(std::string discoveryPort,std::string headerName,int headerID,LocalServerDiscovererInterface* callbackObject)
{
    this->discoveryPort=discoveryPort;
    this->headerID=headerID;
    this->headerName=headerName;
    this->callbackObject=callbackObject;
    searching=false;
    initialize();
}

void LocalServerDiscoverer::initialize()
{
    udpListener=new UdpListener(discoveryPort,true);
    udpListener->enableCallback(this);
    timer=new CallbackTimer(this);
    Logger::getSingleton()->addLine("LocalServerDiscoverer initialized");
}

void LocalServerDiscoverer::findServer(int timeoutMilliseconds)
{
    if(!searching)
    {
        serverList.clear();
        searching=true;
        serverIpAddress="";
        serverName="";
        udpListener->startListening();
        timer->start(timeoutMilliseconds,false);
        Logger::getSingleton()->addLine("LocalServerDiscoverer looking for local server(s)...");
        Logger::getSingleton()->addLine("LocalServerDiscoverer timeout is set to "+Converter::intToString(timeoutMilliseconds)+" milliseconds");
    }
}

void LocalServerDiscoverer::udpMessageReceivedEvent(std::string ipAddress,char* message, size_t size)
{
    if(searching)
    {
        LocalDiscoveryHeader_t* header=(LocalDiscoveryHeader_t*)message;
        if(size>=sizeof(LocalDiscoveryHeader_t) && std::string(header->headerName)==headerName && header->headerID==headerID)
        {
            serverIpAddress=ipAddress;
            serverName=std::string(header->serverName);
            serverList[ipAddress]=serverName;
        }
    }
    delete message;
}

void LocalServerDiscoverer::timeElapsedEvent()
{
    searching=false;
    udpListener->stop();
    timer->stop();
    callbackObject->discoveryFinalizedEvent(serverList);
    if(serverList.size()>0)
    {
        Logger::getSingleton()->addLine("LocalServerDiscoverer: servers discovered on the subnet:");
        for(std::map<std::string,std::string>::iterator iter=serverList.begin();iter!=serverList.end();iter++)
            Logger::getSingleton()->addLine("LocalServerDiscoverer: --- Server: '"+iter->second+ "' at "+iter->first);
    }
    else
        Logger::getSingleton()->addLine("LocalServerDiscoverer: no servers found on the subnet");
}