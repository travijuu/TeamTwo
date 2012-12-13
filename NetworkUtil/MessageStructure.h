/*
	Copyright (C) 2011-2012  Alican Sekerefe 

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

#ifndef MESSAGESTRUCTURE_H
#define	MESSAGESTRUCTURE_H

/*	MessageStructure

	This structure collection contains
	necessary structs which are primarily
	used by the TTNP protocol for general
	purpose client and server messaging.


*/

#define SERVER_NAME_SIZE 52
#define GAME_ROOM_NAME_SIZE 32
#define TTNP_ID 65432123

namespace MessageType
{
    enum MessageTypes
    {
        POSITION_UPDATE = 0,
        BULLET,

        CONNECTION_REQUEST,
        CONNECTION_ACCEPT,
        ROOM_INFO,
        ROOM_STATE_CHAGED,
        ROOM_REMOVED,
        CLIENT_GET_ROOMS,
        CLIENT_CREATE_ROOM,
        CLIENT_CREATE_ROOM_ACCEPT,
        CLIENT_ROOM_JOIN_REQUEST,
        CLIENT_ROOM_JOIN_ACCEPT,
        CLIENT_ROOM_JOIN_DENY,
        CLIENT_NAME_CHANGE,
        CLIENT_CHAT,
        CLIENT_CHAT_FORWARD,

        GAME_ROOM_CONNECTION_REQUEST,
        GAME_ROOM_CONNECTION_ACCEPT,
        GAME_ROOM_CONNECTION_DENIED,
        GAME_ROOM_MAP_FILE_REQUEST,
        GAME_ROOM_MAP_FILE_REQUEST_ACCEPT,
        GAME_ROOM_MAP_FILE_START_DOWNLOAD,
        GAME_ROOM_MAP_FILE_DOWNLOAD_COMPLETED,
        GAME_ROOM_CLIENT_JOINED,
        GAME_ROOM_CLIENT_LEFT_REQUEST,
        GAME_ROOM_CLIENT_LEFT,
        GAME_ROOM_CLIENT_CHAT_REQUEST,
        GAME_ROOM_CLIENT_TEAM_CHAT_REQUEST,
        GAME_ROOM_CLIENT_CHAT,
        GAME_ROOM_CLIENT_TEAM_CHAT,
        GAME_ROOM_CLIENT_NICK_CHANGE_REQUEST,
        GAME_ROOM_CLIENT_NICK_CHANGE,
        GAME_ROOM_CLIENT_NOT_READY_REQUEST,
        GAME_ROOM_CLIENT_READY_REQUEST,
        GAME_ROOM_CLIENT_READY,
        GAME_ROOM_CLIENT_NOT_READY,
        GAME_ROOM_CLIENT_GET_CLIENTS,
        GAME_ROOM_CLIENT_INFO,
        GAME_ROOM_CLIENT_OWNER_REQUEST,
        GAME_ROOM_CLIENT_OWNER,
        GAME_ROOM_CLIENT_OWNER_CHANGE,
        GAME_ROOM_CLIENT_KICK_REQUEST,
        GAME_ROOM_CLIENT_KICKED,
		GAME_ROOM_CLIENT_TEAM_CHANGE_REQUEST,
		GAME_ROOM_CLIENT_TEAM_CHANGED,
		GAME_ROOM_CLIENT_COLOR_CHANGE_REQUEST,
		GAME_ROOM_CLIENT_COLOR_CHANGED,
        GAME_EXIT,
        GAME_START_REQUEST,
        GAME_START,

        
        INGAME_PLAYER_KILLED,
        INGAME_PLAYER_KILL_INFO,
        INGAME_PLAYER_ITEM_USED_NOTIFICATION,
        INGAME_PLAYER_ITEM_USED,
        INGAME_PLAYER_ITEM_EXPIRED,
        INGAME_PLAYER_ITEM_EXPIRED_NOTIFICATION,
        INGAME_PLAYER_RESURRECTED,
        INGAME_SPAWN_POINT_REQUEST,
        INGAME_SPAWN_POINT,
        INGAME_ITEM_TAKE_REQUEST,
        INGAME_ITEM_TAKE,
        INGAME_ITEM_ALIVE,
        INGAME_ITEM_NOT_ALIVE,
        INGAME_WEAPON_CHANGE_NOTIFICATION,
        INGAME_WEAPON_CHANGE,
        INGAME_RESET_POSITION,
        INGAME_PLAYER_JOINED,
        INGAME_CREATE_ITEM,
        INGAME_TELEPORTER_USE_REQUEST,
        INGAME_TELEPORTER_USE_ACCEPT,
        INGAME_TELEPORTER_ACTIVATED,
        INGAME_TELEPORTER_DEACTIVATED,
        INGAME_PLAYER_SCORE_NOTIFICATION,


    };
}

