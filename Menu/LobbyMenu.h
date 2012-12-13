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

#ifndef _LOBBYMENU_H
#define	_LOBBYMENU_H

#include <iostream>
#include "Menu.h"
#include "CreateGameMenu.h"
#include "TeamGameRoomMenu.h"

/* Lobby Menu
 *
 * This class is responsible for creating, opening,
 * closing, handling mouse and keys of the lobby
 * Interface and also handling with available server
 * and game lists.
 * 
 */


class LobbyMenu : public Menu
{
public:
        // Constructor of the class, which takes 3 parameters to control
        LobbyMenu(std::string name, MenuControllerInterface* menuControllerInterface, LobbyCallbackInterface* lobbyCallbackInterface);
        // Destructor of the class
        ~LobbyMenu();
        // Returns room name which is connected
	std::string getRoomName();
        // If an item is clicked in the list, connect button becomes enabled
        // Otherwise, connect button becomes disabled
        bool controlButtonEvent(const CEGUI::EventArgs &evt);
        // Adds created new game to the available games list
        void addGameToList(int gameID, std::string gameName, std::string mapName, std::string type, std::string gameCapacity);
        // Removes given game from available games list
        void removeGameFromList(int gameId);
        // Adds new server to the available servers list
        void addServerToList(int serverId, std::string serverName, std::string serverIP);
        // Removes given server from available servers list
        void removeServerFromList(int serverId);
        // Removes all available games list
        void removeAllGames();
        // Removes all available servers list
        void removeAllServers();
        // Shows the lobby menu
        void showLobby();
        // Hides the lobby menu
        void hideLobby();
        // Returns an object from GameRoomMenu
        GameRoomMenu* getGameRoomMenu();
        // Returns an object from TeamGameRoomMenu
        TeamGameRoomMenu *getTeamGameRoomMenu();
        // Sets the IP address of which server to connect
        void setIPAddress(std::string ipAddress);
        // Modifies the server information in the available server list
        void modifyServerEntry(int serverId, std::string serverName, std::string serverIP);
        // Modifies the game information in the available game list
        void modifyGameEntry(int gameID, std::string gameName, std::string mapName, std::string type, std::string gameCapacity);
        // Adds new map to the list
        void addMap(std::string mapName, std::string path);
        // Removes given map from the list
        void removeMap(std::string mapName);
        // Returns a map contaning game map list
        std::map<std::string,std::string> getMaps();
        // Sets the title of lobby of connected server
        void setTitle(std::string title);
		//
		void hideConnectionMenu();

        /* START - These are overrided functions from Menu */
        // Handles with button clicks in this menu
        bool buttonClickEvent(const CEGUI::EventArgs &evt);
        // Creates GUI of the menu
        void createMenu();
        // Shows the menu on current sheet
        void showMenu();
        // Hides the menu
        void closeMenu();
        // This is used for getting the sheet belonging to this menu
        CEGUI::Window* getSheet();
        /* END - These are overrided functions from Menu */
private:
        CreateGameMenu* createGameMenu;
        GameRoomMenu* gameRoomMenu;
        TeamGameRoomMenu* teamGameRoomMenu;
        std::string roomName, currentMenu, ipAddress;
        std::vector<std::string> subMenuVector;
        int current;
        int serverId, gameId;
        std::map<int, int> gameList;
        std::map<int, int> serverList;
        std::map<std::string, std::string> mapList;
		std::map<int, CEGUI::ListboxTextItem*> gameList2;

        // This resets the menu GUI elements values and also overrided function
        void resetFields();
        // Sets the visibility to false of the sub menus 
        void subMenuCleaner();
};

#endif	/* _LOBBYMENU_H */

