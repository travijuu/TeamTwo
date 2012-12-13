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

#ifndef GAMELOBBY_H
#define GAMELOBBY_H

#include <iostream>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <UdpClient.h>
#include <UdpListener.h>
#include <TcpConnector.h>
#include <ConfigReader.h>
#include <Logger.h>
#include "Client.h"
#include "GameRoom.h"
#include <TcpCallbackInterface.h>
#include <MessageStructure.h>
#include <string.h>
#include <map>
#include "Converter.h"
#include <RoomEventInterface.h>

/*	GameLobby class

	This class is responsible for handling
	lobby. Main purpose of this class is to
	provide server operations that uses TTNP
	as the layer-7 protocol. It accepts and 
	processes incoming messages, creates game rooms
	generates necessary port numbers etc. Simply,
	it is the server.

*/

class GameLobby: public TcpCallbackInterface, public RoomEventInterface
{
public:
	//constructor of the class. requires a tcp port for the server, a logger object, and port generation intervals
    GameLobby(std::string lobbyTcpPort,Logger* logger,std::string minTcpPort,std::string maxTcpPort,std::string minUdpPort,std::string maxUdpPort);
	//starts server operations (listens and accepts incoming connections, performs networking etc.)
    void start();
private:
	//TcpConnector's event method. called whenever a message has been received 
    void tcpMessageReceivedEvent(std::string ipAddress,int connectionIdentifier,char* message,size_t messageSize);
	//TcpConnector's event method. called whenever the connection with the given socket has been terminated
    void connectionTerminatedEvent(int socket);
	//initializes the lobby components
    void initialize();
	//generates a number between the given intervals
    int generateNumber(int min,int max);
	//accepts an incoming connection. given information is used for creating a client object
    void acceptConnection(std::string ipAddress,int socket,std::string playerName,unsigned char color);
	//sends current game rooms to the given client over TCP
    void sendGameRooms(Client* client);
	//creates a game room with the given information. A client object is passed to indicate the room owner
    int createGameRoom(Client* client,std::string roomName,unsigned char gameType,std::string mapName,int capacity);
	//changes the nickname of the client to the given name
    void changeClientName(Client* client,std::string name);
	//advertises the game room to the other clients. if there is a client that does not require
	//to receive this message, a Client object can be passed as the last parameter
    void broadcastGameRoom(GameRoom* room,Client* clientToIgnore=NULL);
	//sends a message to the all clients on the server
    void broadcastMessage(void* message,size_t size,Client* clientToIgnore=NULL);
	//advertises the client name change to the other clients
    void broadcastNewClientName(Client* client);
	//sends a basic (1 byte) message to the given client
    void sendBasicMessage(int messageType,Client* client);
	//handles game room join request that was sent by the client
    void handleGameRoomJoinRequest(Client* client,GameRoom* room);
	//event method called by the GameRoom. used whenever the state of the room has been changed
    void roomStateChanged(int roomID);
	//notifies other players about the state change of the given GameRoom
    void notifyRoomStateChange(GameRoom* room);
	//removes the given room from the lobby/server
    void removeRoom(GameRoom* room);

    std::string lobbyTcpPort;
    int roomCount;
    std::map<int,GameRoom*> gameRoomMap;
    std::map<int,Client*> clientMap;
    TcpConnector* tcpConnector;
    Logger* logger;
    int minTcpPort;
    int maxTcpPort;
    int minUdpPort;
    int maxUdpPort;
};

#endif
