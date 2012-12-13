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

#ifndef _CREATEGAMEMENU_H
#define	_CREATEGAMEMENU_H

#include "Menu.h"
#include "GameRoomMenu.h"

/*
 * This class represents a menu which
 * helps to create new game 
 *
 */
class CreateGameMenu : public Menu {
public:
    /*!
     * \brief
     * Constructor of the CreateGameMenu(std::string name, Menu* menu, CEGUI::Window* sheet, LobbyCallbackInterface* lobbyCallbackInterface)
     * \param name - Menu name
     * \param Menu* menu - Belonging menu
     * \param CEGUI::Window* sheet - which sheet is used for this menu
     * \param LobbyCallbackInterface* lobbyCallbackInterface - This helps to call related callback functions
     * \return
     * a boolean value
     */
    CreateGameMenu(std::string name, Menu* menu, CEGUI::Window* sheet, LobbyCallbackInterface* lobbyCallbackInterface);
    ~CreateGameMenu();

    /*!
     * \brief 
     * Handles with button clicks in this menu
     * \Parameter
     * CEGUI::EventArgs &evt - keeps event info
     * \return
     * a boolean value
     */
    bool buttonClickEvent(const CEGUI::EventArgs &evt);
    /*!
     * \brief
     * Creates GUI of the menu
     * \Parameter
     * no parameter
     * \return
     * no value
     */
    void createMenu();
    /*!
     * \brief
     * Shows the menu on current sheet
     * \Parameter
     * no parameter
     * \return
     * no value
     */
    void showMenu();
    /*!
     * \brief
     * Hides the menu
     * \Parameter
     * no parameter
     * \return
     * no value
     */
    void closeMenu();
    /*!
     * \brief
     * This is used for getting the sheet belonging to this menu
     * \Parameter
     * no parameter
     * \return
     * a sheet belongs to this menu
     */
    CEGUI::Window* getSheet();
private:
  
    GameRoomMenu* gameRoomMenu;
    Menu* parentMenu;
    
    std::string gameMap, gameName;
    double gameCapacity;

    std::map<int, std::string> gameMapList;
    std::map<int, std::string> gameTypeList;
    /*!
     * \brief
     * This resets the menu GUI elements values
     * \Parameter
     * no parameter
     * \return
     * no return
     */
    void resetFields();
};

#endif	/* _CREATEGAMEMENU_H */

