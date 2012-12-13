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

#include "GameRoom.h"
#include "Converter.h"
#include "GameLobby.h"

#define TTFTP_CLIENT_PORT 59999

GameRoom::GameRoom(int id,std::string name,int tcpPort,int ttftpPort,int udpServerPort,int udpClientPort,int owner,unsigned char gameType,std::string mapName,int capacity,RoomEventInterface* object)
{
    this->logger=Logger::getSingleton();
    this->tcpPort=tcpPort;
    this->ttftpPort=ttftpPort;
    this->udpServerPort=udpServerPort;
    this->udpClientPort=udpClientPort;
    this->id=id;
    this->name=name;
    this->ownerId=owner;
    this->mapName=mapName;
    this->capacity=capacity;
    this->eventObject=object;
    this->requestingMap=false;
    this->gameType=gameType;
    playerCount=0;
    started=false;
    initialize();
}


void GameRoom::initialize()
{
    filePath = "Room_" + Converter::intToString(getRoomId()) + "_files";
    currentItemID = 0;
    active=true;
    loopThread=new boost::thread(&GameRoom::theOneLoop,this);
    scheduler=new OperationScheduler();

    tcpConnector=new TcpConnector(Converter::intToString(tcpPort));
    tcpConnector->enableCallback(this);
    tcpConnector->startListening();

    udpListener=new UdpListener(Converter::intToString(udpServerPort));
    udpListener->enableCallback(this);
    udpListener->startListening();

    udpClient=new UdpClient();

    fileServer=new FileServer(filePath,Converter::intToString(ttftpPort),this);
    fileServer->startServer();

    fileReceiver = NULL;

}

void GameRoom::theOneLoop()
{
	//while(!loopThread->interruption_requested())
	while(active)
            scheduler->runOnce();
	std::cout<<"FINISHED\n";
}

