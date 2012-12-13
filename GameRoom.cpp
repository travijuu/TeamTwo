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

#include "GameRoom.h"

GameRoom::GameRoom(unsigned int roomID,std::string roomName,int playerCount,int capacity,std::string mapName,unsigned char gameType)
{
    this->roomID=roomID;
    this->roomName=roomName;
    this->playerCount=playerCount;
    this->mapName=mapName;
    this->capacity=capacity;
    this->owner=-1;
    this->gameType=gameType;
}

int GameRoom::getCapacity()
{
    return capacity;
}

unsigned int GameRoom::getRoomID()
{
    return roomID;
}

std::string GameRoom::getMapName()
{
    return mapName;
}


std::string GameRoom::getRoomName()
{
    return roomName;
}


int GameRoom::getPlayerCount()
{
    return playerCount;
}

void GameRoom::setTcpPort(int tcpPort)
{
    this->tcpPort=tcpPort;
}

void GameRoom::setUdpServerPort(int udpPort)
{
    this->udpServerPort=udpPort;
}

void GameRoom::setUdpClientPort(int udpPort)
{
    this->udpClientPort=udpPort;
}

int GameRoom::getTcpPort()
{
    return tcpPort;
}

int GameRoom::getUdpServerPort()
{
    return udpServerPort;
}

int GameRoom::getUdpClientPort()
{
    return udpClientPort;
}

void GameRoom::addClient(GameClient* client)
{
    clientMap[client->getID()]=client;
}

void GameRoom::removeClient(unsigned int id)
{
    clientMap.erase(clientMap.find(id));
}

GameClient* GameRoom::getClient(unsigned int id)
{
    GameClient* client=NULL;
    if(clientMap.find(id)!=clientMap.end())
        client=clientMap[id];
    return client;
}
void GameRoom::setPlayerCount(int count)
{
    playerCount=count;
}

void GameRoom::setMapName(std::string mapName)
{
    this->mapName=mapName;
}

void GameRoom::setGameType(unsigned char gameType)
{
    this->gameType=gameType;
}

unsigned char GameRoom::getGameType()
{
    return gameType;
}

unsigned int GameRoom::getOwner()
{
    return owner;
}

void GameRoom::setOwner(unsigned int owner)
{
    this->owner=owner;
}

std::map<unsigned int,GameClient*> GameRoom::getClientMap()
{
    return clientMap;
}

void GameRoom::setTTFTPPort(int TTFTPPort)
{
    this->TTFTPPort=TTFTPPort;
}

int GameRoom::getTTFTPPort()
{
    return TTFTPPort;
}
