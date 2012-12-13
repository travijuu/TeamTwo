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

#ifndef GAMEROOM_H
#define	GAMEROOM_H
#include <iostream>
#include "GameClient.h"

/* GameRoom

   This class is mainly used by the GameMessageController
   in order to hold data about the game room which is located
   on the server.
*/

class GameRoom
{
public:
	//constructor of the class. takes the room's identifier,
	//room's name, its current player count, capacity, level's name
	//and the type of the game (deathmatch or team deathmatch)
    GameRoom(unsigned int roomID,std::string roomName,int playerCount,int capacity,std::string mapName,unsigned char gameType);
    //returns room id
	unsigned int getRoomID();
	//returns room name
    std::string getRoomName();
	//returns current player count
    int getPlayerCount();
	//returns tcp port of the game room
    void setTcpPort(int tcpPort);
	//sets the UDP port number of the game room on the server
	//which is going to be used for sending transform updatess
    void setUdpServerPort(int udpPort);
	//sets the UDP client port which is going to be listened
	//for incoming transform updates of other players
    void setUdpClientPort(int udpPort);
	//sets the TTFTP port of the room which is going to be utilized
	//for downloading level files from the TTFTP file server
    void setTTFTPPort(int TTFTPPort);
	//return TCP port of the game room which is going to be used
	//for TTNP protocol
    int getTcpPort();
	//returns the UDP port of game room
    int getUdpServerPort();
	//returns the UDP client port
    int getUdpClientPort();
	//returns the TTFTP port
    int getTTFTPPort();
	//returns the map name
    std::string getMapName();
	//adds a client to the game room by taking a GameClient object
    void addClient(GameClient* client);
	//removes a client by the given id
    void removeClient(unsigned int id);
	//returns the client object by the given id
    GameClient* getClient(unsigned int id);
	//returns the capacity of the game room
    int getCapacity();
	//sets the current player count of the game room
    void setPlayerCount(int count);
	//sets map name of the game room
    void setMapName(std::string mapName);
	//sets the game type of the game room (deathmatch or team deathmatch)
    void setGameType(unsigned char gameType);
	//returns the owner id of the game room
    unsigned int getOwner();
	//sets the owner of the game room by id
    void setOwner(unsigned int owner);
	//returns a map which contains the GameClient objects
    std::map<unsigned int,GameClient*> getClientMap();
	//returns the game type
    unsigned char getGameType();
private:
    unsigned char gameType;
    int capacity;
    std::string mapName;
    std::map<unsigned int,GameClient*> clientMap;
    unsigned int roomID;
    std::string roomName;
    int playerCount;
    int udpServerPort;
    int udpClientPort;
    int tcpPort;
    int TTFTPPort;
    unsigned int owner;
};

#endif	/* GAMEROOM_H */

