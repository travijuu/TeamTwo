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

#include "GameLobby.h"

GameLobby::GameLobby(std::string lobbyTcpPort,Logger* logger,std::string minTcpPort,std::string maxTcpPort,std::string minUdpPort,std::string maxUdpPort)
{
    this->minTcpPort=Converter::stringToInt(minTcpPort);
    this->maxTcpPort=Converter::stringToInt(maxTcpPort);
    this->minUdpPort=Converter::stringToInt(minUdpPort);
    this->maxUdpPort=Converter::stringToInt(maxUdpPort);
    this->logger=logger;
    this->lobbyTcpPort=lobbyTcpPort;
    logger->addLine("Lobby: initializing...");
    initialize();
}

void GameLobby::initialize()
{
    tcpConnector=new TcpConnector(lobbyTcpPort);
    tcpConnector->enableCallback(this);
    logger->addLine("Lobby: initialization completed");
}

void GameLobby::start()
{
    tcpConnector->startListening();
}

void GameLobby::tcpMessageReceivedEvent(std::string ipAddress,int socket,char* message,size_t messageSize)
{
    BasicMessage_t* bMessage=(BasicMessage_t*)message;
    unsigned char messageType=bMessage->messageType;
    logger->addLine("Lobby: a message("+Converter::intToString((int)messageType)+")  received from a client. size: "+Converter::intToString(messageSize)+" bytes");
    
	if(clientMap.find(socket)==clientMap.end())
        {
            if(messageType==MessageType::CONNECTION_REQUEST)
            {
                logger->addLine("Lobby: incoming connection detected...");
                ConnectionRequestMessage_t* req=(ConnectionRequestMessage_t*)message;
				acceptConnection(ipAddress,socket,req->playerName,req->color);
            }
            else;
        }
        else
        {
            int size=messageSize;
            int structSize=0;
            char* _message=(char*)message;
            Client* client=clientMap[socket];
            //while(size>0)
            {
                messageType=((BasicMessage_t*)_message)->messageType;
                switch(messageType)
                {
                    case MessageType::CLIENT_GET_ROOMS:
                    {
                        logger->addLine("Lobby: get game room message received from "+client->getName());
                        sendGameRooms(client);

                        structSize=sizeof(GameRoomInformationRequestMessage_t);
                    }break;
                    case MessageType::CLIENT_CREATE_ROOM:
                    {
                        logger->addLine("Lobby: create game room message received from "+client->getName());
                        GameRoomCreateRequestMessage_t* req=(GameRoomCreateRequestMessage_t*)_message;
                        int roomId=createGameRoom(client,std::string(req->name),req->gameType,req->mapName,req->gameCapacity);
                        GameRoomCreateAcceptMessage_t accept;
                        accept.messagetype=MessageType::CLIENT_CREATE_ROOM_ACCEPT;
                        accept.roomID=roomId;
                        accept.tcpPort=gameRoomMap[roomId]->getTcpPort();
                        accept.TTFTPServerPort=gameRoomMap[roomId]->getTTFTPPort();
                        accept.udpServerPort=gameRoomMap[roomId]->getUdpServerPort();
                        accept.udpClientPort=gameRoomMap[roomId]->getUdpClientPort();
                        strcpy(accept.roomName,gameRoomMap[roomId]->getRoomName().c_str());
                        strcpy(accept.mapName,gameRoomMap[roomId]->getMapName().c_str());
                        accept.capacity=req->gameCapacity;
                        accept.gameType=req->gameType;
                        gameRoomMap[roomId]->addToWaitingList(client);
                        tcpConnector->sendData(socket,&accept,sizeof(GameRoomCreateAcceptMessage_t));
                        broadcastGameRoom(gameRoomMap[roomId]);
                        structSize=sizeof(GameRoomCreateRequestMessage_t);
                    }break;
                    case MessageType::CLIENT_ROOM_JOIN_REQUEST:
                    {
                        logger->addLine("Lobby: game room join request message received from "+client->getName());
                        GameRoomJoinRequestMessage_t* req=(GameRoomJoinRequestMessage_t*)_message;
                        if(gameRoomMap.find(req->uniqueId)!=gameRoomMap.end() && !client->isJoined() && gameRoomMap[req->uniqueId]->getCapacity()-gameRoomMap[req->uniqueId]->getPlayerCount()>0)
                        {
                            handleGameRoomJoinRequest(client,gameRoomMap[req->uniqueId]);
                            logger->addLine("Lobby: join request from "+client->getName()+" for room "+Converter::intToString(req->uniqueId)+" has accepted");
                        }
                        else
                        {
                            logger->addLine("Lobby: room id "+Converter::intToString(req->uniqueId)+" not found");
                            logger->addLine("Lobby: sending join deny message...");
                            sendBasicMessage(MessageType::CLIENT_ROOM_JOIN_DENY,client);
                        }

                        structSize=sizeof(GameRoomJoinRequestMessage_t);

                    }
					break;
                    default:logger->addLine("Lobby: UNIDENTIFIED PACKET RECEIVED("+Converter::intToString((int)messageType)+")!",true);
                    structSize=size;
                }
                size-=structSize;
                _message+=structSize;
            }
        }
        delete message;
}

