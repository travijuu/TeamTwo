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

#ifndef GAMEMESSAGEINTERFACE_H
#define GAMEMESSAGEINTERFACE_H
#include "stdafx.h"
#include "NetworkUtil/MessageStructure.h"
#include "GameRoom.h"
#include "GameClient.h"

/* GameMessageInterface

   This interface is used by the GameMessageController for
   providing publish-subscribe design pattern. This interface
   is implemented by the subscriber class for enabling the
   callback system.


*/

class GameMessageInterface
{
public:
	//called when a bullet information message is received
    virtual void addBullet(BulletInfo_t* info) = 0;
	//called when a transform update of an other player has been received
    virtual void scheduleTransformUpdate(PositionUpdateMessage_t* message) = 0;
	//called when the localdiscovery operation has been finished (timeout)
    virtual void localDiscoveryFinalized(std::map<std::string,std::string> serverList) = 0;
	//called when an item has been taken.
    virtual void itemTakeEvent(int itemID) = 0;
	//called when the status of an item has been changed. it indicates whether
	//it is activated or deactivated
    virtual void itemStatusChanged(int itemID, bool alive) = 0;
	//called when a player has been killed. it also passes the killer's id
    virtual void playerKilledEvent(int playerID, int killerID) = 0;
	//called when a player with the given id has been resurrected
    virtual void playerResurrectedEvent(int playerID) = 0;
	//called when the next spawn point has been received
    virtual void nextSpawnPointEvent(double x, double y, double z) = 0;
	//called when a player with the given id changes his/her weapon
    virtual void playerWeaponSwitchEvent(int playerID, int weaponType) = 0;
	//called when the game room requests to reset the position of the player
    virtual void resetPositionEvent() = 0;
	//called when a special item usage has expired
    virtual void itemExpiredEvent(int itemID) = 0;
	//called when a player with the given id has used an item ith the given id
    virtual void playerSpecialItemUsedEvent(unsigned int playerID,int itemID)=0;
	//called when a player with the given id is no more using a special item 
    virtual void playerSpecialItemExpiredEvent(unsigned int playerID,int itemID)=0;
	//called when the score of a client has been changed
    virtual void playerScoreUpdated(GameClient* client)=0;
	//called when the team of a player has been changed
	virtual void playerTeamChanged(GameClient* client)=0;
	//called when the color of a player has been changed
	virtual void playerColorChanged(GameClient* client)=0;
	//called when the requested teleporter usage with the given teleporter id has been accepted
    virtual void teleporterUsageAccepted(int teleporterID)=0;
	//called when the teleporter when the given teleporter has been activated
    virtual void teleporterActivated(int teleportedID)=0;
	//called when the teleporter when the given teleporter has been deactivated
    virtual void teleporterDeactivated(int teleporterID)=0;
	//called when the server connection has been established
    virtual void connectionAccepted() = 0;
	//called when the server connection has been failed
	virtual void connectionFailed()=0;
	//called when a game room information has been received
    virtual void gameRoomInfoReceived(GameRoom* gameRoom) = 0;
	//called when the game room that as requested before has been accepted
    virtual void gameRoomCreated() = 0;
	//called when the local client has joined the room
    virtual void gameRoomConnectionEstablished() = 0;
	//called when the local client has left the room
	virtual void gameRoomConnectionTerminated()=0;
	//called when the game room join request of the local client has been denied
	virtual void gameRoomConnectionDenied()=0;
	//called when a client with the given object has joined the game room
	virtual void gameRoomClientJoined(GameClient* client) = 0;
	//called when a client with the given object has left the game room
    virtual void gameRoomClientLeft(GameClient* client) = 0;
	//called when a client with the given object has changed his/her ready status
    virtual void gameRoomClientReadyStatusChanged(GameClient* client) = 0;
	//called when a message received from a client
    virtual void gameRoomClientMessageReceived(GameClient* client, std::string message, bool teamChat) = 0;
	//called when a client has changed his/her nickname
    virtual void gameRoomClientNickChanged(GameClient* client, std::string oldNick) = 0;
	//called when a gameroom on the server has been removed/deleted
    virtual void gameRoomRemoved(unsigned int id) = 0;
	//called when a client has been kicked from the game room
    virtual void gameRoomClientKicked(GameClient* client)=0;
	//called when the local client has been kicked
    virtual void gameRoomLocalClientKicked()=0;
	//called when the owner of the current game room has been changed
    virtual void gameRoomOwnerChanged(GameClient* client) = 0;
	//called when the game is going to be started
    virtual void gameStarting() = 0;
	//called when the state (player count) of the current game room has been changed
    virtual void gameRoomStateChanged(GameRoom *room)=0;




    virtual void itemExpiredNotification(int playerID, int itemID){}//=========================
    virtual void itemCreateEvent(int itemID, int itemType, float posX, float posY, float posZ)
    {
        std::cout << "new item created!\n";
        std::cout << "id: " << itemID << std::endl;
        std::cout << "x: " << posX << std::endl;
        std::cout << "y: " << posY << std::endl;
        std::cout << "z: " << posZ << std::endl;
    }
};
 


#endif