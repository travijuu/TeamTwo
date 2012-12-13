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
#include <map>
#include <UdpClient.h>
#include <UdpListener.h>
#include <TcpConnector.h>
#include <ConfigReader.h>
#include <FileServer.h>
#include <FileReceiver.h>
#include <Logger.h>
#include "Client.h"
#include <TcpCallbackInterface.h>
#include <UdpCallbackInterface.h>
#include <sstream>
#include <MessageStructure.h>
#include <RoomEventInterface.h>
#include <MapReader.h>
#include "Item.h"
#include "ExpirableItem.h"
#include "OperationScheduler.h"
#include "Teleporter.h"
#include "TeleporterCallbackInterface.h"

/*	GameRoom


*/
class GameRoom:TcpCallbackInterface,UdpCallbackInterface,ItemCallbackInterface,ClientCallbackInterface,FileReceiverCallbackInterface,FileServerCallbackInterface,ExpirableItemCallbackInterface,TeleporterCallbackInterface
{
public:
    GameRoom(int id,std::string roomName,int tcpPort,int ttftpPort,int udpServerPort,int udpClientPort,int owner,unsigned char gameType,std::string mapName,int capacity,RoomEventInterface* object);
    //returns the current player count of the room
	int getPlayerCount();
	//returns the UDP port of the game room that is being listening
    int getUdpServerPort();
	//retunrs the client UDP port of game room
    int getUdpClientPort();
	//returns the TCP port of the game room
    int getTcpPort();
	//returns the id of the game room
    int getRoomId();
	//returns the server TTFTP port of the room
    int getTTFTPPort();
	//returns the name of the room
    std::string getRoomName();
	//returns the map name of the room
    std::string getMapName();
	//returns the capacity of the game room
    int getCapacity();
	//returns the game type of the game room
    unsigned char getGameType();
	//returns true if the game has been started
    bool isStarted();
	//makes the game room ready for deletion by terminating networking
    void destroy();
	//TcpConnector's message receive event method. schedules the message into the message queue
    void tcpMessageReceivedEvent(std::string ipAddress,int socket,char* message,size_t messageSize);
	//UdpListener's message receive event method. schedules the message into the message queue
    void udpMessageReceivedEvent(std::string ipAddress,char* message,size_t messageSize);
	//TcpConnector's socket termination callback method
    void connectionTerminatedEvent(int socket);
	//ItemCallbackInterface's event method
    void itemResurrected(int id);
	//ExpirableItemCallbackInterface's event method
    void itemExpiredEvent(unsigned int playerID,int itemType);
	//ClientcallbackInterface's event method
    void clientResurrectedEvent(unsigned int id);
	//notifies the clients of the game that they need to download the map file from the TTFTP FileServer
    void notifyMapFileDownload();
	//TeleporterCallbackInterface's event method
    void teleporterActivatedEvent(int teleporterID);
	//returns true if the given message is encoded in TTNP format.
    bool isTTNP(char* message);
	//loads the given map file's attributes such as teleporters, items etc.
    bool initializeMap(std::string mapName);
	//requests the map file from the owner of the game room
    void requestMapFile();
	//adds the given client to the waiting list
    void addToWaitingList(Client* client);
	//advertises the given client to the other users
    void advertiseClient(Client* client);
	//advertises the new game room owner
    void advertiseNewOwner();
	//sends the given message to the all clients of the current room using TCP. ignores the clientToBeIgnored
    void tcpBroadcastMessage(void* message,int size,Client* clientToBeIgnored=NULL);
	//sends the given message to the all clients of the current room using UDP. ignores the clientToBeIgnored
    void udpBroadcastMessage(void* message,int size,Client* clientToBeIgnored=NULL);
	//sends the given message to the all clients who are on the given team using TCP. ignores the clientToBeIgnored
    void tcpBroadcastTeamMessage(void* message,int size,unsigned char team,Client* clientToBeIgnored=NULL);
	//reassigns the owner of the room in an event of client leave
    void reassignOwner();
	//loads spawn points from the map file
    void loadSpawnPoints();
	//loads items from the map file
    void loadItems();
	//loads teleporters from the map file
    void loadTeleporters();
	//starts the game
    void startGame();
	//sends next spawnpoint to the given client
    void sendNextSpawnPoint(Client* client);
	//sends initial spawn points to the clients who are playing the game
    void sendInitialSpawnPoints();
	//returns true if all players are ready and the game can be started
    bool canStart();
	//starts file receiver
    void startFileReceiver();
	//returns the next spawn point
    Entity2Position_t getNextSpawnPoint();
	//returns true if all clients has the map
    bool isMapSyncCompleted();
	//sends the updated score of the client to the others
    void sendPlayerScore(Client* client);
	//handles incoming connection request
    void handleConnectionRequest(unsigned int clientID,int socket);
	//handles incoming chat message
    void handleChatMessage(Client* client,std::string message,bool teamChat);
	//handles the status change of the client
    void handlePlayerStatusChange(Client* client,bool ready);
	//handles the nickname change request of the given client
    void handleNickChangeRequest(Client* client,std::string newNick);
	//handles the get clients message and sends the client list to the requested one
    void handleGetClients(Client* client);
	//performs necessary operations for terminating client sockets and advertisement
    void handleClientLeave(Client* client);
	//performs necessary operations for terminating client sockets and advertisement
    void handleClientLeave(int socket);
	//handles the game room owner request by sending the game room owner
    void handleOwnerRequest(Client* client);
	//handles the team change request and advertises the current status to the others
	void handleTeamChangeRequest(Client* client,unsigned char newTeam);
	//handles game start request. checks conditions such as map synch and ready status
    void handleGameStartRequest(Client* client);
	//handles the player death event. notifies other users and starts the respawn timer
    void handlePlayerDeath(Client* player,Client* killer,int weaponType,float x, float y, float z);
	//handles the item take request 
    void handleItemTakeRequest(Client* player,int itemID);
	//handles weapon change event of the client and notify the others
    void handleWeaponChange(Client* player,int weaponID);
	//handles map file download completition of the given client. if everybody is ready, starts the game
    void handleMapDownloadComplete(Client* client);
	//performs the necessary operations when the given client has loaded the game
    void handleGameJoin(Client* client);
	//handles the usage of a special item. starts an expiration timer on the given item
    void handleItemUsed(Client* client, int itemType);
	//handles the kick request for the given client
    void handleClientKick(Client* client, unsigned int kickPlayerID);
	//handles the event of teleport reactivation
    void handleTeleporterReactivation(int teleporterID);
	//handles the incoming teleporter usage request 
    void handleTeleporterUseRequest(Client* client,int teleporterdID);
	//handles the reactivation event of the given item
    void handleItemResurrection(int itemID);
	//handles the special item expiration event. notifies the users about it.
    void handleItemExpiration(unsigned int playerID, int itemType);
	//handles client spawn event
    void handleClientResurrection(unsigned int clientID);
	//handles the incoming color change event. notifies the other users.
	void handleColorChange(Client* client,unsigned char color);
	//handles incoming transform update. forwards the message to the other players
    void handleTransformUpdate(Client* player,PositionUpdateMessage_t* transformUpdate);
	//handles fire event.
    void handleFireEvent(Client* client,BulletInfo_t* bulletInfo);

	

