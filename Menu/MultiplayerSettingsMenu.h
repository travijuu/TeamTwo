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

#ifndef _MULTIPLAYERSETTINGSMENU_H
#define	_MULTIPLAYERSETTINGSMENU_H

#include "Menu.h"

/* Multiplayer Settings Menu
 *
 * This class is responsible for creating, opening,
 * closing, handling mouse and keys of the multiplayer
 * menu interfaces and also enabling player to change
 * its nickname and the character type
 *
 */


class MultiplayerSettingsMenu : public Menu {
public:
    // Constructor of the class, which takes 3 parameters to control
    MultiplayerSettingsMenu(std::string name, MenuControllerInterface* menuControllerInterface, SettingsCallbackInterface* settingsCallbackInterface);
    // Destructor of the class
    ~MultiplayerSettingsMenu();
    // Returns the player's nickname
    std::string getPlayerName();
    // Returns the mouse sensivity value
    int getMouseSensivity();
    //
    int getColor();
    // Sets the player's nickname
    void setPlayerName(std::string value);
    // Sets the mouse sensivity value 
    void setMouseSensivity(float value);
    //
    void setColor(int value);
    //
    bool comboboxChangedEvent(const CEGUI::EventArgs &evt);


    /* START - These are overrided functions from Menu */
    // Handles with button clicks in this menu
    bool buttonClickEvent(const CEGUI::EventArgs &evt);
    // Creates GUI of the menu
    void createMenu();
    // Shows the menu on current sheet
    void showMenu();
    // Hides the menu
    void closeMenu();
    /* END - These are overrided functions from Menu */
	CEGUI::Window* getSheet(){return 0;}
private:    
    std::string playerName;
    float mouseSensivity;
    int color;

    
    // This resets the menu GUI elements values and also overrided function
    void resetFields();
};

#endif	/* _MULTIPLAYERSETTINGSMENU_H */

