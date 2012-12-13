/*
	Copyright (C) 2011-2012 Erkin Cakar

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

#ifndef LOBBYCALLBACKINTERFACE_H
#define	LOBBYCALLBACKINTERFACE_H

/* Lobbdy Callback Interface
 *
 * This interface includes functions related to
 * lobby part of the game.
 * 
 */

class LobbyCallbackInterface {
public:
    // This sends a request to server for connection with given IP address
    virtual void lobbyServerConnectEvent(std::string ipAddress) = 0;
    // This sends a request to server to enable
    // Local Discover Protocol (LDP) to find available
    // servers in the network
    virtual void lobbyEnableLocalServerDiscoveryEvent() = 0;
    // This sends a request to server to refresh the available server list
    virtual void lobbyRefreshServerListEvent() = 0;
    // This sends a request to server to exit from the connected server
    virtual void lobbyExitEvent() = 0;

    // This sends a request to server to connect
    // selected game room
    virtual void lobbyGameRoomConnectEvent(int gameId) = 0;
    // This sends a request to server to create
    // new game room according to given game spesifications
    virtual void lobbyGameRoomCreateEvent(std::string gameName, std::string gameMap,unsigned char type, int gameCapacity) = 0;
    // This sends a request to refresh available game rooms
    // in the server and gets the available game room list
    virtual void lobbyGameRoomRefreshEvent() = 0;
    // When player sends chat message in game room
    // this function informs server to process
    virtual void lobbyGameRoomSendMessageEvent(std::string message) = 0;
    // This indicates that player hits ready button
    // and server is informed whether player is ready or not
    virtual void lobbyGameRoomReadyStatusChangeEvent(bool ready) = 0;
    // This indicates that given player is kicked by host
    // and server is informed to do this
    virtual void lobbyGameRoomKickPlayer(int playerId) = 0;
    // This indicates that the player exited from game room
    // then server is informed
    virtual void lobbyGameRoomExitEvent() = 0 ;
    // This indicates that the host hitted the start game button
    // so server is informed for starting game
    virtual void lobbyGameRoomStartEvent() = 0;
    // This indicates that when selected map in the game room
    // is changed by host, it informs the server to change the map
    virtual void lobbyGameRoomMapChangedEvent() = 0;
    // 
    virtual void lobbyGameRoomTeamChangedEvent() = 0;
};


#endif	/* LOBBYCALLBACKINTERFACE_H */

