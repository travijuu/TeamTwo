/*
	Copyright (C) 2011-2012  Erkin Cakar

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

#ifndef TEAMGAMEROOMMENU_H
#define	TEAMGAMEROOMMENU_H

#include <iostream>
#include "Menu.h"
#include "Util/Converter.h"

namespace PLAYER_LIST_TYPE
{
    enum { ALLY_LIST=0, OPPONENT_LIST };
}

class TeamGameRoomMenu : Menu {
public:
  
    TeamGameRoomMenu(std::string name, Menu*, CEGUI::Window*,  LobbyCallbackInterface*);

    ~TeamGameRoomMenu();
    // To control the "enter" key
    // when it is pressed, chat message is sent
    bool keyUpEvent(const CEGUI::EventArgs&);
    // If player is host, this controls the kick button
    // if host click on a player on the list, it becomes enabled to kick
    // otherwise, kick button becomes disabled
    bool controlButtonEvent(const CEGUI::EventArgs&);
    // Adds coming chat message to the chatbox
    void addMessageToChat(std::string chatMessage);
    // Clears the chat editbox after sending chat message
    void clearChatEditbox();
    // Sets the title of the game room
    void setTitle(std::string title);
    // Set the ready state of the player given playerId
    void setReady(int playerId, bool ready);
    // Set player status as host, which means
    // this player has ability to start game, kick player, changed game properties
    void setHost(bool state);
    // This adds player to the list when connected
    void addPlayerToList(int listType, int playerId, std::string playerName, bool ready);
    // This removes the player in the player list
    // when the player is disconnected or kicked by host player
    void removePlayerFromList(int playerId);
    // Checks whether everyone is ready or not
    bool isEveryoneReady();
    void updateReady(int type);
    void setGameInfo(std::string gameName, std::string gameMap, std::string type, int gameCapacity);
    bool emptyEvent(const CEGUI::EventArgs& evt);
    void setMapImage(std::string mapName);

    /* START - These are overrided functions from Menu */
    // Handles with button clicks in this menu
    bool buttonClickEvent(const CEGUI::EventArgs&);
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
    int playerId, gameType;
    std::string chatMessage;
    bool readyRequest, hostStatus;
    Menu* parentMenu;
    std::map<int, std::pair<std::string,bool> > allyList;
    std::map<int, std::pair<std::string,bool> > opponentList;

    // This resets the menu GUI elements values and also overrided function
    void resetFields();
};

#endif	/* TEAMGAMEROOMMENU_H */

