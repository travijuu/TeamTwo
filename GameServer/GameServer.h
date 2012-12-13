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

#ifndef GAMESERVER_H
#define	GAMESERVER_H

#include <iostream>
#include <map>
#include <UdpClient.h>
#include <UdpListener.h>
#include <TcpConnector.h>
#include <ConfigReader.h>
#include <Logger.h>
#include "Client.h"
#include "GameRoom.h"
#include "GameLobby.h"
#include "LocalDiscoveryController.h"

/*	GameServer

	This class collects the main and 
	necessary components of the server,
	enables and initilizes them

*/



class GameServer
{
public:
	//constructor. takes the configuration file's name in order to initialize
    GameServer(std::string configFileName);
	//starts server
    void start();
	//enables local discovery advertiser
    void enableLocalDiscovery();
	//disables local discovery advertiser
    void disableLocalDiscovery();
private:
	//initilizes the system
    void initialize();
    LocalDiscoveryController* localDiscoveryController;
    TcpConnector* tcpConnector;
    int serverId;
    std::string serverName;

    int lsc_timeInterval;
    int lsc_headerID;
    std::string lsc_headerName;
    std::string lsc_port;

    std::string serverTcpPort;
    std::string serverUdpPort;
    std::string minTcpPort;
    std::string maxTcpPort;
    std::string minUdpPort;
    std::string maxUdpPort;
    
    GameLobby* gameLobby;
    Logger* logger;
    ConfigReader* configReader;
    std::string gameLogFileName;
};

#endif

