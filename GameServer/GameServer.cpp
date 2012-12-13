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

#include "GameServer.h"

GameServer::GameServer(std::string configFileName)
{
    configReader=new ConfigReader(configFileName);
    if(configReader->readFile())
    {
        std::string logFileName=configReader->getFieldValueAsString("LogFileSettings","game-logfile");
        std::string logFileTitle=configReader->getFieldValueAsString("LogFileSettings","game-logfile-title");
        logger=Logger::getSingleton();
        logger->setFileName(logFileName);
        logger->setTitle(logFileTitle);
        logger->setConsolePrint(true);
        logger->begin();
        logger->addLine("Logger initialized");
        logger->addLine("initializing server components...");
        initialize();
    }
    else
        std::cout<<"Game configuration file not found!"<<std::endl;
}
    
void GameServer::initialize()
{
    serverName=configReader->getFieldValueAsString("GeneralSettings","server-name");
    serverTcpPort=configReader->getFieldValueAsString("NetworkSettings","server-tcp-port");
    serverUdpPort=configReader->getFieldValueAsString("NetworkSettings","server-udp-port");
    minTcpPort=configReader->getFieldValueAsString("NetworkSettings","game-min-tcp-port");
    maxTcpPort=configReader->getFieldValueAsString("NetworkSettings","game-max-tcp-port");
    minUdpPort=configReader->getFieldValueAsString("NetworkSettings","game-min-udp-port");
    maxUdpPort=configReader->getFieldValueAsString("NetworkSettings","game-max-udp-port");
    lsc_timeInterval=configReader->getFieldValueAsInteger("LocalServerDiscoverySettings","broadcast-time-interval");
    lsc_headerID=configReader->getFieldValueAsInteger("LocalServerDiscoverySettings","header-id");
    lsc_headerName=configReader->getFieldValueAsString("LocalServerDiscoverySettings","header-name");
    lsc_port=configReader->getFieldValueAsString("LocalServerDiscoverySettings","broadcast-port");

    if(lsc_headerName!="NULL" && lsc_headerID!=-1 && lsc_timeInterval!=-1 && lsc_port!="NULL")
        localDiscoveryController=new LocalDiscoveryController(serverName,lsc_headerName,lsc_headerID,lsc_timeInterval,lsc_port);
    else
        Logger::getSingleton()->addLine("LocalServerDiscoveryService error. check for values.");
    gameLobby=new GameLobby(serverTcpPort,logger,minTcpPort,maxTcpPort,minUdpPort,maxUdpPort);
}

void GameServer::enableLocalDiscovery()
{
    if(localDiscoveryController!=NULL)
        localDiscoveryController->startBroadcasting();
}

void GameServer::disableLocalDiscovery()
{
    if(localDiscoveryController!=NULL)
        localDiscoveryController->stopBroadcasting();
}

void GameServer::start()
{
    gameLobby->start();
}