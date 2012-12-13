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

#ifndef _CREDITSMENU_H
#define	_CREDITSMENU_H

#include "Menu.h" 

/*
 * This class is reponsible of creating, opening,
 * closing of the Credits Panel which contains
 * some information of the game developers
 * 
 */


class CreditsMenu : public Menu {
public:
    // Constuctor of the CreditsMenu, it takes 3 parameters to control the class
    CreditsMenu(std::string name, MenuControllerInterface* menuControllerInterface, LobbyCallbackInterface* lobbyCallbackInterface);
    // Destructor of the class
    ~CreditsMenu();
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
private:
    // This resets the menu GUI elements values
    void resetFields();   
};

#endif	/* _CREDITSMENU_H */