void GameLobby::handleGameRoomJoinRequest(Client* client, GameRoom* room)
{
    room->addToWaitingList(client);
    GameRoomJoinAcceptMessage_t message={MessageType::CLIENT_ROOM_JOIN_ACCEPT,room->getRoomId(),room->getTcpPort(),room->getUdpServerPort(),room->getUdpClientPort(),room->getTTFTPPort()};
    tcpConnector->sendData(client->getLobbySocket(),&message,sizeof(GameRoomJoinAcceptMessage_t));
    //broadcastGameRoom(room);
    logger->addLine("Lobby: client "+client->getName()+" joined the room "+room->getRoomName());
}

void GameLobby::connectionTerminatedEvent(int socket)
{
    logger->addLine("Lobby: connection terminated with the client named: "+clientMap[socket]->getName());
    Client* client= clientMap[socket];
    clientMap.erase(socket);
	while(client->isJoined());
	Logger::getSingleton()->addLine("Lobby: deleting client...");
    delete client;
	Logger::getSingleton()->addLine("Lobby: client has been deleted");

}

void GameLobby::acceptConnection(std::string ipAddress,int socket,std::string playerName,unsigned char color)
{
    Logger::getSingleton()->addLine("Lobby: connection accepted");
    Client* client=new Client(playerName,generateNumber(0,10000),ipAddress,socket);                         
	client->setColor(color);
    clientMap[socket]=client;
    ConnectionAcceptMessage_t message;
    message.messageType=MessageType::CONNECTION_ACCEPT;
    message.uniqueId=client->getId();
    tcpConnector->sendData(client->getLobbySocket(),&message,sizeof(ConnectionAcceptMessage_t));
    logger->addLine("Lobby: New client created:");
    logger->addLine("Lobby: --id: "+Converter::intToString(client->getId()));
    logger->addLine("Lobby: --name: "+client->getName());
}

void GameLobby::sendGameRooms(Client* client)
{
    Logger::getSingleton()->addLine("Lobby: sending rooms to "+client->getName());
    std::map<int,GameRoom*>::iterator iter=gameRoomMap.begin();
    
    for(int i=0;i<gameRoomMap.size();i++)
    {
        GameRoom* room=iter->second;
        GameRoomInformationMessage_t info;
        info.messageType=MessageType::ROOM_INFO;
        info.id=room->getRoomId();
        strcpy(info.roomName,room->getRoomName().c_str());
        strcpy(info.mapName,room->getMapName().c_str());
        info.maxPlayerCount=room->getCapacity();
        info.gameType=room->getGameType();
        info.playerCount=room->getPlayerCount();
        tcpConnector->sendData(client->getLobbySocket(),&info,sizeof(GameRoomInformationMessage_t));
        iter++;
    }

    if(gameRoomMap.size()==0)
        Logger::getSingleton()->addLine("Lobby: nothing to send");
}

int GameLobby::createGameRoom(Client* client,std::string roomName,unsigned char gameType,std::string mapName,int capacity)
{
    int roomId=generateNumber(100,10000);
    int randomTcpPort=generateNumber(minTcpPort,maxTcpPort);
    int udpServerPort=generateNumber(minUdpPort,maxUdpPort);
    int udpClientPort=generateNumber(minUdpPort,maxUdpPort);
    int ttftpPort=generateNumber(minTcpPort,maxTcpPort);
    GameRoom* room=new GameRoom(roomId,roomName,randomTcpPort,ttftpPort,udpServerPort,udpClientPort,client->getId(),gameType,mapName,capacity,this);
    gameRoomMap[roomId]=room;
    logger->addLine("Lobby: new room created");
    logger->addLine("Lobby: --id:"+Converter::intToString(roomId));
    logger->addLine("Lobby: --name:"+roomName);
    logger->addLine("Lobby: --level:"+mapName);
    logger->addLine("Lobby: --capacity:"+Converter::intToString(capacity));
	logger->addLine("Lobby: --game type:"+std::string((gameType==0?"Deathmatch":"Team Deathmatch")));
    logger->addLine("Lobby: --tcp port:"+Converter::intToString(randomTcpPort));
    logger->addLine("Lobby: --ttftp port:"+Converter::intToString(ttftpPort));
    logger->addLine("Lobby: --udp server port:"+Converter::intToString(udpServerPort));
    logger->addLine("Lobby: --udp client port:"+Converter::intToString(udpClientPort));
    return roomId;
}

