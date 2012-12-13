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

#ifndef GAMEMESSAGECONTROLLER_H
#define GAMEMESSAGECONTROLLER_H

#include "NetworkUtil/MessageStructure.h"
#include "NetworkUtil/UdpClient.h"
#include "NetworkUtil/UdpListener.h"
#include "NetworkUtil/TcpConnector.h"
#include "NetworkUtil/FileServer.h"
#include "NetworkUtil/FileReceiverCallbackInterface.h"
#include "NetworkUtil/FileReceiver.h"
#include "Util/Converter.h"
#include "LocalServerDiscoverer.h"
#include "GameMessageInterface.h"
#include "GameRoom.h"
#include "stdafx.h"
#include "GameClient.h"
#include "ItemStructure.h"
#include <string.h>

/* GameMessageController
   
   This class is responsible for enable messaging
   betweeen the server and the client applications
   using TCP and UDP communication types. It utilizes
   from the callback mechanism of the related networking 
   classes. LocalServerDiscoveryClient is also utilized.
   In addition, a file server and file receiver system
   (TTFTP) is also used in the class.

   GameMessageController is the core of the networking in the
   client application. This class is based on the publish-subscribe 
   design pattern. Therefore, GameMessageInterface must be 
   implemented by the subscriber class.



*/

class GameMessageController:public TcpCallbackInterface, public UdpCallbackInterface, public LocalServerDiscovererInterface,public FileServerCallbackInterface,public FileReceiverCallbackInterface
{
public:
		//constructor of the class. it takes GameMessageInterface object for enabling the callback system
		//tcp port for the server and the TTFTP client port that is going to be established
		GameMessageController(GameMessageInterface* core,int tcp,int TTFTPClientPort);
		//custom callback method for the TcpConnector class. parameters are the same. used for lobby/server comm.
		void serverTcpMessageReceivedEvent(std::string ipAddress,int socket,char* message,std::size_t size);
		//custom callback method for the TcpConnector class. parameters are the same. used for game room comm.
		void gameTcpMessageReceivedEvent(std::string ipAddress,int socket,char* message,std::size_t size);
		//callback method of the UdpListener class. this method handles transform updates and bullet messages
		void udpMessageReceivedEvent(std::string ipAddress,char* message,std::size_t size);
		//terminates the server connection (if established)
		void serverConnectionTerminatedEvent(int socket);
		//terminates the game room connection (if established)
		void gameConnectionTerminatedEvent(int socket);
		//sends a transform update over UDP. it takes the position, orientation and animation state of the player
		void sendTransformUpdate(Ogre::Vector3 pos, Ogre::Quaternion ori,unsigned char animationState);
		//sends bullet information. it takes bullet's position, angles and the weapon type as parameters
		void sendBulletInfo(Ogre::Vector3 pos, float x, float y,int weaponType);
		//callback method of the LocalServerDiscoverer. called when the discovery has been resulted
        void discoveryFinalizedEvent(std::map<std::string,std::string> serverList);
		//starts LocalServerDiscoverer by the given parameter as the timeout value
        void startLocalServerDiscoverer(int timeoutMilliseconds=-1);
		//connects to the server/lobby
        void connectToServer();
		//sets the server IPv4 address
        void setServerIP(std::string ip);
		//sets ttftp client port that is going to be used for establishing file server
        void setClientTTFTP(int port);
		//sets server's TCP port
        void setServerTcp(int port);
		//enables UDP listening operations. called when the game is starting
        void enableUdpListener();
		//verifies whether the given UDP message type belongs to the TTNP or not
        bool isAuthentic(std::string ip,char* message);
		//handles a map file request that comes from the server application. it starts
		//the file server application
        void handleMapFileRequest();
		//starts file server by sharing the map file
        void startFileServer();
		//sets file server's working path
        void setFilePath(std::string path);
		//starts file receiver. used for handling file transfer between the server
        void startFileReceiver();
		//shares the given file. it adds the file to the file server
        void shareFile(std::string fileName);
		//downloads the map file from the server. must be called after the startFileReceiver()'s invocation
        void downloadMapFile();
		//FileReceiver's callback method
        void fileFound();
		//FileReceiver's callback method
        void fileNotFound();
		//FileReceiver's callback method
        void fileTransferStarted();
		//FileReceiver's callback method
        void fileTransferCompleted();
		//FileReceiver's callback method
        void fileServerConnectionEstablished();
		//requests game room from the server
        void requestGameRooms();
		//requests game room clients from the server
        void requestGameRoomClients();
		//requests the room's owner from the server
        void requestGameRoomOwner();
		//requests to kick the player with the given id from the server
		void requestPlayerKick(unsigned int playerID);
		//requests a game start from the server
        void requestGameStart();
		//request to take an item with the given id from the server
        void requestItemTake(int itemID);
		//request to used a teleport point with the given id from the server
        void requestTeleporterUse(int teleporterID);
		//requests next spawnpoint for the local player
        void requestNextSpawnPoint();
		//sends team change request message to the server
		void sendTeamChangeRequest(unsigned char team);
		//creates a game room with the given attributes
        void createGameRoom(std::string name,unsigned char gameType,std::string mapName,unsigned char gameCapacity);
        //requests to join the game room with the given id
		void joinGameRoom(unsigned int id);
		//leaves the joined game room. terminates gametcp 
        void leaveGameRoom();
		//leaves from the server and terminates the serverTcp
        void leaveServer();
		//notifies the game room that the local player is ready to start
        void sendReady();
		//notifies the game room that the local player is not ready to start
        void sendNotReady();
		//sends a chat message to the server. true should be send as the second
		//parameter if it is a teamChat message
        void sendChatMessage(std::string message,bool teamChat=false);
		//requests a nickname change from the server
        void changeNickName(std::string newNickname);
		//notifies the server that the local player has changed his/her color
		void changeColor(unsigned char color);
		//notifies the server that the localplayer is dead. killer's attributes
		//death position and weapon type.
        void notifyDeath(unsigned int killerID,int weaponID,float posX,float posY,float posZ);
		//notifies the server that the local player has changed his/her weapon
        void notifyWeaponChange(int weaponType);
		//notifies the server that the local player has used a special item with the given ItemType
        void notifySpecialItemUse(ItemType::ItemTypes itemType);
        //notifies the server that the local player has joined and loaded the game
		void notifyGameJoin();
		//FileServer callback method
        void fileUploadStarted(std::string fileName);
		//FileServer callback method
        void fileUploadFinished(std::string fileName);
		//FileServer callback method
        void fileUploadProgressChanged(std::string fileName,int percentage, float kbSpeed);
        //returns the GameRoom object of the joined game room
        GameRoom* getCurrentRoom();
		//returns the id of the local client
        int getClientID();
		//returns true if the server connection has been established
        bool isConnected();
		//returns the GameClient object of the local client
        GameClient* getLocalClient();
		//sets nickname of the player. this method does not notifies the server and must be
		//used before a server connection is going to be established
        void setNickname(std::string nickName);
		//sets the color of the local player. this method does not notifies the server and must be
		//used before a server connection is going to be established
		void setColor(unsigned char color);

private:
		//sends a basic (1 byte) message over the given TcpConnector object
        void tcpSendBasicMessage(unsigned char messageType,TcpConnector* destination);
		//connects to the game room. returns true if in the event of success
        bool connectGameRoom();
		//terminates the game room connection
        void killRoomConnection();
		//terminates the server connecetion
        void killServerConnection();

        GameRoom* currentRoom;
        std::map<int,GameRoom*> gameRoomMap;
		GameMessageInterface* callbackObject;
        LocalServerDiscoverer* localServerDiscoverer;
        std::string serverIP;
        std::string serverName;
		int serverTcpPort;
        int TTFTPClientPort;
        int TTFTPServerPort;
		UdpClient* udpClient;
		UdpListener* udpListener;
        TcpConnector* serverTcp;
        TcpConnector* gameTcp;
        FileServer* fileServer;
        FileReceiver* fileReceiver;
        int localServerDiscoveryTimeout;
        int clientID;
        bool connected;
        bool gameStarted;
        GameClient* client;
        std::string nickName;
        bool connectedToServer;
        bool connectedToRoom;
        bool downloadingMapFile;
        std::string filePath;
		unsigned char color;
};




#endif