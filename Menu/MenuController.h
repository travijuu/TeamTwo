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

#ifndef _MENUCONTROLLER_H
#define	_MENUCONTROLLER_H
#include <CEGUI/CEGUI.h>

#include "Menu.h"
#include "MenuControllerInterface.h"
#include "MenuActionCallbackInterface.h"
//#include "SoundManager.h"
#include <OgreAL/OgreAL.h>
#include <OgreAL/OgreALListener.h>
//#include "SoundController.h"

/* Menu Controller
 *
 * This class is responsible for creating, adding, opening,
 * closing, handling mouse event of the menu
 * and also giving messagebox to player
 *
 */
 
class MenuController : public MenuControllerInterface {
public:
    // Constuctor of the class, which takes the name of menu
    MenuController( std::string menuName );
    // Destructor of the class
    ~MenuController();
    //
    void addOption( std::string optionName );
    //
    void addOption( std::string optionName, Menu *menu );
    //
    void addSubOption( std::string subOptionName, std::string parentOptionName, Menu* menu );
    //
    void addSubOption( std::string subOptionName, std::string parentOptionName );
 
    // Handles with button clicks in this menu
    bool buttonClicked(const CEGUI::EventArgs& evt);
    // First clearMenu() and
    // shows the given menu
    void showMenu( std::string menu );
    // First clears the menu and
    // shows the main menu
    void showMain();
    //
    bool backMenu(const CEGUI::EventArgs &e);
    // Sets the visibility of the all menus
    void clearMenu();
    // Takes message string and the instance of a menu in which menu will be shown
    // This is basic information box like popup about the game
    void showMessage(std::string message);
    //
    void showLoading();
    //
    void hideLoading();


    // This is triggered when the player wants to
    // pass one level upper on the menu
    virtual void back() ;
private:
    CEGUI::Window* sheet;
    CEGUI::WindowManager *wManager;
    CEGUI::Window* item;
    CEGUI::Window* informationPanel; 
    int optionNum;
    std::string menuName;
    MenuActionCallbackInterface *callbackObject;
    std::vector<std::string> optionVector;
    std::map<std::string, std::string> subOptionMap;
    std::map<std::string, Menu*> menuObjectMap;
    std::string currentMenu;
   OgreAL::SoundManager* soundManager;
   //SoundController *soundController;
};

#endif	/* _MENUCONTROLLER_H */