void GameLobby::broadcastGameRoom(GameRoom* room,Client* clientToIgnore)
{
    Logger::getSingleton()->addLine("Lobby: broadcasting new room");
    GameRoomInformationMessage_t info;
    info.messageType=MessageType::ROOM_INFO;
    info.playerCount=1;
    info.maxPlayerCount=room->getCapacity();
    info.id=room->getRoomId();
    strcpy(info.roomName,room->getRoomName().c_str());
    strcpy(info.mapName,room->getMapName().c_str());
    broadcastMessage(&info,sizeof(GameRoomInformationMessage_t),clientToIgnore);
}

void GameLobby::broadcastNewClientName(Client* client)
{
    GameRoomNickChangeMessage_t message;
    message.messageType=MessageType::GAME_ROOM_CLIENT_NICK_CHANGE;
    strcpy(message.name,client->getName().c_str());
    broadcastMessage(&message,sizeof(GameRoomNickChangeMessage_t),client);
    logger->addLine("Lobby: "+client->getName()+" client name broadcasted");
}

void GameLobby::broadcastMessage(void* message, size_t size, Client* clientToIgnore)
{
    std::map<int,Client*>::iterator iter=clientMap.begin();
    for(int i=0;i<clientMap.size();i++)
    {
        if(iter->second!=clientToIgnore)
            tcpConnector->sendData(iter->second->getLobbySocket(),message,size);
        iter++;
    }
}

int GameLobby::generateNumber(int min, int max)
{
    static int val=1;
    static std::map<int,int> numberMap;
    int uniqueId=-1;
    int number=-1;
    do
    {
        srand(val+time(NULL));
        uniqueId=rand();
        val+=uniqueId%100;
        number=(uniqueId%(max-min))+min;
    }while(numberMap.count(number)>0);
    numberMap[number]=number;
    return number;
}

void GameLobby::sendBasicMessage(int messageType,Client* client=NULL)
{
    BasicMessage_t message={messageType};
    if(client!=NULL)
        tcpConnector->sendData(client->getLobbySocket(),&message,sizeof(BasicMessage_t));
    else
        broadcastMessage(&message,sizeof(BasicMessage_t));
}

void GameLobby::roomStateChanged(int roomID)
{
	std::cout<<"player count: "<<gameRoomMap[roomID]->getPlayerCount()<<std::endl;
    if(gameRoomMap[roomID]->getPlayerCount()==0)
        removeRoom(gameRoomMap[roomID]);
    else
        notifyRoomStateChange(gameRoomMap[roomID]);
}

void GameLobby::removeRoom(GameRoom* room)
{
    GameRoomRemovedMessage_t message={MessageType::ROOM_REMOVED,room->getRoomId()};
    broadcastMessage(&message,sizeof(GameRoomRemovedMessage_t));
    gameRoomMap.erase(room->getRoomId());
    room->destroy();
    delete room;
    Logger::getSingleton()->addLine("Lobby: room deleted");
}

void GameLobby::notifyRoomStateChange(GameRoom* room)
{
    Logger::getSingleton()->addLine("Lobby: broadcasting state change of room "+room->getRoomName());
    GameRoomStateChangedMessage_t stateMessage;
    stateMessage.messageType=MessageType::ROOM_STATE_CHAGED;
    stateMessage.gameType=room->getGameType();
    stateMessage.id=room->getRoomId();
    stateMessage.maxPlayerCount=room->getCapacity();
    stateMessage.playerCount=room->getPlayerCount();
    strcpy(stateMessage.mapName,room->getMapName().c_str());
    strcpy(stateMessage.roomName,room->getRoomName().c_str());
    broadcastMessage(&stateMessage,sizeof(GameRoomStateChangedMessage_t));
}