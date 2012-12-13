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

#include "CreditsMenu.h"

#include "CreateGameMenu.h"


CreditsMenu::CreditsMenu(std::string name, MenuControllerInterface* menuControllerInterface, LobbyCallbackInterface* lobbyCallbackInterface)
{
    this->name = name;
    this->menuControllerInterface = menuControllerInterface;
    this->lobbyCallbackInterface = lobbyCallbackInterface;
    wManager = &CEGUI::WindowManager::getSingleton();
    sheet = wManager->createWindow("DefaultWindow",this->name+"Sheet");
 
    createMenu();
}

CreditsMenu::~CreditsMenu()
{

}

bool CreditsMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str(); 

     if(actionName == "Credits/backButton")
     {
         menuControllerInterface->back();
     }
	return true;
}

void CreditsMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}
void CreditsMenu::createMenu()
{
    CEGUI::Window* menuLayout = wManager->loadWindowLayout("layouts/Credits.layout");
    wManager->getWindow("Credits")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.35f, 0)));
    wManager->getWindow("Credits/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CreditsMenu::buttonClickEvent,this));
    sheet->addChildWindow(menuLayout);
}
void CreditsMenu::showMenu()
{
    wManager->getWindow(name)->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);
}

void CreditsMenu::resetFields()
{

}
CEGUI::Window* CreditsMenu::getSheet()
{
    return NULL;
}