	//FileReceiver & File Server events===============================
    void fileFound();
    void fileNotFound();
    void fileTransferStarted();
    void fileTransferCompleted();
    void fileServerConnectionEstablished();
    void fileUploadStarted(std::string fileName){}
    void fileUploadFinished(std::string fileName){}
    void fileUploadProgressChanged(std::string fileName,int percentage,float kbSpeed){}
	//FileReceiver & File Server events===============================

private:
	//initializes the necessary variables and components
    void initialize();
	//starts the message queue loop
    void theOneLoop();
	//handles the incoming game message
    void handleGameMessage(std::string ipAddress,int socket, char* message, size_t messageSize);
	//handles the incoming udp message
	void handleUdpMessage(std::string ipAddress,char* message,size_t messageSize);

    MapReader* mapReader;
    RoomEventInterface* eventObject;
    Logger* logger;
    bool requestingMap;
    bool started;
    TcpConnector* tcpConnector;
    UdpListener* udpListener;
    UdpClient* udpClient;
    FileReceiver* fileReceiver;
    FileServer* fileServer;
    std::string name;
    int id;
    int ownerId;
    int playerCount;
    int udpServerPort;
    int udpClientPort;
    int ttftpPort;
    int tcpPort;
    int capacity;
	int currentItemID;
    std::string mapName;
	std::string filePath;
    unsigned char gameType;
    std::map<int,Client*> clientWaitMap;
    std::map<int,Client*> clientTcpMap;
    std::map<unsigned int,Client*> clientUdpMap;
    std::map<int,Item*> itemMap;
    std::map<int,Teleporter*> teleporterMap;
    std::vector<ExpirableItem*> expirableItemVector;
    std::vector<Entity2Position_t> spawnPointVector;
    std::vector<Entity2Position_t>::iterator spawnPointIterator;
    OperationScheduler* scheduler;
    boost::thread* loopThread;
    bool active;
};

#endif

