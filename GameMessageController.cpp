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

#include "GameMessageController.h"
#include "Util/ConfigReader.h"
#include "NetworkUtil/FileReceiver.h"
#include "Util/Logger.h"

GameMessageController::GameMessageController(GameMessageInterface* core, int tcp,int TTFTPClientPort)
{
		this->callbackObject=core;
		this->serverTcpPort=tcp;
        nickName="UNNAMED!";
        localServerDiscoverer=NULL;
        serverTcp=NULL;
        gameTcp=NULL;
        connected=gameStarted=false;
        currentRoom=NULL;
        connectedToServer=connectedToRoom=false;
        this->TTFTPClientPort=TTFTPClientPort;
        this->fileServer=NULL;
        this->filePath=".";
        this->fileReceiver=NULL;
        downloadingMapFile=false;
		udpClient=new UdpClient();
		udpListener=NULL;
		this->color=0;

        if(ConfigReader::getConfigFile("GameSettings")!=NULL)
        {
            ConfigReader* reader=ConfigReader::getConfigFile("GameSettings");
            std::string discoveryPort=reader->getFieldValueAsString("LocalServerDiscoverer","discovery-port");
            this->localServerDiscoveryTimeout=reader->getFieldValueAsInteger("LocalServerDiscoverer","discovery-timeout");
            std::string headerName=reader->getFieldValueAsString("LocalServerDiscoverer","header-name");
            int headerID=reader->getFieldValueAsInteger("LocalServerDiscoverer","header-id");
            if(discoveryPort!="NULL" && headerName!="NULL" && headerID!=-1)
                localServerDiscoverer=new LocalServerDiscoverer(discoveryPort,headerName,headerID,this);
            else
                Logger::getSingleton()->addLine("GameMessageController LSD cannot be initialized. Check init values");
        }
		else
		{
			Logger::getSingleton()->addLine("GMC: GameSettings not found!",true);
			exit(-1);
		}
        Logger::getSingleton()->addLine("GameMessageController initialized");
}

void GameMessageController::serverTcpMessageReceivedEvent(std::string ipAddress,int socket,char* message,std::size_t size)
{
    Logger::getSingleton()->addLine("GMC tcp message received from the server, size "+Converter::intToString(size));
    BasicMessage_t* bMessage=(BasicMessage_t*)message;
    unsigned char messageType=bMessage->messageType;
    int remainingSize=size;
    int currentSize=0;
    char* _message=message;
    //while(remainingSize>0)
    {
        messageType=((BasicMessage_t*)_message)->messageType;
        switch(messageType)
        {
            case MessageType::CONNECTION_ACCEPT:
            {
				Logger::getSingleton()->addLine("GMC: server connection accept messag received");
                ConnectionAcceptMessage_t* acceptMessage=(ConnectionAcceptMessage_t*)_message;
                this->clientID=acceptMessage->uniqueId;
                connectedToServer=true;
                connected=true;
                client=new GameClient(acceptMessage->uniqueId,nickName,-1);
				client->setColor(color);
                callbackObject->connectionAccepted();
                currentSize=sizeof(ConnectionAcceptMessage_t);
                break;
            }
            case MessageType::ROOM_INFO:
            {
                Logger::getSingleton()->addLine("GMC: server room info message received");
                GameRoomInformationMessage_t* infoMessage=(GameRoomInformationMessage_t*)_message;
                GameRoom* gameRoom=new GameRoom(infoMessage->id,infoMessage->roomName,infoMessage->playerCount,infoMessage->maxPlayerCount,infoMessage->mapName,infoMessage->gameType);
                gameRoomMap[infoMessage->id]=gameRoom;
                callbackObject->gameRoomInfoReceived(gameRoom);
                currentSize=sizeof(GameRoomInformationMessage_t);
                break;
            }
            case MessageType::CLIENT_CREATE_ROOM_ACCEPT:
            {
				Logger::getSingleton()->addLine("GMC: server create room accept message received");
                GameRoomCreateAcceptMessage_t* acceptMessage=(GameRoomCreateAcceptMessage_t*)_message;
                GameRoom* gameRoom = new GameRoom(acceptMessage->roomID, acceptMessage->roomName, 1, acceptMessage->capacity, acceptMessage->mapName,acceptMessage->gameType);
                currentRoom = gameRoom;
                currentRoom->setTcpPort(acceptMessage->tcpPort);
                currentRoom->setUdpServerPort(acceptMessage->udpServerPort);
                currentRoom->setUdpClientPort(acceptMessage->udpClientPort);
                currentRoom->setTTFTPPort(acceptMessage->TTFTPServerPort);
                std::cout << "room tcp: " << currentRoom->getTcpPort() << std::endl;
                std::cout << "room ttftp tcp: " << currentRoom->getTTFTPPort() << std::endl;
                std::cout << "room server udp: " << currentRoom->getUdpServerPort() << std::endl;
                std::cout << "room client udp: " << currentRoom->getUdpClientPort() << std::endl;
                //if(connectGameRoom())
                //    callbackObject->gameRoomCreated();
                connectGameRoom();

                currentSize=sizeof(GameRoomCreateAcceptMessage_t);
                break;
            }
            case MessageType::CLIENT_ROOM_JOIN_ACCEPT:
            {
				Logger::getSingleton()->addLine("GMC: server room join req. accept message received");
                GameRoomJoinAcceptMessage_t* acceptMessage=(GameRoomJoinAcceptMessage_t*)_message;
                currentRoom=gameRoomMap[acceptMessage->roomID];
                currentRoom->setTcpPort(acceptMessage->tcpPort);
                currentRoom->setTTFTPPort(acceptMessage->ttftpPort);
                currentRoom->setUdpServerPort(acceptMessage->udpServerPort);
                currentRoom->setUdpClientPort(acceptMessage->udpClientPort);
                std::cout<<"room tcp: "<<currentRoom->getTcpPort()<<std::endl;
                std::cout<<"room server udp: "<<currentRoom->getUdpServerPort()<<std::endl;
                std::cout<<"room client udp: "<<currentRoom->getUdpClientPort()<<std::endl;
                connectGameRoom();
                currentSize=sizeof(GameRoomJoinAcceptMessage_t);
                break;
            }
            case MessageType::CLIENT_ROOM_JOIN_DENY:
            {
				callbackObject->gameRoomConnectionDenied();
                break;
            }
            case MessageType::ROOM_STATE_CHAGED:
            {
				Logger::getSingleton()->addLine("GMC: server room state change message received");
                GameRoomStateChangedMessage_t* stateMessage=(GameRoomStateChangedMessage_t*)_message;
                if(gameRoomMap.count(stateMessage->id)>0)
                {
                    GameRoom* room=gameRoomMap[stateMessage->id];
                    room->setMapName(stateMessage->mapName);
                    room->setPlayerCount(stateMessage->playerCount);
                    room->setGameType(stateMessage->gameType);
                    callbackObject->gameRoomStateChanged(room);
                }
                else
                    requestGameRooms();
                currentSize=sizeof(GameRoomStateChangedMessage_t);
                break;
            }
            case MessageType::ROOM_REMOVED:
            {
				Logger::getSingleton()->addLine("GMC: Room remove message received");
                GameRoomRemovedMessage_t* removeMessage=(GameRoomRemovedMessage_t*)_message;
                callbackObject->gameRoomRemoved(removeMessage->roomID);
                GameRoom* room=gameRoomMap[removeMessage->roomID];
                //gameRoomMap.erase(room->getRoomID());
                //delete room;
                currentSize=sizeof(GameRoomRemovedMessage_t);
                break;
            }
            default:
            {
                remainingSize=0;
                Logger::getSingleton()->addLine("GMC Server message type("+Converter::intToString(_message[0])+") is unknown!",true);
            }
        }
        remainingSize-=currentSize;
        _message+=currentSize;
    }
    delete message;
}

