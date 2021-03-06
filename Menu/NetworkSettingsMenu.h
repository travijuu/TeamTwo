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

#ifndef _NETWORKSETTINGSMENU_H
#define	_NETWORKSETTINGSMENU_H

#include <iostream>
#include "Menu.h"

/* Network Settings Menu
 * 
 * This class is responsible for creating, opening,
 * closing, handling mouse and keys of the multiplayer
 * menu interfaces and also enabling player to change
 * its TCP and TTFTP port numbers
 *
 */

class NetworkSettingsMenu : public Menu
{
public:
    // Constructor of the class, which takes 3 parameters to control
    NetworkSettingsMenu(std::string name, MenuControllerInterface *menuControllerInterface, SettingsCallbackInterface* settingsCallbackInterface);
    // Destructor of the class
    ~NetworkSettingsMenu();
    // Returns TTFTP port number
    std::string getTTFTP();
    // Returns TCP port number of server
    std::string getServerPort();
    // Returns IP address
    std::string getIPAddress();
    // Sets the TTFTP port number
    void setTTFTP(std::string value);
    // Sets the server TCP port number
    void setServerPort(std::string value);
    // Sets the server IP address
    void setIPAddress(std::string value);

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
    std::string serverPort, TTFTPPort, ipAddress;

    // This resets the menu GUI elements values and also overrided function
    void resetFields();

};

#endif	/* _NETWORKSETTINGSMENU_H */