namespace GameType
{
    enum GameTypes
    {
        DM=0,
        TDM,
    };
}

namespace Teams
{
	enum Team
	{
		RED=0,
		BLUE,
	};
}

typedef struct SpawnPointMessage
{
    unsigned char messageType;
    float x;
    float y;
    float z;
}SpawnPointMessage_t;

typedef struct BasicMessage
{
    unsigned char messageType;
}BasicMessage_t;

typedef struct ConnectionAcceptMessage
{
    unsigned char messageType;
    unsigned int uniqueId;
    int TTFTPClientPort;
}ConnectionAcceptMessage_t;

typedef struct GameRoomInformationMessage
{
    unsigned char messageType;
    char roomName[30];
    char mapName[30];
    unsigned int id;
    unsigned char maxPlayerCount;
    unsigned char playerCount;
    unsigned char gameType;
}GameRoomInformationMessage_t;

typedef struct GameRoomCreateRequestMessage
{
    unsigned char messageType;
    unsigned char gameType;
    char name[30];
    char mapName[30];
    unsigned char gameCapacity;
}GameRoomCreateRequestMessage_t;

typedef struct GameRoomNickChangeMessage
{
    unsigned char messageType;
    char name[30];
    unsigned int id;
}GameRoomNickChangeMessage_t;

typedef struct GameRoomNickChangeRequestMessage
{
    unsigned char messageType;
    char name[30];
}GameRoomNickChangeRequestMessage_t;


typedef struct GameRoomJoinAcceptMessage
{
    unsigned char messagetype;
    unsigned int roomID;
    int tcpPort;
    int udpServerPort;
    int udpClientPort;
    int ttftpPort;
}GameRoomJoinAcceptMessage_t;

typedef struct GameRoomChatRequestMessage
{
    unsigned char messageType;
    char message[255];
}GameRoomChatRequestMessage_t;

typedef struct GameRoomChatMessage
{
    unsigned char messageType;
    unsigned int ownerID;
    char message[255];
}GameRoomChatMessage_t;

typedef struct UdpHeader
{
    unsigned int protocolID;
    unsigned int messageType;
    unsigned int clientID;
}UdpHeader_t;


typedef struct PositionUpdateMessage
{
    UdpHeader header;
    unsigned char animation;
    float x;
    float y;
    float z;
    float angleX;
    float angleY;
}PositionUpdateMessage_t;

typedef struct BulletInfo
{
    UdpHeader header;
    float x;
    float y;
    float z;
    float angleX;
    float angleY;
    int weapon;
}BulletInfo_t;

typedef struct LocalDiscoveryHeader
{
    char headerName[4];
    int headerID;
    char serverName[SERVER_NAME_SIZE];
}LocalDiscoveryHeader_t;


typedef struct GameRoomCreateAcceptMessage
{
    unsigned char messagetype;
    char roomName[30];
    char mapName[30];
    unsigned int roomID;
    int tcpPort;
    int udpServerPort;
    int udpClientPort;
    int TTFTPServerPort;
    unsigned char capacity;
    unsigned char gameType;
}GameRoomCreateAcceptMessage_t;

typedef struct GameRoomJoinRequestMessage
{
    unsigned char messageType;
    unsigned int uniqueId;
}GameRoomJoinRequestMessage_t;

typedef struct GameRoomClientJoinedMessage
{
    unsigned char messageType;
    unsigned int uniqueId;
	int kill;
	int death;
    char clientName[30];
    unsigned char team;
	unsigned char color;
}GameRoomClientJoinedMessage_t;

typedef struct GameRoomClientLeftMessage
{
    unsigned char messageType;
    unsigned int uniqueId;
    unsigned int newOwner;
}GameRoomClientLeftMessage_t;

typedef struct GameRoomClientReadyStatusMessage
{
    unsigned char messageType;
    unsigned int clientID;
}GameRoomClientReadyStatusMessage_t;

typedef struct GameRoomRemovedMessage
{
    unsigned char messageType;
    unsigned int roomID;
}GameRoomRemovedMessage_t;


typedef struct ConnectionRequestMessage
{
    unsigned char messageType;
    char playerName[30];
	unsigned char color;
}ConnectionRequestMessage_t;

typedef struct PlayerKilledMessage
{
    unsigned char messageType;
    unsigned int enemyID;
	int weaponType;
	float posX;
	float posY;
	float posZ;
}PlayerKilledMessage_t;

typedef struct PlayerKillInfoMessage
{
    unsigned char messageType;
    unsigned int player;
    unsigned int killer;
}PlayerKillInfoMessage_t;

