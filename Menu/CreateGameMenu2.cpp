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

#include "CreateGameMenu.h"
#include "LobbyMenu.h"
#include "GameRoomMenu.h"


CreateGameMenu::CreateGameMenu(std::string name, Menu* menu, CEGUI::Window* sheet,  LobbyCallbackInterface* lobbyCallbackInterface)
{
    this->name = name; 
    this->wManager = &CEGUI::WindowManager::getSingleton();
    this->wManager->destroyWindow(this->name+"Sheet");
    this->sheet = sheet;
    parentMenu = menu;
    this->lobbyCallbackInterface = lobbyCallbackInterface;

    gameName = "My Game";
    gameCapacity = 5;
    createMenu();
    
}

CreateGameMenu::~CreateGameMenu()
{
    delete &gameMapList;
    delete &gameTypeList;
}

bool CreateGameMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str();

     if(actionName == name+"/backButton")
     {
         closeMenu();
     }
     else if (actionName == name+"/createButton")
     {
         std::string gameName = wManager->getWindow(name+"/gameEditbox")->getText().c_str();
         if(gameName=="")
             return true;//no name specified
         gameName=gameName.substr(0,30);
         std::cout<<"name: "<<gameName<<std::endl;
         std::string gameMap = wManager->getWindow(name+"/mapCombobox")->getText().c_str();
         std::string type = wManager->getWindow(name+"/typeCombobox")->getText().c_str();
         
         int gameCapacity = ((CEGUI::Spinner*)wManager->getWindow(name+"/sizeSpinner"))->getCurrentValue();
         
         this->lobbyCallbackInterface->lobbyGameRoomCreateEvent(gameName,gameMap,(unsigned char)(type=="Death-Match"?0:1),gameCapacity);

         //gameRoomMenu = ((LobbyMenu*)parentMenu)->getGameRoomMenu();
         
         if(type=="Death-Match")
         { 
             ((LobbyMenu*)parentMenu)->getGameRoomMenu()->setHost(true);
             ((LobbyMenu*)parentMenu)->getGameRoomMenu()->setGameInfo(gameName,gameMap,type,gameCapacity);
             ((LobbyMenu*)parentMenu)->getGameRoomMenu()->showMenu();
         }
         else
         {
            ((LobbyMenu*)parentMenu)->getTeamGameRoomMenu()->setHost(true);
            ((LobbyMenu*)parentMenu)->getTeamGameRoomMenu()->setGameInfo(gameName,gameMap,type,gameCapacity);
            ((LobbyMenu*)parentMenu)->getTeamGameRoomMenu()->showMenu();
         }

 
         closeMenu();
     }

     resetFields();
}
void CreateGameMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}
void CreateGameMenu::createMenu()
{
    CEGUI::Window* menuLayout = wManager->loadWindowLayout("layouts/CreateGameRoom.layout");
    wManager->getWindow(name)->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow(name)->setVisible(false);
    wManager->getWindow(name)->setAlwaysOnTop(true);
    wManager->getWindow(name+"/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CreateGameMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/createButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CreateGameMenu::buttonClickEvent,this));
    
    ((CEGUI::Combobox*)wManager->getWindow(name+"/typeCombobox"))->addItem(new CEGUI::ListboxTextItem("Death-Match"));
    ((CEGUI::Combobox*)wManager->getWindow(name+"/typeCombobox"))->addItem(new CEGUI::ListboxTextItem("Team Match"));
    ((CEGUI::Combobox*)wManager->getWindow(name+"/typeCombobox"))->setReadOnly(true);
    ((CEGUI::Combobox*)wManager->getWindow(name+"/typeCombobox"))->setItemSelectState((size_t)0,true);

    sheet->addChildWindow(menuLayout);
    
    resetFields();
}
void CreateGameMenu::showMenu()
{
    wManager->getWindow(name)->setVisible(true);
}
void CreateGameMenu::resetFields()
{
    wManager->getWindow(name+"/gameEditbox")->setText(gameName);
   // wManager->getWindow(name+"/mapCombobox")->setText(gameMap);
    ((CEGUI::Spinner*)wManager->getWindow(name+"/sizeSpinner"))->setCurrentValue(gameCapacity);
}

CEGUI::Window* CreateGameMenu::getSheet()
{
    return sheet;
}
 