void GameMessageController::gameTcpMessageReceivedEvent(std::string ipAddress,int socket,char* message,std::size_t size)
{
    Logger::getSingleton()->addLine("GMC tcp message received from the game room, size "+Converter::intToString(size));
    BasicMessage_t* bMessage=(BasicMessage_t*)message;
    unsigned char messageType=bMessage->messageType;
    int remainingSize=size;
    int currentSize=0;
    char* _message=message;
    //while(remainingSize>0)
    {
        messageType=((BasicMessage_t*)_message)->messageType;
        switch(messageType)
        {
            case MessageType::GAME_ROOM_CONNECTION_ACCEPT:
            {
				Logger::getSingleton()->addLine("GMC: game room connection accept message received");
                currentSize=sizeof(GameRoomConnectionAcceptMessage_t);
                connectedToRoom=true;
                currentRoom->addClient(client);
				client->setTeam(Teams::BLUE);
                callbackObject->gameRoomConnectionEstablished();
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_JOINED:
            {
				Logger::getSingleton()->addLine("GMC: game room client joined message received");
                GameRoomClientJoinedMessage_t* joinMessage=(GameRoomClientJoinedMessage_t*)_message;
                GameClient* gameClient=new GameClient(joinMessage->uniqueId,joinMessage->clientName,0);
				gameClient->setTeam(joinMessage->team);
				gameClient->setKillCount(joinMessage->kill);
				gameClient->setDeathCount(joinMessage->death);
                currentRoom->addClient(gameClient);
                callbackObject->gameRoomClientJoined(gameClient);
                currentSize=sizeof(GameRoomClientJoinedMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_LEFT:
            {
				Logger::getSingleton()->addLine("GMC: game room client left message received");
                GameRoomClientLeftMessage_t* leftMessage=(GameRoomClientLeftMessage_t*)_message;
                GameClient* gameClient=currentRoom->getClient(leftMessage->uniqueId);
                callbackObject->gameRoomClientLeft(gameClient);
                callbackObject->gameRoomOwnerChanged(currentRoom->getClient(leftMessage->newOwner));
                currentRoom->removeClient(leftMessage->uniqueId);
                delete gameClient;
                currentSize=sizeof(GameRoomClientLeftMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_READY:
            case MessageType::GAME_ROOM_CLIENT_NOT_READY:
            {
				Logger::getSingleton()->addLine("GMC: game room client ready message received");
                GameRoomClientReadyStatusMessage_t* statusMessage=(GameRoomClientReadyStatusMessage_t*)_message;
                GameClient* client=currentRoom->getClient(statusMessage->clientID);
                client->setReady((messageType==MessageType::GAME_ROOM_CLIENT_READY?true:false));
                callbackObject->gameRoomClientReadyStatusChanged(client);
                currentSize=sizeof(GameRoomClientReadyStatusMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_CHAT:
            case MessageType::GAME_ROOM_CLIENT_TEAM_CHAT:
            {
				Logger::getSingleton()->addLine("GMC: game room client chat message received");
                GameRoomChatMessage_t* chatMessage=(GameRoomChatMessage_t*)_message;
                bool teamChat=(chatMessage->messageType==MessageType::GAME_ROOM_CLIENT_CHAT?false:true);
                callbackObject->gameRoomClientMessageReceived(currentRoom->getClient(chatMessage->ownerID),std::string(chatMessage->message),teamChat);

                currentSize=sizeof(GameRoomChatMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_NICK_CHANGE:
            {
				Logger::getSingleton()->addLine("GMC: game room client nick change message received");
                GameRoomNickChangeMessage_t* nickMsg=(GameRoomNickChangeMessage_t*)_message;
                std::string oldNick=currentRoom->getClient(nickMsg->id)->getName();
                currentRoom->getClient(nickMsg->id)->setName(nickMsg->name);
                callbackObject->gameRoomClientNickChanged(currentRoom->getClient(nickMsg->id),oldNick);
                currentSize=sizeof(GameRoomNickChangeMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_INFO:
            {
				Logger::getSingleton()->addLine("GMC: game room client info message received");
                GameRoomClientInfoMessage_t* info=(GameRoomClientInfoMessage_t*)_message;
                GameClient* client=new GameClient(info->uniqueId,info->clientName,info->team);
				client->setKillCount(info->kill);
				client->setDeathCount(info->death);
				client->setTeam(info->team);
				client->setColor(info->color);
                currentRoom->addClient(client);
                callbackObject->gameRoomClientJoined(client);
                currentSize=sizeof(GameRoomClientInfoMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_OWNER:
            {
				Logger::getSingleton()->addLine("GMC: game room owner changed message received");
                GameRoomOwnerMessage_t* own=(GameRoomOwnerMessage_t*)_message;
                currentRoom->setOwner(own->uniqueId);
                if(currentRoom->getClient(own->uniqueId)!=NULL)
                    callbackObject->gameRoomOwnerChanged(currentRoom->getClient(own->uniqueId));
                else
                    std::cout<<"owner does not exist!\n";
                currentSize=sizeof(GameRoomOwnerMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_MAP_FILE_START_DOWNLOAD:
            {
                Logger::getSingleton()->addLine("GMC: start downloading map file message received");
                downloadMapFile();
                break;
            }
            case MessageType::GAME_ROOM_CLIENT_KICKED:
            {
                GameRoomClientKickedMessage_t* kicked=(GameRoomClientKickedMessage_t*)_message;
                if(kicked->uniqueId==getLocalClient()->getID())
                {
                    //kicked by the room
                    Logger::getSingleton()->addLine("Room: you have been kicked by the room owner");
                    callbackObject->gameRoomLocalClientKicked();
                    killRoomConnection();
                }
                else
                {
                    Logger::getSingleton()->addLine("Room: client "+currentRoom->getClient(kicked->uniqueId)->getName()+" has been kicked by the room owner");
					callbackObject->gameRoomClientKicked(currentRoom->getClient(kicked->uniqueId));
                }
                break;
            }
			case MessageType::GAME_ROOM_CLIENT_TEAM_CHANGED:
			{
                Logger::getSingleton()->addLine("GMC: team change message received");
				GameRoomTeamChangedMessage_t* tc=(GameRoomTeamChangedMessage_t*)_message;
				GameClient* client=currentRoom->getClient(tc->playerID);
				client->setTeam(tc->team);
				callbackObject->playerTeamChanged(client);
				break;
			}
			case MessageType::GAME_ROOM_CLIENT_COLOR_CHANGED:
			{
				Logger::getSingleton()->addLine("GMC: client color changed message received");
				GameRoomColorChangedMessage_t* colorChange=(GameRoomColorChangedMessage_t*)_message;
				GameClient* client=currentRoom->getClient(colorChange->playerId);
				client->setColor(colorChange->color);
				callbackObject->playerColorChanged(client);
				break;
			}
            case MessageType::GAME_START:
            {
                Logger::getSingleton()->addLine("GMC: Game start message received");
                gameStarted=true;
				Logger::getSingleton()->addLine("GMC: enabling UDP listener");
                enableUdpListener();
				Logger::getSingleton()->addLine("GMC: ---ok");
                callbackObject->gameStarting();
                currentSize=sizeof(GameStartMessage_t);
                break;
            }
            case MessageType::GAME_ROOM_MAP_FILE_REQUEST:
            {
				Logger::getSingleton()->addLine("GMC: game room map file received");
                handleMapFileRequest();
                break;
            }
            case MessageType::INGAME_ITEM_TAKE:
            {
				Logger::getSingleton()->addLine("GMC: game room item take message received");
                ItemTakeMessage_t* take=(ItemTakeMessage_t*)_message;
                callbackObject->itemTakeEvent(take->itemID);
                currentSize=sizeof(ItemTakeMessage_t);
                break;
            }
            case MessageType::INGAME_ITEM_NOT_ALIVE:
            case MessageType::INGAME_ITEM_ALIVE:
            {
                Logger::getSingleton()->addLine("GMC: item statuschange message received");
                ItemNotAliveMessage_t* status=(ItemNotAliveMessage_t*)_message;
                callbackObject->itemStatusChanged(status->itemID,status->messageType==MessageType::INGAME_ITEM_ALIVE?true:false);
                currentSize=sizeof(ItemNotAliveMessage_t);
                break;
            }
            case MessageType::INGAME_PLAYER_KILL_INFO:
            {
                Logger::getSingleton()->addLine("GMC: player kill info received");
                PlayerKillInfoMessage_t* info=(PlayerKillInfoMessage_t*)_message;
                callbackObject->playerKilledEvent(info->player,info->killer);
                currentSize=sizeof(PlayerKillInfoMessage_t);
                break;
            }
            case MessageType::INGAME_SPAWN_POINT:
            {
                Logger::getSingleton()->addLine("GMC: next spawn point received");
                SpawnPointMessage_t* spawnPoint=(SpawnPointMessage_t*)_message;
                callbackObject->nextSpawnPointEvent(spawnPoint->x,spawnPoint->y,spawnPoint->z);
                currentSize=sizeof(SpawnPointMessage_t);
                break;
            }
            case MessageType::INGAME_PLAYER_RESURRECTED:
            {
                Logger::getSingleton()->addLine("GMC: player resurrection message received");
                PlayerResurrected_t* res=(PlayerResurrected_t*)_message;
                callbackObject->playerResurrectedEvent(res->playerID);
                currentSize=sizeof(PlayerResurrected_t);
                break;
            }
            case MessageType::INGAME_WEAPON_CHANGE:
            {
                Logger::getSingleton()->addLine("GMC: weapon change message received");
                WeaponChangeMessage_t* info=(WeaponChangeMessage_t*)_message;
                callbackObject->playerWeaponSwitchEvent(info->playerID,info->weaponType);
                currentSize=sizeof(WeaponChangeMessage_t);
                break;
            }
            case MessageType::INGAME_RESET_POSITION:
            {
                Logger::getSingleton()->addLine("GMC: set position message received");
                callbackObject->resetPositionEvent();
                break;
            }
            case MessageType::INGAME_PLAYER_ITEM_EXPIRED:
            {
				Logger::getSingleton()->addLine("GMC: game room player item expire message received");
                ItemExpiredMessage_t* exp=(ItemExpiredMessage_t*)_message;
                callbackObject->itemExpiredEvent(exp->itemType);
                break;
            }
            case MessageType::INGAME_TELEPORTER_USE_ACCEPT:
            {
				Logger::getSingleton()->addLine("GMC: teleporter usage accept message received");
                TeleporterUseAcceptMessage_t* accept=(TeleporterUseAcceptMessage_t*)_message;
                callbackObject->teleporterUsageAccepted(accept->itemID);
                break;
            }
            case MessageType::INGAME_TELEPORTER_ACTIVATED:
            {
				Logger::getSingleton()->addLine("GMC: teleporter activate message received");
                TeleporterActivatedMessage_t* activated=(TeleporterActivatedMessage_t*)_message;
                callbackObject->teleporterActivated(activated->itemID);
                break;
            }
            case MessageType::INGAME_TELEPORTER_DEACTIVATED:
            {
				Logger::getSingleton()->addLine("GMC: teleporter deactivate message received");
                TeleporterDeactivatedMessage_t* deactivated=(TeleporterDeactivatedMessage_t*)_message;
                callbackObject->teleporterDeactivated(deactivated->itemID);
                break;
            }
            case MessageType::INGAME_CREATE_ITEM:
            {
				Logger::getSingleton()->addLine("GMC: create item message received");
                CreateItemMessage_t* createItem = (CreateItemMessage_t*) _message;
                callbackObject->itemCreateEvent(createItem->itemID, createItem->itemType, createItem->x, createItem->y, createItem->z);
                break;
            }
            case MessageType::INGAME_PLAYER_ITEM_USED_NOTIFICATION:
            {
				Logger::getSingleton()->addLine("GMC: player item usage not. message received");
                ItemUsedNotificationMessage_t* itemUsed=(ItemUsedNotificationMessage_t*)_message;
                callbackObject->playerSpecialItemUsedEvent(itemUsed->playerID,itemUsed->itemType);
                break;
            }
            case MessageType::INGAME_PLAYER_ITEM_EXPIRED_NOTIFICATION:
            {
				Logger::getSingleton()->addLine("GMC: player item usage exp. not. message received");
                ItemExpiredNotificationMessage_t* itemExpired=(ItemExpiredNotificationMessage_t*)_message;
                callbackObject->playerSpecialItemExpiredEvent(itemExpired->playerID,itemExpired->itemType);
                break;
            }
            case MessageType::INGAME_PLAYER_SCORE_NOTIFICATION:
            {
				Logger::getSingleton()->addLine("GMC: player score message received");
                PlayerScoreNotificationMessage_t* score=(PlayerScoreNotificationMessage_t*)_message;
				GameClient* _client=NULL;
                if(client->getID()==score->playerID)
                    _client=client;
                else
                    _client=currentRoom->getClient(score->playerID);
                _client->setKillCount(score->killCount);
                _client->setDeathCount(score->deathCount);
                callbackObject->playerScoreUpdated(_client);
                break;
            }
            default:
            {
                remainingSize=0;
                Logger::getSingleton()->addLine("GMC Game message type("+Converter::intToString(_message[0])+") is unknown!",true);
            }
        }
        remainingSize-=currentSize;
        _message+=currentSize;
    }
    delete message;
}

void GameMessageController::fileServerConnectionEstablished()
{
    if(downloadingMapFile)
    {
        Logger::getSingleton()->addLine("GMC: connection established with the file server. requesting file");
        fileReceiver->requestFile(currentRoom->getMapName());
    }
}


void GameMessageController::fileFound()
{
    if(downloadingMapFile)
    {
        Logger::getSingleton()->addLine("GMC: map file is found on the file server. starting to download");
        fileReceiver->startReceiving(currentRoom->getMapName(),filePath);
    }
}

void GameMessageController::fileNotFound()
{
    if(downloadingMapFile)
        Logger::getSingleton()->addLine("GMC: map file is NOT found on the file server. starting to download",true);
}

void GameMessageController::fileTransferStarted()
{

}

void GameMessageController::sendTeamChangeRequest(unsigned char team)
{
	if(connectedToRoom)
	{
		GameRoomTeamChangeRequestMessage_t request={MessageType::GAME_ROOM_CLIENT_TEAM_CHANGE_REQUEST,team};
		gameTcp->sendData(&request,sizeof(GameRoomTeamChangeRequestMessage_t));
	}
}

void GameMessageController::fileTransferCompleted()
{
    if(downloadingMapFile)
    {
        Logger::getSingleton()->addLine("GMC: map file has been downloaded");
        downloadingMapFile=false;
        GameRoomMapFileDownloadCompleteMessage_t message={MessageType::GAME_ROOM_MAP_FILE_DOWNLOAD_COMPLETED};
        gameTcp->sendData(&message,sizeof(GameRoomMapFileDownloadCompleteMessage_t));
    }
}

void GameMessageController::downloadMapFile()
{
    if(!downloadingMapFile)
    {
        downloadingMapFile=true;
        startFileReceiver();
    }
}

void GameMessageController::startFileReceiver()
{
    if(fileReceiver!=NULL)
    {
        fileReceiver->stop();
        delete fileReceiver;
    }
    fileReceiver=new FileReceiver(serverIP,Converter::intToString(currentRoom->getTTFTPPort()),this);
    fileReceiver->connect();
}

void GameMessageController::startFileServer()
{
    if(fileServer!=NULL)
    {
        fileServer->stop();
        delete fileServer;
    }

    fileServer=new FileServer(filePath,Converter::intToString(TTFTPClientPort),this);
    fileServer->startServer();
}

void GameMessageController::shareFile(std::string fileName)
{
    if(fileServer!=NULL)
        fileServer->addFile(fileName);
}

void GameMessageController::handleMapFileRequest()
{
    //if(fileServer!=NULL)
        //startFileServer();
    GameRoomMapFileRequestAcceptMessage_t message={MessageType::GAME_ROOM_MAP_FILE_REQUEST_ACCEPT};
    gameTcp->sendData(&message,sizeof(GameRoomMapFileRequestAcceptMessage_t));
}


void GameMessageController::requestNextSpawnPoint()
{
    Logger::getSingleton()->addLine("GMC: Sending next spawn point request");
    SpawnPointRequest_t req={MessageType::INGAME_SPAWN_POINT_REQUEST};
    gameTcp->sendData(&req,sizeof(SpawnPointRequest_t));
}

void GameMessageController::enableUdpListener()
{
    if (udpListener != NULL)
    {
        udpListener->stop();
        delete udpListener;
    }
    udpListener = new UdpListener(Converter::intToString(currentRoom->getUdpClientPort()));
    udpListener->enableCallback(this);
    udpListener->startListening();
}

bool GameMessageController::isAuthentic(std::string ip,char* message)
{
    UdpHeader_t* header=(UdpHeader_t*)message;
    if(serverIP==ip && header->protocolID==TTNP_ID)
        return true;
    else
        return false;
}

void GameMessageController::udpMessageReceivedEvent(std::string ipAddress,char* message,std::size_t size)
{
    UdpHeader_t* header;
    if(size>sizeof(UdpHeader_t))
    {
        int remainingSize=size;
        int currentSize;
        char* _message=message;
        while(remainingSize>0)
        {
            if(isAuthentic(ipAddress,_message))
            {
                header=(UdpHeader_t*)_message;
                switch (header->messageType)
                {
                    case MessageType::POSITION_UPDATE:
                    {
                        callbackObject->scheduleTransformUpdate((PositionUpdateMessage*) message);
                        currentSize=sizeof(PositionUpdateMessage_t);
                        break;
                    }
                    case MessageType::BULLET:
                    {
                        callbackObject->addBullet((BulletInfo_t*) message);
                        currentSize=sizeof(BulletInfo_t);
                        break;
                    }
                }
                _message+=currentSize;
                remainingSize-=currentSize;
            }
			else
			{
				Logger::getSingleton()->addLine("GMC: Unknown UDP message",true);
				break;
			}
        }
        
    }
}

void GameMessageController::notifyDeath(unsigned int killerID,int weaponID,float posX,float posY,float posZ)
{
    PlayerKilledMessage_t message={MessageType::INGAME_PLAYER_KILLED,killerID,weaponID,posX,posY,posZ};
    gameTcp->sendData(&message,sizeof(PlayerKilledMessage_t));
}

void GameMessageController::notifyGameJoin()
{
    PlayerJoinedMessage_t message={MessageType::INGAME_PLAYER_JOINED};
    gameTcp->sendData(&message,sizeof(PlayerJoinedMessage_t));
}

void GameMessageController::tcpSendBasicMessage(unsigned char messageType, TcpConnector* destination)
{
    BasicMessage_t message={messageType};
    destination->sendData(&message,sizeof(BasicMessage_t));
}

void GameMessageController::serverConnectionTerminatedEvent(int socket)
{
    Logger::getSingleton()->addLine("GMC connection terminated with the server");
    connected=false;
}

void GameMessageController::gameConnectionTerminatedEvent(int socket)
{
    Logger::getSingleton()->addLine("GMC connection terminated with the game room");
	callbackObject->gameRoomConnectionTerminated();
    connected=false;
}

void GameMessageController::sendTransformUpdate(Ogre::Vector3 pos, Ogre::Quaternion orientation,unsigned char animationState)
{
    if(gameStarted)
    {
        PositionUpdateMessage_t posUpdate;
        posUpdate.header.protocolID=TTNP_ID;
        posUpdate.header.messageType = MessageType::POSITION_UPDATE;
        posUpdate.header.clientID=getLocalClient()->getID();

        posUpdate.x = pos.x;
        posUpdate.y = pos.y;
        posUpdate.z = pos.z;

        posUpdate.angleY = orientation.getYaw().valueRadians();
        posUpdate.animation = animationState;

        udpClient->sendData(serverIP,Converter::intToString(currentRoom->getUdpServerPort()),(char*)&posUpdate,sizeof(PositionUpdateMessage_t));
    }
}

void GameMessageController::sendBulletInfo(Ogre::Vector3 pos, float x, float y,int weaponType)
{
    if (gameStarted)
    {
        BulletInfo_t info;
        info.header.protocolID = TTNP_ID;
        info.header.messageType = MessageType::BULLET;
        info.header.clientID = getLocalClient()->getID();

        info.x = pos.x;
        info.y = pos.y;
        info.z = pos.z;

        info.angleX = x;
        info.angleY = y;

        info.weapon=weaponType;
        udpClient->sendData(serverIP, Converter::intToString(currentRoom->getUdpServerPort()), (char*) &info, sizeof (BulletInfo_t));
    }
}

void GameMessageController::discoveryFinalizedEvent(std::map<std::string,std::string> serverList)
{
    if(serverList.size()>0)
    {
        this->serverName=serverList.begin()->first;
        this->serverIP=serverList.begin()->second;
        callbackObject->localDiscoveryFinalized(serverList);
    }
}

void GameMessageController::startLocalServerDiscoverer(int timeoutMilliseconds)
{
    if(localServerDiscoverer!=NULL)
        localServerDiscoverer->findServer((timeoutMilliseconds<0?localServerDiscoveryTimeout:timeoutMilliseconds));
}

void GameMessageController::setServerIP(std::string serverIP)
{
    this->serverIP=serverIP;
}

void GameMessageController::connectToServer()
{
    Logger::getSingleton()->addLine("GMC connecting to server");
    Logger::getSingleton()->addLine("GMC server ip "+serverIP);
    Logger::getSingleton()->addLine("GMC server tcp port "+Converter::intToString(serverTcpPort));

    if(connectedToServer)
    {
        killRoomConnection();
        serverTcp->stop();
        delete serverTcp;
    }
    connectedToServer=false;
    serverTcp = new TcpConnector(serverIP, Converter::intToString(serverTcpPort));
    serverTcp->enableCustomCallback(this, (newMessageCallbackMethod) & GameMessageController::serverTcpMessageReceivedEvent,
            (connectionTerminatedCallbackMethod) & GameMessageController::serverConnectionTerminatedEvent);

    bool connectionResult = serverTcp->connectToHost();
    if (connectionResult)
    {
        Logger::getSingleton()->addLine("GMC connection established.");
        Logger::getSingleton()->addLine("GMC sending lobby join request...");
        ConnectionRequestMessage_t message;
        message.messageType = MessageType::CONNECTION_REQUEST;
        strncpy(message.playerName, nickName.c_str(), 30);
		message.color=this->color;
        serverTcp->sendData(&message, sizeof (ConnectionRequestMessage_t));
    }
    else
	{
        Logger::getSingleton()->addLine("GMC connection failed.");
		callbackObject->connectionFailed();
	}
}

bool GameMessageController::connectGameRoom()
{
    killRoomConnection();
    gameTcp=new TcpConnector(serverIP,Converter::intToString(currentRoom->getTcpPort()));
    gameTcp->enableCustomCallback(this,(newMessageCallbackMethod)&GameMessageController::gameTcpMessageReceivedEvent,
            (connectionTerminatedCallbackMethod)&GameMessageController::gameConnectionTerminatedEvent);
    if(gameTcp->connectToHost())
    {
        GameRoomConnectionRequestMessage_t conReq={MessageType::GAME_ROOM_CONNECTION_REQUEST,client->getID()};
        gameTcp->sendData(&conReq,sizeof(GameRoomConnectionRequestMessage_t));
        return true;
    }
    else
        return false;
}

void GameMessageController::killRoomConnection()
{
    if(connectedToRoom)
    {
        tcpSendBasicMessage(MessageType::GAME_ROOM_CLIENT_LEFT_REQUEST,gameTcp);
        if(currentRoom!=NULL)
            currentRoom=NULL;
    }

    if(gameTcp!=NULL)
    {
        gameTcp->stop();
        delete gameTcp;
        gameTcp=NULL;
    }
    
    connectedToRoom=false;
}

void GameMessageController::killServerConnection()
{
    Logger::getSingleton()->addLine("GMC: killing server connection");
    if(connectedToServer)
    {
        if(serverTcp!=NULL)
        {
            serverTcp->stop();
            delete serverTcp;
            serverTcp=NULL;
        }
        connectedToServer=false;
    }
    Logger::getSingleton()->addLine("GMC: server connection killed");
}

void GameMessageController::requestGameRooms()
{
    if(isConnected())
    {
        GameRoomInformationRequestMessage_t message;
        message.messageType=MessageType::CLIENT_GET_ROOMS;
        serverTcp->sendData(&message,sizeof(GameRoomInformationRequestMessage_t));
    }
}

void GameMessageController::requestGameRoomClients()
{
    if(currentRoom!=NULL)
    {
        GameRoomClientGetClientsMessage_t message;
        message.messageType=MessageType::GAME_ROOM_CLIENT_GET_CLIENTS;
        gameTcp->sendData(&message,sizeof(GameRoomClientGetClientsMessage_t));
    }
}

void GameMessageController::requestGameRoomOwner()
{
    if(currentRoom!=NULL)
    {
        GameRoomOwnerRequestMessage_t request={MessageType::GAME_ROOM_CLIENT_OWNER_REQUEST};
        gameTcp->sendData(&request,sizeof(GameRoomOwnerRequestMessage_t));
    }
}

void GameMessageController::createGameRoom(std::string name, unsigned char gameType,std::string mapName,unsigned char gameCapacity)
{
    if(connectedToServer)
    {
        killRoomConnection();
        GameRoomCreateRequestMessage_t message;
        name=name.substr(0,30);
        strncpy(message.name,name.c_str(),30);
        strncpy(message.mapName,mapName.c_str(),30);
        message.messageType=MessageType::CLIENT_CREATE_ROOM;
        message.gameType=gameType;
        message.gameCapacity=gameCapacity;
        serverTcp->sendData(&message,sizeof(GameRoomCreateRequestMessage_t));
    }
}

void GameMessageController::joinGameRoom(unsigned int id)
{
    if(connectedToServer)
    {
        GameRoomJoinRequestMessage_t message;
        message.messageType=MessageType::CLIENT_ROOM_JOIN_REQUEST;
        message.uniqueId=id;
        serverTcp->sendData(&message,sizeof(GameRoomJoinAcceptMessage_t));
    }
}

void GameMessageController::sendReady()
{
    if(currentRoom!=NULL)
    {
        GameRoomClientReadyRequestMessage_t message={MessageType::GAME_ROOM_CLIENT_READY_REQUEST};
        gameTcp->sendData(&message,sizeof(GameRoomClientReadyRequestMessage_t));
    }
}

void GameMessageController::sendNotReady()
{
    if(currentRoom!=NULL)
    {
        GameRoomClientNotReadyRequestMessage_t message={MessageType::GAME_ROOM_CLIENT_NOT_READY_REQUEST};
        gameTcp->sendData(&message,sizeof(GameRoomClientNotReadyRequestMessage_t));
    }
}

void GameMessageController::notifyWeaponChange(int weaponType)
{
    if(currentRoom!=NULL)
    {
        WeaponChangeNotificationMessage_t weaponChange={MessageType::INGAME_WEAPON_CHANGE_NOTIFICATION,weaponType};
        gameTcp->sendData(&weaponChange,sizeof(WeaponChangeNotificationMessage_t));
        Logger::getSingleton()->addLine("GMC: notifying weapon change...");
    }
    else
        Logger::getSingleton()->addLine("GMC: cannot notify weapon change! currentroom is NULL!",true);
}

void GameMessageController::requestGameStart()
{
	if(connectedToRoom)
	{
		if(currentRoom->getOwner()==getLocalClient()->getID())
		{
			Logger::getSingleton()->addLine("GMC: sending game start request...");
			GameStartRequestMessage_t req={MessageType::GAME_START_REQUEST};
			gameTcp->sendData(&req,sizeof(GameStartRequestMessage_t));
		}
		else
			Logger::getSingleton()->addLine("GMC: Game cannot be started! Room owner is "+currentRoom->getClient(currentRoom->getOwner())->getName());
	}
}

void GameMessageController::requestItemTake(int itemID)
{
    if (connectedToRoom)
    {
        ItemTakeRequestMessage_t req = {MessageType::INGAME_ITEM_TAKE_REQUEST, itemID};
        gameTcp->sendData(&req, sizeof (ItemTakeRequestMessage_t));
    }
}

void GameMessageController::requestTeleporterUse(int teleporterID)
{
    if(connectedToRoom)
    {
        TeleporterUseRequestMessage_t req={MessageType::INGAME_TELEPORTER_USE_REQUEST,teleporterID};
        Logger::getSingleton()->addLine("GMC: requesting teleport usage for tid: "+Converter::intToString(teleporterID));
        gameTcp->sendData(&req,sizeof(TeleporterUseRequestMessage_t));
    }
}

void GameMessageController::sendChatMessage(std::string chatMessage, bool teamChat)
{
    if(currentRoom!=NULL)
    {
        if(teamChat)
        {
            GameRoomTeamChatRequestMessage_t _message;
            _message.messageType=MessageType::GAME_ROOM_CLIENT_TEAM_CHAT_REQUEST;
            strncpy(_message.message,chatMessage.c_str(),255);
            gameTcp->sendData(&_message,sizeof(GameRoomTeamChatRequestMessage_t));
        }
        else
        {
            GameRoomChatRequestMessage_t _message;
            _message.messageType=MessageType::GAME_ROOM_CLIENT_CHAT_REQUEST;
            strncpy(_message.message,chatMessage.c_str(),255);
            gameTcp->sendData(&_message,sizeof(GameRoomChatRequestMessage_t));
        }
    }
}

void GameMessageController::changeNickName(std::string newNickname)
{
    if(connectedToRoom)
    {
        GameRoomNickChangeRequestMessage_t message;
        message.messageType=MessageType::GAME_ROOM_CLIENT_NICK_CHANGE_REQUEST;
        strncpy(message.name,newNickname.c_str(),30);
        gameTcp->sendData(&message,sizeof(GameRoomNickChangeRequestMessage_t));
    }
}

void GameMessageController::changeColor(unsigned char color)
{
	if(connectedToRoom)
	{
		GameRoomColorChangeRequestMessage_t message={MessageType::GAME_ROOM_CLIENT_COLOR_CHANGE_REQUEST,color};
		gameTcp->sendData(&message,sizeof(GameRoomColorChangeRequestMessage_t));
	}
}

void GameMessageController::notifySpecialItemUse(ItemType::ItemTypes item)
{
    if(connectedToRoom)
    {
        ItemUsedMessage_t message={MessageType::INGAME_PLAYER_ITEM_USED,item};
        gameTcp->sendData(&message,sizeof(ItemUsedMessage_t));
    }
}

void GameMessageController::requestPlayerKick(unsigned int playerID)
{
    if(connectedToRoom && currentRoom->getOwner()==getLocalClient()->getID())
    {
        GameRoomClientKickRequestMessage_t request={MessageType::GAME_ROOM_CLIENT_KICK_REQUEST,playerID};
        gameTcp->sendData(&request,sizeof(GameRoomClientKickRequestMessage_t));
    }
}

void GameMessageController::fileUploadStarted(std::string fileName)
{
    Logger::getSingleton()->addLine("GMC: file upload started: "+fileName);
}
void GameMessageController::fileUploadFinished(std::string fileName)
{
    Logger::getSingleton()->addLine("GMC: file upload finished: "+fileName);
}
void GameMessageController::fileUploadProgressChanged(std::string fileName, int percentage, float kbSpeed)
{
    Logger::getSingleton()->addLine("GMC: file upload progress changed: "+fileName);
}

void GameMessageController::leaveGameRoom()
{
    killRoomConnection();
}

void GameMessageController::leaveServer()
{
    killServerConnection();
}

bool GameMessageController::isConnected()
{
    return connectedToServer;
}

GameRoom* GameMessageController::getCurrentRoom()
{
    return currentRoom;
}

GameClient* GameMessageController::getLocalClient()
{
    return client;
}

void GameMessageController::setNickname(std::string nickName)
{
    this->nickName=nickName;
}

void GameMessageController::setFilePath(std::string path)
{
    this->filePath=path;
}

void GameMessageController::setClientTTFTP(int port)
{
    TTFTPClientPort=port;
}

void GameMessageController::setServerTcp(int port)
{
    serverTcpPort=port;
}

void GameMessageController::setColor(unsigned char color)
{
	this->color=color;
}