typedef struct ItemTakeRequestMessage
{
    unsigned char messageType;
    int itemID;
}ItemTakeRequestMessage_t;


typedef struct PlayerResurrected
{
    unsigned char messageType;
    unsigned int playerID;
}PlayerResurrected_t;

typedef struct WeaponChangeNotificationMessage
{
    unsigned char messageType;
    int weaponType;
}WeaponChangeNotificationMessage_t;

typedef struct WeaponChangeMessage
{
    unsigned char messageType;
    unsigned int playerID;
    int weaponType;
}WeaponChangeMessage_t;

typedef struct ItemUsedNotificationMessage
{
    unsigned char messageType;
    unsigned int playerID;
    int itemType;
}ItemUsedNotificationMessage_t;

typedef struct ItemUsedMessage
{
    unsigned char messageType;
    int itemType;
}ItemUsedMessage_t;

typedef struct CreateItemMessage
{
	unsigned char messageType;
	int itemID;
	int itemType;
	float x;
	float y;
	float z;
}CreateItemMessage_t;

typedef struct PlayerScoreNotificationMessage
{
    unsigned char messageType;
    unsigned int playerID;
    int deathCount;
    int killCount;
}PlayerScoreNotificationMessage_t;

typedef struct GameRoomTeamChangeRequestMessage
{
	unsigned char messageType;
	unsigned char team;
}GameRoomTeamChangeRequestMessage_t;

typedef struct GameRoomTeamChangedMessage
{
	unsigned char messageType;
	unsigned char team;
	unsigned int playerID;
}GameRoomTeamChangedMessage_t;

typedef struct GameRoomColorChangeRequestMessage
{
	unsigned char messageType;
	unsigned char color;
}GameRoomColorChangeRequestMessage_t;

typedef struct GameRoomColorChangedMessage
{
	unsigned char messageType;
	unsigned int playerId;
	unsigned char color;
}GameRoomColorChangedMessage_t;

typedef BasicMessage_t GameRoomInformationRequestMessage_t;
typedef BasicMessage_t GameRoomConnectionAcceptMessage_t;
typedef BasicMessage_t GameRoomConnectionDeniedMessage_t;
typedef BasicMessage_t GameRoomClientNotReadyRequestMessage_t;
typedef BasicMessage_t GameRoomClientReadyRequestMessage_t;
typedef BasicMessage_t GameRoomClientGetClientsMessage_t;
typedef BasicMessage_t SpawnPointRequest_t;
typedef BasicMessage_t GameRoomMapFileRequestMessage_t;
typedef BasicMessage_t GameRoomMapFileRequestAcceptMessage_t;
typedef BasicMessage_t GameRoomMapFileStartDownloadMessage_t;
typedef BasicMessage_t GameRoomMapFileDownloadCompleteMessage_t;
typedef BasicMessage_t ResetPositionMessage_t;
typedef BasicMessage_t PlayerJoinedMessage_t;
typedef GameRoomJoinRequestMessage_t GameRoomClientKickRequestMessage_t;
typedef GameRoomJoinRequestMessage_t GameRoomClientKickedMessage_t;
typedef GameRoomJoinRequestMessage_t GameRoomConnectionRequestMessage_t;
typedef GameRoomChatRequestMessage_t GameRoomTeamChatRequestMessage_t;
typedef GameRoomChatMessage_t GameRoomTeamChatMessage_t;
typedef GameRoomClientJoinedMessage_t GameRoomClientInfoMessage_t;//==FIX
typedef GameRoomInformationMessage_t GameRoomStateChangedMessage_t;
typedef GameRoomJoinRequestMessage_t GameRoomOwnerMessage_t;
typedef GameRoomJoinRequestMessage_t GameRoomOwnerChangeMessage_t;
typedef BasicMessage_t GameRoomOwnerRequestMessage_t;
typedef BasicMessage_t GameStartRequestMessage_t;
typedef BasicMessage_t GameStartMessage_t;
typedef ItemTakeRequestMessage_t ItemTakeMessage_t;
typedef ItemTakeRequestMessage_t ItemAliveMessage_t;
typedef ItemTakeRequestMessage_t ItemNotAliveMessage_t;
typedef ItemTakeRequestMessage_t TeleporterUseRequestMessage_t;
typedef ItemTakeRequestMessage_t TeleporterUseAcceptMessage_t;
typedef ItemTakeRequestMessage_t TeleporterDeactivatedMessage_t;
typedef ItemTakeRequestMessage_t TeleporterActivatedMessage_t;
typedef ItemUsedMessage_t ItemExpiredMessage_t;
typedef ItemUsedNotificationMessage_t ItemExpiredNotificationMessage_t;



#endif	/* MESSAGESTRUCTURE_H */
