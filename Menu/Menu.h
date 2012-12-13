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

#ifndef _MENU_H
#define	_MENU_H

#include <CEGUI/CEGUI.h>
#include "MenuControllerInterface.h"
#include "LobbyCallbackInterface.h"
#include "SettingsCallbackInterface.h"

/* Menu
 *
 * This is the base class of all other menu classes
 * 
 */

class Menu
{
protected:
    std::string name;
    MenuControllerInterface *menuControllerInterface;
    LobbyCallbackInterface  *lobbyCallbackInterface;
    SettingsCallbackInterface *settingsCallbackInterface;
    CEGUI::WindowManager* wManager;
    CEGUI::Window* sheet;
public:
    // Constructor of the class, which takes name and controller object for callbacks
    Menu(std::string name, MenuControllerInterface* menuControllerInterface);
    // Default constructor
    Menu();
    // Destructor of the class
    ~Menu();
    // Returns the menu name
    std::string getName();
    // Handles with button clicks in this menu
    virtual bool buttonClickEvent(const CEGUI::EventArgs &evt);
    // Shows the menu on current sheet
    virtual void showMenu() = 0;
    // Hides the menu
    virtual void closeMenu() = 0;
    // Creates GUI of the menu
    virtual void createMenu() = 0;
    // This resets the menu GUI elements values
    virtual void resetFields() = 0;
    // This is used for getting the sheet belonging to this menu
    virtual CEGUI::Window* getSheet() = 0;
};

#endif	/* _MENU_H */

