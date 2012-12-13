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

#ifndef _GRAPHICSETTINGSMENU_H
#define	_GRAPHICSETTINGSMENU_H

#include <iostream>
#include "Menu.h"

/* Graphic Settings Menu
 * 
 * This class is responsible for creating, opening,
 * closing, handling mouse and keys of the Graphic
 * Settings Interface and also shadow and antialiasing
 * properties can be setted via this UI
 *
 */

class GraphicSettingsMenu : public Menu
{
public:
    // Constructor of the class, which takes 3 parameters to control
    GraphicSettingsMenu(std::string name, MenuControllerInterface* menuControllerInterface, SettingsCallbackInterface* settingsCallbackInterface);
    // Destructor of the class
    ~GraphicSettingsMenu();
  
    // returns whether shadow property is enable or not
    bool getShadow();
    // returns whether light property is enable or not
    bool getLight();
    // returns whether spot light property is enable or not
    bool getSpotLight();
    // sets shadow property as given boolean value
    void setShadow(bool value);
    // sets light property as given boolean value
    void setLight(bool value);
    // sets spotlight property as given boolean value
    void setSpotLight(bool value);

    /* START - These are overrided functions from Menu */
    // Handles with button clicks in this menu
    bool buttonClickEvent(const CEGUI::EventArgs &evt);
    //
    bool checkboxEvent(const CEGUI::EventArgs &evt);
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
    bool shadow, light, spotLight;
    
    // This resets the menu GUI elements values and also overrided function
    void resetFields();
};

#endif	/* _GRAPHICSETTINGSMENU_H */