void GameRoom::handleGameMessage(std::string ipAddress,int socket, char* message, size_t messageSize)
{
    BasicMessage_t* bmsg=(BasicMessage_t*)message;
    char messageType=bmsg->messageType;
    int remainingSize=messageSize;
    int currentSize=0;
    char* _message=message;
    logger->addLine("Room: message("+Converter::intToString((int)messageType)+")  received from a client. size: "+Converter::intToString(messageSize)+" bytes");
    if(clientTcpMap.count(socket)==0 && messageType==MessageType::GAME_ROOM_CONNECTION_REQUEST)
    {
        Logger::getSingleton()->addLine("Room: connection request detected");
        GameRoomConnectionRequestMessage_t* grcm = (GameRoomConnectionRequestMessage_t*) message;
        handleConnectionRequest(grcm->uniqueId, socket);
    }
    else
    {
        Client* client=clientTcpMap[socket];
        //while(remainingSize>0)
        {
            messageType=((BasicMessage_t*)_message)->messageType;
            switch(messageType)
            {
                case MessageType::GAME_ROOM_CLIENT_CHAT_REQUEST:
                case MessageType::GAME_ROOM_CLIENT_TEAM_CHAT_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: chat message detected");
                    GameRoomChatRequestMessage_t* chatMessage=(GameRoomChatRequestMessage_t*)_message;
                    bool teamChat=messageType==MessageType::GAME_ROOM_CLIENT_CHAT_REQUEST?false:true;
                    handleChatMessage(client,std::string(chatMessage->message),teamChat);
                    currentSize=sizeof(GameRoomChatRequestMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_CLIENT_READY_REQUEST:
                case MessageType::GAME_ROOM_CLIENT_NOT_READY_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: status change request detected");
                    GameRoomClientReadyRequestMessage_t* readyMessage=(GameRoomClientReadyRequestMessage_t*)_message;
                    bool ready=readyMessage->messageType==MessageType::GAME_ROOM_CLIENT_READY_REQUEST?true:false;
                    handlePlayerStatusChange(client,ready);
                    currentSize=sizeof(GameRoomClientReadyRequestMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_CLIENT_NICK_CHANGE_REQUEST:
                {
                    logger->addLine("Room: client nickname change request detected for "+Converter::intToString(client->getId()));
                    GameRoomNickChangeRequestMessage_t* nameS=(GameRoomNickChangeRequestMessage_t*)_message;
                    handleNickChangeRequest(client,std::string(nameS->name));
                    currentSize=sizeof(GameRoomNickChangeRequestMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_CLIENT_GET_CLIENTS:
                {
                    Logger::getSingleton()->addLine("Room: get clients request received");
                    handleGetClients(client);
                    currentSize=sizeof(GameRoomClientGetClientsMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_CLIENT_LEFT_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: client leave request received");
                    handleClientLeave(client);
                    currentSize=sizeof(BasicMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_CLIENT_KICK_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: client kick request received");
                    GameRoomClientKickRequestMessage_t* req=(GameRoomClientKickRequestMessage_t*)_message;
                    handleClientKick(client,req->uniqueId);
                    break;
                }
                case MessageType::GAME_ROOM_CLIENT_OWNER_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: owner request received");
                    handleOwnerRequest(client);
                    currentSize=sizeof(GameRoomOwnerRequestMessage_t);
                    break;
                }
				case MessageType::GAME_ROOM_CLIENT_TEAM_CHANGE_REQUEST:
			    {
					Logger::getSingleton()->addLine("Room: team change request received");
					GameRoomTeamChangeRequestMessage_t* tc=(GameRoomTeamChangeRequestMessage_t*)_message;
					handleTeamChangeRequest(client,tc->team);
					break;
				}
                case MessageType::GAME_START_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: game start request received");
                    handleGameStartRequest(client);
                    currentSize=sizeof(GameStartRequestMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_MAP_FILE_REQUEST_ACCEPT:
                {
                    Logger::getSingleton()->addLine("Room: map file request accepted");
                    //if(requestingMap)
                        startFileReceiver();
                    currentSize=sizeof(GameStartRequestMessage_t);
                    break;
                }
                case MessageType::GAME_ROOM_MAP_FILE_DOWNLOAD_COMPLETED:
                {
                    Logger::getSingleton()->addLine("Room: client "+client->getName()+" has downloaded the map file");
                    handleMapDownloadComplete(client);
                    break;
                }
				case MessageType::GAME_ROOM_CLIENT_COLOR_CHANGE_REQUEST:
				{
					Logger::getSingleton()->addLine("Room: color change request received from the client "+client->getName());
					GameRoomColorChangeRequestMessage_t* req=(GameRoomColorChangeRequestMessage_t*)_message;
					handleColorChange(client,req->color);
					break;
				}
                case MessageType::INGAME_PLAYER_KILLED:
                {
                    Logger::getSingleton()->addLine("Room: player death notification message detected");
                    PlayerKilledMessage_t* death=(PlayerKilledMessage_t*)_message;
                    if(clientUdpMap.find(death->enemyID)!=clientUdpMap.end())
                        handlePlayerDeath(client, clientUdpMap[death->enemyID], death->weaponType, death->posX, death->posY, death->posZ);
                    else
                        Logger::getSingleton()->addLine("Room: ERROR! unidentified enemy player!",true);
                    currentSize=sizeof(PlayerKilledMessage_t);
                    break;
                }
                case MessageType::INGAME_ITEM_TAKE_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: item take request detected");
                    ItemTakeRequestMessage_t* req=(ItemTakeRequestMessage_t*)_message;
                    handleItemTakeRequest(client,req->itemID);
                    currentSize=sizeof(ItemTakeRequestMessage_t);
                    break;
                }
                case MessageType::INGAME_SPAWN_POINT_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: spawn point request detected");
                    sendNextSpawnPoint(client);
                    currentSize=sizeof(SpawnPointRequest_t);
                    break;
                }
                case MessageType::INGAME_WEAPON_CHANGE_NOTIFICATION:
                {
                    Logger::getSingleton()->addLine("Room: weapon change notification received");
                    WeaponChangeNotificationMessage_t* info=(WeaponChangeNotificationMessage_t*)_message;
                    handleWeaponChange(client,info->weaponType);
                    currentSize=sizeof(WeaponChangeNotificationMessage_t);
                    break;
                }
                case MessageType::INGAME_PLAYER_JOINED:
                {
                    Logger::getSingleton()->addLine("Room: player "+client->getName()+" has joined the game");
                    handleGameJoin(client);
                    break;
                }
                case MessageType::INGAME_PLAYER_ITEM_USED:
                {
                    Logger::getSingleton()->addLine("Room: player "+client->getName()+ "has used a special item");
                    ItemUsedMessage_t* itemUsed=(ItemUsedMessage_t*)_message;
                    handleItemUsed(client,itemUsed->itemType);
                    break;
                }
                case MessageType::INGAME_TELEPORTER_USE_REQUEST:
                {
                    Logger::getSingleton()->addLine("Room: player "+client->getName()+" has requested to use teleporter");
                    TeleporterUseRequestMessage_t* req=(TeleporterUseRequestMessage_t*)_message;
                    handleTeleporterUseRequest(client,req->itemID);
                    break;
                }
            }
            remainingSize-=currentSize;
            _message+=currentSize;
        }
    }
    delete message;
}

void GameRoom::tcpMessageReceivedEvent(std::string ipAddress,int socket, char* message, size_t messageSize)
{
    char* _message=new char[messageSize];
    memcpy(_message,message,messageSize);
    delete message;
    scheduler->scheduleOperation(new Operation4<GameRoom,std::string,int,char*,size_t>(this,&GameRoom::handleGameMessage,ipAddress,socket,_message,messageSize));
}

void GameRoom::handleClientKick(Client* client, unsigned int kickPlayerID)
{
    if(client->getId()==ownerId && clientUdpMap.count(kickPlayerID)>0)
    {
        GameRoomClientKickedMessage_t message={MessageType::GAME_ROOM_CLIENT_KICKED,kickPlayerID};
        tcpBroadcastMessage(&message,sizeof(GameRoomClientKickedMessage_t));
        Logger::getSingleton()->addLine("Room: player "+clientUdpMap[kickPlayerID]->getName()+" kicked by the room owner "+client->getName());
    }
}

void GameRoom::startFileReceiver()
{
    Client* client=clientUdpMap[ownerId];
    fileReceiver=new FileReceiver(client->getIp(),Converter::intToString(TTFTP_CLIENT_PORT),this);
    fileReceiver->connect();
    Logger::getSingleton()->addLine("Room: requesting map file...");
}

void GameRoom::fileServerConnectionEstablished()
{
	std::cout<<"connected to the file server!\n";
    //if(requestingMap)
    {
        fileReceiver->requestFile(getMapName());
        Logger::getSingleton()->addLine("Room: file server connection established!");
    }
}

void GameRoom::fileFound()
{
    //if(requestingMap)
	{std::cout<<"starting to download the file\n";
        fileReceiver->startReceiving(getMapName(),filePath);
        Logger::getSingleton()->addLine("Room: file found by file server. starting to receive.");
    }
}

void GameRoom::fileNotFound()
{
    Logger::getSingleton()->addLine("Room: game cannot be started! file not found on the file server",true);
}

void GameRoom::fileTransferStarted()
{
    Logger::getSingleton()->addLine("Room: file transfer started!");
}

void GameRoom::fileTransferCompleted()
{
    Logger::getSingleton()->addLine("Room: file transfer completed!");
    requestingMap=false;
    /*if (initializeMap(getMapName()))
    {
        Logger::getSingleton()->addLine("Room: map initialization completed");
        if (clientTcpMap.size() == 1)
            startGame();
        else
        {
			Logger::getSingleton()->addLine("Room: notifying other players to download the map");
            notifyMapFileDownload(clientUdpMap[ownerId]);
        }
    }
    else*/
	std::cout<<"file downloaded\n";
	if(MapReader::isTTM(filePath+"/"+getMapName()) && initializeMap(filePath+"/"+getMapName()))
	{
		Logger::getSingleton()->addLine("Room: map file '"+getMapName()+"' is valid");
		fileServer->addFile(getMapName());
	}
	else
	{
        Logger::getSingleton()->addLine("Room: received map file is not coded in TTM format!", true);
		//terminate room
	}
    
    fileReceiver->stop();
    delete fileReceiver;
	fileReceiver=NULL;
}

void GameRoom::notifyMapFileDownload()
{
	Logger::getSingleton()->addLine("Room: sending map file download notification");
    Client* roomOwner = clientUdpMap[ownerId];
    GameRoomMapFileStartDownloadMessage_t message={MessageType::GAME_ROOM_MAP_FILE_START_DOWNLOAD};
    std::map<int,Client*>::iterator iter=clientTcpMap.begin();
    while(iter!=clientTcpMap.end())
	{
        if(iter->second!= roomOwner && !iter->second->hasMap())
		{
			Logger::getSingleton()->addLine("Room: ignoring client "+iter->second->getName());
            tcpConnector->sendData(iter->second->getGameSocket(),&message,sizeof(GameRoomMapFileStartDownloadMessage_t));
		}
		iter++;
	}
}

void GameRoom::handleMapDownloadComplete(Client* client)
{
    client->setHasMap(true);
    if(!started && isMapSyncCompleted())
        startGame();
    else if(started)
    {
		Logger::getSingleton()->addLine("Room: sending individual game start message");
        GameStartMessage_t message = {MessageType::GAME_START};
        tcpConnector->sendData(client->getGameSocket(),&message,sizeof(GameStartMessage_t));
    }
    else
        Logger::getSingleton()->addLine("Room: map sync is not completed. waiting for other players...");
}

bool GameRoom::isMapSyncCompleted()
{
    bool _ready=true;
    std::map<int,Client*>::iterator iter=clientTcpMap.begin();
    while(iter!=clientTcpMap.end())
    {
        if(!iter->second->hasMap())
        {
            _ready=false;
            break;
        }
        iter++;
    }
    return _ready;
}

void GameRoom::handleWeaponChange(Client* player, int weaponID)
{
    WeaponChangeMessage_t weaponChange={MessageType::INGAME_WEAPON_CHANGE,player->getId(),weaponID};
    tcpBroadcastMessage(&weaponChange,sizeof(WeaponChangeMessage_t),player);
}

void GameRoom::handleGameJoin(Client* client)
{
    sendNextSpawnPoint(client);
}

void GameRoom::handleItemTakeRequest(Client* player, int itemID)
{
    Item* item;
    if(itemMap.count(itemID)>0 && (item=itemMap[itemID])->isAlive())
    {
        item=itemMap[itemID];
		if(!item->isTemporary())
			item->die();
		else
		{
			//send destroy item message...
			delete item;
		}

		ItemNotAliveMessage_t alive={MessageType::INGAME_ITEM_NOT_ALIVE,itemID};
		tcpBroadcastMessage(&alive,sizeof(ItemNotAliveMessage_t));

        ItemTakeMessage_t take={MessageType::INGAME_ITEM_TAKE,itemID};
        tcpConnector->sendData(player->getGameSocket(),&take,sizeof(ItemTakeMessage_t));
    }
}

void GameRoom::handlePlayerDeath(Client* player, Client* killer,int weaponType,float x, float y, float z)
{
    if(started)
    {
        //notify players with the death event...
        sendNextSpawnPoint(player);
        player->increaseDeathCount();
        killer->increaseKillCount();
        sendPlayerScore(player);
        sendPlayerScore(killer);
        player->startSpawnTimer(4000,this);
        PlayerKillInfoMessage_t message={MessageType::INGAME_PLAYER_KILL_INFO,player->getId(),killer->getId()};
        tcpBroadcastMessage(&message,sizeof(PlayerKillInfoMessage_t));
        itemMap[currentItemID] = new Item(currentItemID, 0, NULL, 0, true);
        CreateItemMessage_t createItem = {MessageType::INGAME_CREATE_ITEM, currentItemID++, weaponType, x, y, z};
        tcpBroadcastMessage(&createItem, sizeof (CreateItemMessage_t));
    }
    else
        Logger::getSingleton()->addLine("Room: ERROR! game is not started!",true);
}

bool GameRoom::canStart()
{
    bool result=true;

    std::map<int,Client*>::iterator it=clientTcpMap.begin();
    while(it!=clientTcpMap.end())
    {
        if(!(it++)->second->isReady())
        {
            result=false;
            break;
        }
    }
    return result;
}

Entity2Position_t GameRoom::getNextSpawnPoint()
{
	Entity2Position_t pos2={0,0};
    if(spawnPointVector.size()>0)
    {
		if(spawnPointIterator==spawnPointVector.end())
            spawnPointIterator=spawnPointVector.begin();
        pos2=*(spawnPointIterator++);
    }
    return pos2;
}

void GameRoom::loadSpawnPoints()
{
    Logger::getSingleton()->addLine("Room: loading spawn points");
    Entity2Position_t* entity;
    Entity2Position_t pos2;
    while ((entity = mapReader->getSpawnPoint()) != NULL)
    {
		pos2.startX = entity->startX*(double)-mapReader->getScaleFactor();
        //pos2.y = 0;
        pos2.startZ = entity->startZ*(double)mapReader->getScaleFactor();
        spawnPointVector.push_back(pos2);
        //delete entity;
    }
	spawnPointIterator=spawnPointVector.begin();
    Logger::getSingleton()->addLine("Room: spawn points loaded");
}

void GameRoom::sendNextSpawnPoint(Client* client)
{
    SpawnPointMessage_t sp;
    sp.messageType=MessageType::INGAME_SPAWN_POINT;

    Entity2Position_t pos2 = getNextSpawnPoint();
    sp.x = pos2.startX;
    sp.y = 10;
    sp.z = pos2.startZ;
	std::string _pointString=Converter::intToString(sp.x)+","+Converter::intToString(sp.y)+","+Converter::intToString(sp.z);
	Logger::getSingleton()->addLine("Room: sending spawn point "+_pointString+" to client "+client->getName());
    tcpConnector->sendData(client->getGameSocket(),&sp,sizeof(SpawnPointMessage_t));
}

void GameRoom::sendInitialSpawnPoints()
{
    std::map<int,Client*>::iterator iter=clientTcpMap.begin();
    while(iter!=clientTcpMap.end())
    {
        sendNextSpawnPoint(iter->second);
        iter++;
    }
}

void GameRoom::loadItems()
{
    Logger::getSingleton()->addLine("Room: loading level items");
    MapEntity2* item;
    while ((item = mapReader->getItem()) != NULL)
    {
        itemMap[currentItemID] = new Item(currentItemID, 4000, this, item->type);
        currentItemID++;
    }
}

void GameRoom::loadTeleporters()
{
    Logger::getSingleton()->addLine("Room: loading teleporters");
    int teleporterID=0;
    while(mapReader->getTeleportEntity()!=NULL)
    {
        teleporterMap[teleporterID]=new Teleporter(teleporterID,this);
        teleporterID++;
    }
}

void GameRoom::handleItemResurrection(int itemID)
{
    Logger::getSingleton()->addLine("Room: map item respawned");
    ItemAliveMessage_t alive={MessageType::INGAME_ITEM_ALIVE,itemID};
    tcpBroadcastMessage(&alive,sizeof(ItemAliveMessage_t));
}

void GameRoom::teleporterActivatedEvent(int teleporterID)
{
    scheduler->scheduleOperation(new Operation1<GameRoom,int>(this,&GameRoom::handleTeleporterReactivation,teleporterID));
}

void GameRoom::handleTeleporterReactivation(int teleporterID)
{
    Logger::getSingleton()->addLine("Room: handling teleporter reactivation");
    TeleporterActivatedMessage_t message={MessageType::INGAME_TELEPORTER_ACTIVATED,teleporterID};
    tcpBroadcastMessage(&message,sizeof(TeleporterActivatedMessage_t));
}

void GameRoom::handleTeleporterUseRequest(Client* client,int teleporterID)
{
    if(teleporterMap.count(teleporterID)>0)
    {
        Teleporter* teleporter=teleporterMap[teleporterID];
        if(teleporter->isActivated())
        {
            teleporter->deactivate(5000);
            TeleporterUseAcceptMessage_t accept={MessageType::INGAME_TELEPORTER_USE_ACCEPT,teleporterID};
            tcpConnector->sendData(client->getGameSocket(),&accept,sizeof(TeleporterUseAcceptMessage_t));
            TeleporterDeactivatedMessage_t deactivate={MessageType::INGAME_TELEPORTER_DEACTIVATED,teleporterID};
            tcpBroadcastMessage(&deactivate,sizeof(TeleporterDeactivatedMessage_t));
        }
    }
    else
        Logger::getSingleton()->addLine("Room: Teleporter not found!",true);
}

void GameRoom::itemResurrected(int id)
{
    scheduler->scheduleOperation(new Operation1<GameRoom, int>(this, &GameRoom::handleItemResurrection, id));
}

void GameRoom::startGame()
{
    started=true;
    GameStartMessage_t message = {MessageType::GAME_START};
    tcpBroadcastMessage(&message, sizeof (GameStartMessage_t));
}

bool GameRoom::initializeMap(std::string mapName)
{
    Logger::getSingleton()->addLine("Room: initializing map");
    bool result=false;
	if(MapReader::isTTM(mapName))
	{
		mapReader = new MapReader(mapName);
		if(mapReader->readMap())
		{
			loadSpawnPoints();
			loadItems();
                        loadTeleporters();
			result=true;
		}
    }

    return result;
}

void GameRoom::handleGameStartRequest(Client* client)
{
    if (ownerId == client->getId())
    {
        bool ready=true;
        std::map<int,Client*>::iterator iter=clientTcpMap.begin();
        while(iter!=clientTcpMap.end())
        {
            if(iter->second!=client && !iter->second->isReady())
            {
                ready=false;
                break;
            }
            iter++;
        }
        if(ready)
        {
			Logger::getSingleton()->addLine("Room: game is ready to be started");
			/*
            if(initializeMap(getMapName()))
            {
                Logger::getSingleton()->addLine("Room: map has been read");
                loadSpawnPoints();
                //loadItems();
                //sendInitialSpawnPoints();
                GameStartMessage_t message = {MessageType::GAME_START};
                tcpBroadcastMessage(&message, sizeof (GameStartMessage_t));
            }
            else*/
            {
                //always download the map from the room owner
				if(isMapSyncCompleted())
					startGame();
				else
				{
					Logger::getSingleton()->addLine("Room: notifying file download");
					notifyMapFileDownload();
				}
                //requestMapFile();
            }
        }
        else
			Logger::getSingleton()->addLine("Room: players are not ready to start");
    }
	else
		Logger::getSingleton()->addLine("Room: cannot start the game. player is not the owner");
}

void GameRoom::requestMapFile()
{
    Client* owner=clientUdpMap[ownerId];
    owner->setHasMap(true);
    requestingMap=true;
    GameRoomMapFileRequestMessage_t message={MessageType::GAME_ROOM_MAP_FILE_REQUEST};
    tcpConnector->sendData(owner->getGameSocket(),&message,sizeof(GameRoomMapFileRequestMessage_t));
    Logger::getSingleton()->addLine("Room: requesting map file from the client " + owner->getName());
}

void GameRoom::handleOwnerRequest(Client* client)
{
    GameRoomOwnerMessage_t info={MessageType::GAME_ROOM_CLIENT_OWNER,ownerId};
    tcpConnector->sendData(client->getGameSocket(),&info,sizeof(GameRoomOwnerMessage_t));
}

void GameRoom::handleGetClients(Client* client)
{
    GameRoomClientInfoMessage_t info;
    info.messageType=MessageType::GAME_ROOM_CLIENT_INFO;
    std::map<int,Client*>::iterator it=clientTcpMap.begin();
    Client* _client;
    while(it!=clientTcpMap.end())
    {
        if((_client=it->second)!=client)
        {
			info.kill=_client->getKillCount();
			info.death=_client->getDeathCount();
            info.team=_client->getTeam();
            info.uniqueId=_client->getId();
			info.color=_client->getColor();
            strncpy(info.clientName,_client->getName().c_str(),30);
            tcpConnector->sendData(client->getGameSocket(),&info,sizeof(GameRoomClientInfoMessage_t));
        }
        it++;
    }
}


void GameRoom::handleNickChangeRequest(Client* client, std::string name)
{
    logger->addLine("Room: client name "+client->getName()+" -> "+name);
    client->setName(name);
    GameRoomNickChangeMessage_t message;
    message.messageType=MessageType::GAME_ROOM_CLIENT_NICK_CHANGE;
    message.id=client->getId();
    strcpy(message.name,client->getName().c_str());
    tcpBroadcastMessage(&message,sizeof(GameRoomNickChangeMessage_t));
}

void GameRoom::handlePlayerStatusChange(Client* client, bool ready)
{
    client->setReady(ready);
    GameRoomClientReadyStatusMessage_t status;
    status.messageType=ready?MessageType::GAME_ROOM_CLIENT_READY:MessageType::GAME_ROOM_CLIENT_NOT_READY;
    status.clientID=client->getId();
    tcpBroadcastMessage(&status,sizeof(GameRoomClientReadyStatusMessage_t));
}

void GameRoom::handleChatMessage(Client* client, std::string chatMessage,bool teamChat)
{
    GameRoomChatMessage_t message;
    message.messageType = (teamChat ? MessageType::GAME_ROOM_CLIENT_TEAM_CHAT : MessageType::GAME_ROOM_CLIENT_CHAT);
    strcpy(message.message, chatMessage.c_str());
    message.ownerID = client->getId();
    if (teamChat)
        tcpBroadcastTeamMessage(&message, sizeof (GameRoomChatMessage_t), client->getTeam());
    else
        tcpBroadcastMessage(&message, sizeof (GameRoomChatMessage_t));
}

void GameRoom::tcpBroadcastTeamMessage(void* message, int size, unsigned char team, Client* clientToBeIgnored)
{
    std::map<int,Client*>::iterator it=clientTcpMap.begin();
    while(it!=clientTcpMap.end())
    {
        if(it->second!=clientToBeIgnored && it->second->getTeam()==team)
            tcpConnector->sendData(it->second->getGameSocket(),message,size);
        it++;
    }
}

void GameRoom::handleItemUsed(Client* client, int itemType)
{
    ExpirableItem* exItem=new ExpirableItem(client->getId(),itemType,this);
    expirableItemVector.push_back(exItem);
    exItem->startExpirationTimer(30000);
    ItemUsedNotificationMessage_t message={MessageType::INGAME_PLAYER_ITEM_USED_NOTIFICATION,client->getId(),itemType};
    tcpBroadcastMessage(&message,sizeof(ItemUsedNotificationMessage_t),client);
}

void GameRoom::handleItemExpiration(unsigned int playerID,int itemType)
{
    if(clientUdpMap.count(playerID)>0)
    {
        Client* client=clientUdpMap[playerID];
        ItemExpiredMessage_t uniExpired={MessageType::INGAME_PLAYER_ITEM_EXPIRED,itemType};
        tcpConnector->sendData(client->getGameSocket(),&uniExpired,sizeof(ItemExpiredMessage_t));
        ItemExpiredNotificationMessage_t brExpired={MessageType::INGAME_PLAYER_ITEM_EXPIRED_NOTIFICATION,playerID,itemType};
        tcpBroadcastMessage(&brExpired,sizeof(ItemExpiredNotificationMessage_t),client);
    }
	else
		Logger::getSingleton()->addLine("Room: cannot notify item expiration! client does not exist!",true);
}

void GameRoom::itemExpiredEvent(unsigned int playerID, int itemType)
{
	scheduler->scheduleOperation(new Operation2<GameRoom,unsigned int,int>(this,&GameRoom::handleItemExpiration,playerID,itemType));
}

void GameRoom::handleConnectionRequest(unsigned int clientID,int socket)
{
    if (clientWaitMap.find(clientID) != clientWaitMap.end() && !clientWaitMap[clientID]->isJoined())
    {
        Client* client = clientWaitMap[clientID];
        clientWaitMap.erase(clientID);
        clientTcpMap[socket]=client;
        clientUdpMap[clientID]=client;
        playerCount++;
        client->setGameSocket(socket);
        GameRoomConnectionAcceptMessage_t accept={MessageType::GAME_ROOM_CONNECTION_ACCEPT};
        tcpConnector->sendData(socket,&accept,sizeof(GameRoomConnectionAcceptMessage_t));
        client->setJoined(true);
        client->zeroRatio();
		client->setTeam(Teams::BLUE);
        advertiseClient(client);
        eventObject->roomStateChanged(getRoomId());
        if (clientID == ownerId)
            requestMapFile();
        if(started)
		{
			Logger::getSingleton()->addLine("Room: game already started. handling map download");
            notifyMapFileDownload();
		}

        logger->addLine("Room: valid client " + client->getName() + " has joined to the game");
    }
    else
    {
        GameRoomConnectionDeniedMessage_t denied={MessageType::GAME_ROOM_CONNECTION_DENIED};
        tcpConnector->sendData(socket,&denied,sizeof(GameRoomConnectionDeniedMessage_t));
        tcpConnector->terminateConnection(socket);
        logger->addLine("Room: incoming connection request with the client id "+Converter::intToString(clientID)+" is refused by the room");
    }
}

void GameRoom::advertiseClient(Client* client)
{    
    GameRoomClientJoinedMessage_t message;
    int size=sizeof(GameRoomClientJoinedMessage_t);
    message.messageType=MessageType::GAME_ROOM_CLIENT_JOINED;
    message.uniqueId=client->getId();
	message.team=client->getTeam();
	message.kill=client->getKillCount();
	message.death=client->getDeathCount();
	message.color=client->getColor();
    strncpy(message.clientName,client->getName().c_str(),30);
    tcpBroadcastMessage(&message,size,client);
}

void GameRoom::advertiseNewOwner()
{
    GameRoomOwnerMessage_t info={MessageType::GAME_ROOM_CLIENT_OWNER_CHANGE,ownerId};
    tcpBroadcastMessage(&info,sizeof(GameRoomOwnerChangeMessage_t));
}

void GameRoom::tcpBroadcastMessage(void* message,int size,Client* clientToBeIgnored)
{
    std::map<int,Client*>::iterator it=clientTcpMap.begin();
    while(it!=clientTcpMap.end())
    {
        if(it->second!=clientToBeIgnored)
            tcpConnector->sendData(it->second->getGameSocket(),message,size);
        it++;
    }
}

void GameRoom::udpBroadcastMessage(void* message,int size,Client* clientToBeIgnored)
{
    std::map<unsigned int,Client*>::iterator it=clientUdpMap.begin();
    while(it!=clientUdpMap.end())
    {
        if(it->second!=clientToBeIgnored)
            udpClient->sendData(it->second->getIp(),Converter::intToString(getUdpClientPort()),(char*)message,size);
        it++;
    }
}

void GameRoom::addToWaitingList(Client* client)
{
    clientWaitMap[client->getId()]=client;
}

bool GameRoom::isTTNP(char* message)
{
    UdpHeader_t* header=(UdpHeader_t*)message;
    if(header->protocolID==TTNP_ID)
        return true;
    return false;    
}

void GameRoom::udpMessageReceivedEvent(std::string ipAddress,char* message, size_t messageSize)
{
    BasicMessage_t* bMessage=(BasicMessage_t*)message;
    unsigned char messageType=bMessage->messageType;
    int remainingSize=messageSize;
    int currentSize=0;
    char* _message=message;
    UdpHeader_t* header;
    Client* client;
    
    //while (remainingSize > 0)
    {
        if (isTTNP(_message))
        {
            header = (UdpHeader_t*) _message;
            messageType = header->messageType;
            if (clientUdpMap.find(header->clientID) != clientUdpMap.end())
                client = clientUdpMap[header->clientID];
            switch (messageType)
            {
                case MessageType::POSITION_UPDATE:
                {
                    handleTransformUpdate(client,(PositionUpdateMessage*)_message);
                    currentSize=sizeof(PositionUpdateMessage_t);
                    break;
                }
                case MessageType::BULLET:
                {
                    handleFireEvent(client,(BulletInfo_t*)_message);
                    currentSize=sizeof(BulletInfo_t);
                    break;
                }
                default:
                {
                    remainingSize = 0;
                }
            }
        }
        remainingSize -= currentSize;
        _message += currentSize;
    }
    
    delete message;
}

void GameRoom::handleTeamChangeRequest(Client* client,unsigned char newTeam)
{
	client->setTeam(newTeam);
	GameRoomTeamChangedMessage_t teamChanged={MessageType::GAME_ROOM_CLIENT_TEAM_CHANGED,newTeam,client->getId()};
	tcpBroadcastMessage(&teamChanged,sizeof(GameRoomTeamChangedMessage_t));
}

void GameRoom::handleFireEvent(Client* player, BulletInfo_t* bulletInfo)
{
    udpBroadcastMessage(bulletInfo,sizeof(BulletInfo_t),player);
}

void GameRoom::handleTransformUpdate(Client* player, PositionUpdateMessage_t* transformUpdate)
{
    udpBroadcastMessage(transformUpdate, sizeof (PositionUpdateMessage_t), player);
}

void GameRoom::connectionTerminatedEvent(int socket)
{
	scheduler->scheduleOperation(new Operation1<GameRoom,int>(this,&GameRoom::handleClientLeave,socket));
}

void GameRoom::handleClientLeave(int socket)
{
    if (clientTcpMap.count(socket)>0)
        handleClientLeave(clientTcpMap[socket]);
}

void GameRoom::handleClientLeave(Client* client)
{
    playerCount--;
    clientTcpMap.erase(client->getGameSocket());
    clientUdpMap.erase(client->getId());
    tcpConnector->terminateConnection(client->getGameSocket());
    client->stopSpawnTimer();
    if(client->getId()==ownerId)
        reassignOwner();
	client->setHasMap(false);
    GameRoomClientLeftMessage_t message={MessageType::GAME_ROOM_CLIENT_LEFT,client->getId(),ownerId};
    tcpBroadcastMessage(&message,sizeof(GameRoomClientLeftMessage_t));
    client->setJoined(false);
	new boost::thread(&RoomEventInterface::roomStateChanged,eventObject,getRoomId());
}

void GameRoom::reassignOwner()
{
    std::map<int,Client*>::iterator iter=clientTcpMap.begin();
    if(iter!=clientTcpMap.end())
        ownerId=((Client*)iter->second)->getId();
}

void GameRoom::handleClientResurrection(unsigned int clientID)
{
	Logger::getSingleton()->addLine("Room: player "+clientUdpMap[clientID]->getName()+" has resurrected");
    PlayerResurrected_t res={MessageType::INGAME_PLAYER_RESURRECTED,clientID};
    tcpBroadcastMessage(&res,sizeof(PlayerResurrected_t));
}

void GameRoom::clientResurrectedEvent(unsigned int id)
{	
	scheduler->scheduleOperation(new Operation1<GameRoom,unsigned int>(this,&GameRoom::handleClientResurrection,id));
}

void GameRoom::sendPlayerScore(Client* client)
{
    PlayerScoreNotificationMessage_t score;
    score.messageType=MessageType::INGAME_PLAYER_SCORE_NOTIFICATION;
    score.deathCount=client->getDeathCount();
    score.killCount=client->getKillCount();
    score.playerID=client->getId();
    tcpBroadcastMessage(&score,sizeof(PlayerScoreNotificationMessage_t));
}

void GameRoom::handleColorChange(Client* client,unsigned char color)
{
	client->setColor(color);
	GameRoomColorChangedMessage_t message={MessageType::GAME_ROOM_CLIENT_COLOR_CHANGED,client->getId(),color};
	tcpBroadcastMessage(&message,sizeof(GameRoomColorChangedMessage_t));
}

std::string GameRoom::getRoomName()
{
    return name;
}

int GameRoom::getRoomId()
{
    return id;
}

int GameRoom::getPlayerCount()
{
    return playerCount;
}

int GameRoom::getUdpServerPort()
{
    return udpServerPort;
}

int GameRoom::getUdpClientPort()
{
    return udpClientPort;
}

int GameRoom::getTcpPort()
{
    return tcpPort;
}

std::string GameRoom::getMapName()
{
    return mapName;
}

int GameRoom::getCapacity()
{
    return capacity;
}

unsigned char GameRoom::getGameType()
{
    return gameType;
}

bool GameRoom::isStarted()
{
    return started;
}

int GameRoom::getTTFTPPort()
{
    return ttftpPort;
}

void GameRoom::destroy()
{
    if(tcpConnector!=NULL)
        tcpConnector->stop();
    if(fileServer!=NULL)
        fileServer->stop();
    if(fileReceiver!=NULL)
        fileReceiver->stop();
    
    std::map<unsigned int,Client*>::iterator clientIterator=clientUdpMap.begin();
    while(clientIterator!=clientUdpMap.end())
        (clientIterator++)->second->stopSpawnTimer();

    std::map<int,Item*>::iterator itemIterator=itemMap.begin();
    while(itemIterator!=itemMap.end())
        (itemIterator++)->second->stopSpawnTimer();

    for(int i=0;i<expirableItemVector.size();i++)
        expirableItemVector.at(i)->stopExpirationTimer();

    for(std::map<int,Teleporter*>::iterator iter=teleporterMap.begin();iter!=teleporterMap.end();iter++)
            iter->second->stopActivationTimer();

    active = false;
    loopThread->join();
    //loopThread->interrupt();
}