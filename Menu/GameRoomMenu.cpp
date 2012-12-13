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

#include "GameRoomMenu.h"
#include "LobbyMenu.h"
#include "Util/Converter.h"


GameRoomMenu::GameRoomMenu(std::string name, Menu* menu, CEGUI::Window* sheet,  LobbyCallbackInterface* lobbyCallbackInterface)
{
    this->name = name;
    this->parentMenu = menu;
    this->lobbyCallbackInterface = lobbyCallbackInterface;
    this->wManager = &CEGUI::WindowManager::getSingleton();
    wManager->destroyWindow(name+"Sheet");
    this->sheet = this->wManager->createWindow("DefaultWindow",this->name+"Sheet");


    readyRequest = false;
    int playerId = -1;

    createMenu();
    
}

GameRoomMenu::~GameRoomMenu()
{
    delete &playerList;
}

bool GameRoomMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str();
     if(actionName == name+"/backButton")
     {
         resetFields();
		 ((LobbyMenu*)parentMenu)->removeAllGames();
         closeMenu();
         parentMenu->showMenu();
         this->lobbyCallbackInterface->lobbyGameRoomExitEvent();
     }
     else if(actionName == name+"/sendButton" )
     {
         std::string message = wManager->getWindow(name+"/chatEditbox")->getText().c_str();
         if(message != "")
         {
             this->lobbyCallbackInterface->lobbyGameRoomSendMessageEvent(message);
             wManager->getWindow(name+"/chatEditbox")->setText("");
         }
     }
     else if(actionName == name+"/readyButton")
     {
         readyRequest = !readyRequest;
         this->lobbyCallbackInterface->lobbyGameRoomReadyStatusChangeEvent(readyRequest);
  
         if(!readyRequest)
             wManager->getWindow(name+"/readyButton")->setText("Ready To Play");
         else
             wManager->getWindow(name+"/readyButton")->setText("Not Ready");
     }
     else if(actionName == name+"/kickButton")
     {
         if(((CEGUI::Listbox*) wManager->getWindow(name+"/playerListbox"))->getFirstSelectedItem() == NULL)
             return false;
         std::string playerName = ((CEGUI::Listbox*) wManager->getWindow(name+"/playerListbox"))->getFirstSelectedItem()->getText().c_str();

         std::map<int,std::pair<std::string,bool> >::iterator itr;

         for(itr=playerList.begin();itr!=playerList.end();itr++)
           if(itr->second.first == playerName)
              playerId = itr->first;

         this->lobbyCallbackInterface->lobbyGameRoomKickPlayer(playerId);

     }
     else if(actionName == name+"/startButton")
         this->lobbyCallbackInterface->lobbyGameRoomStartEvent();

     std::cout<<actionName<<std::endl;
 
     return true;
}
bool GameRoomMenu::keyUpEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::KeyEventArgs& keyEventArgs = (CEGUI::KeyEventArgs&)evt;
    if(keyEventArgs.scancode == 28)
    {
        std::string message = wManager->getWindow(name+"/chatEditbox")->getText().c_str();
        if(message != "")
         {
            this->lobbyCallbackInterface->lobbyGameRoomSendMessageEvent(message);
			wManager->getWindow(name+"/chatEditbox")->setText("");
            //addMessageToChat(message);
        }
    }
    return true;
}
bool GameRoomMenu::controlButtonEvent(const CEGUI::EventArgs& evt)
{
    if(!hostStatus)
        return true;

     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;

     CEGUI::Listbox* listbox = (CEGUI::Listbox*) wManager->getWindow(name+"/playerListbox");
    
     if(listbox->getFirstSelectedItem() != NULL)
         wManager->getWindow(name+"/kickButton")->setEnabled(true);
     else
     {

         updateReady();
         wManager->getWindow(name+"/kickButton")->setEnabled(false);
     }
     return true;
}

bool GameRoomMenu::emptyEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
    ((CEGUI::Listbox*) wManager->getWindow(name+"/playerListbox"))->clearAllSelections();
    return true;
}
void GameRoomMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}
void GameRoomMenu::createMenu()
{
    wManager->destroyWindow(name);
    
    CEGUI::Window* menuLayout = wManager->loadWindowLayout("layouts/GameRoomDM.layout");
    wManager->getWindow(name)->setVisible(false);
    wManager->getWindow(name+"/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/sendButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/readyButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/kickButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/kickButton")->setEnabled(false);
    wManager->getWindow(name+"/chatEditbox")->subscribeEvent(CEGUI::Editbox::EventKeyUp, CEGUI::Event::Subscriber(&GameRoomMenu::keyUpEvent,this));
    ((CEGUI::Editbox*)wManager->getWindow(name+"/chatEditbox"))->setMaxTextLength(36);
    wManager->getWindow(name+"/startButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/playerListbox")->subscribeEvent(CEGUI::Listbox::EventMouseClick, CEGUI::Event::Subscriber(&GameRoomMenu::controlButtonEvent,this));
    wManager->getWindow(name+"/playerListbox")->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick, CEGUI::Event::Subscriber(&GameRoomMenu::emptyEvent,this));
    wManager->getWindow(name+"/playerListbox")->subscribeEvent(CEGUI::Listbox::EventMouseTripleClick, CEGUI::Event::Subscriber(&GameRoomMenu::emptyEvent,this));
    wManager->getWindow(name+"/playerListbox")->setEnabled(false);

    sheet->addChildWindow(menuLayout);


}
void GameRoomMenu::showMenu()
{
	resetFields();
    wManager->getWindow(name)->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);
}
void GameRoomMenu::setTitle(std::string title)
{
    wManager->getWindow(name)->setText(title);
}

void GameRoomMenu::setGameInfo(std::string gameName, std::string gameMap, std::string type, int gameCapacity)
{
    std::string info = "Game Name: ";
    info.append(gameName).append("\nGame Map: ").append(gameMap).append("\nGame Type: ").append(type).append("\nCapacity: ").append(Converter::intToString(gameCapacity));
    
    wManager->getWindow(name+"/gameInfoText")->setText(info);
}

void GameRoomMenu::resetFields()
{
    wManager->getWindow(name+"/chatArea")->setText("");
    wManager->getWindow(name+"/chatEditbox")->setText("");
    wManager->getWindow(name+"/gameInfoText")->setText("");
    wManager->getWindow(name+"/readyButton")->setText("Ready To Play");
    readyRequest = false;
    ((CEGUI::Listbox*)wManager->getWindow(name+"/playerListbox"))->resetList();
}
void GameRoomMenu::addPlayerToList(int listType, int playerId, std::string playerName, bool ready)
{
    CEGUI::ListboxItem* listboxItem = new CEGUI::ListboxTextItem(playerName);
    listboxItem->setSelectionBrushImage(&CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ));
    ((CEGUI::Listbox*)wManager->getWindow(name+"/playerListbox"))->addItem(listboxItem);
    std::pair<std::string, bool>playerPair(playerName,ready);
    playerList[playerId] = playerPair;
    setReady(playerId,ready);
 
}
void GameRoomMenu::removePlayerFromList(int playerId)
{
	std::cout<<"removing player from the list\n";
    std::map<int, std::pair<std::string, bool> >::iterator itr;
    for(itr=playerList.begin();itr!=playerList.end();itr++)
    {
		bool found=false;
        if(itr->first == playerId)
        {
            CEGUI::Listbox* listbox = (CEGUI::Listbox*) wManager->getWindow(name+"/playerListbox");
            for(int i =0;i<listbox->getItemCount();i++)
            {
                if( listbox->getListboxItemFromIndex(i)->getText() == itr->second.first )
                {
                    listbox->removeItem(listbox->getListboxItemFromIndex(i));
                    wManager->getWindow(name+"/kickButton")->setEnabled(false);
                    playerList.erase(playerId);
					found=true;
                 }
            }
			if(found)
				break;
        }
    }

    updateReady();
    if(isEveryoneReady())
        wManager->getWindow(name+"/startButton")->setEnabled(true);
	std::cout<<"ok\n";
}
void GameRoomMenu::addMessageToChat(std::string chatMessage)
{
    if(chatMessage != "")
    {
        wManager->getWindow(name+"/chatArea")->appendText(chatMessage+"\n");
        //wManager->getWindow(name+"/chatEditbox")->setText("");
        
        ((CEGUI::MultiLineEditbox*)wManager->getWindow(name+"/chatArea"))->setCaratIndex(wManager->getWindow(name+"/chatArea")->getText().length());
    }
}
void GameRoomMenu::addSystemMessageToChat(std::string chatMessage)
{
    if(chatMessage != "")
    {
        wManager->getWindow(name+"/chatArea")->appendText("***"+chatMessage+"***\n");

        ((CEGUI::MultiLineEditbox*)wManager->getWindow(name+"/chatArea"))->setCaratIndex(wManager->getWindow(name+"/chatArea")->getText().length());
    }
}
void GameRoomMenu::setReady(int playerId, bool ready)
{
    std::pair<std::string, bool> playerPair = playerList[playerId];
    playerPair.second = ready;
    playerList[playerId] = playerPair;
    CEGUI::Listbox* listbox = (CEGUI::Listbox*)wManager->getWindow(name+"/playerListbox");

    for(int i=0;i<listbox->getItemCount();i++)
        if( listbox->getListboxItemFromIndex(i)->getText().c_str() == playerPair.first )
            listbox->setItemSelectState(listbox->getListboxItemFromIndex(i),ready);
    
    if(isEveryoneReady())
        wManager->getWindow("GameRoom/startButton")->setEnabled(true);
}

void GameRoomMenu::setHost(bool state)
{
    hostStatus = state;

    if(!hostStatus)
    {
        wManager->getWindow("GameRoom/startButton")->setEnabled(false);
        wManager->getWindow("GameRoom/startButton")->setVisible(false);
        wManager->getWindow("GameRoom/playerListbox")->setEnabled(false);
        wManager->getWindow("GameRoom/readyButton")->setEnabled(true);
		wManager->getWindow("GameRoom/readyButton")->setVisible(true);
    }
    else
    {
        wManager->getWindow("GameRoom/readyButton")->setVisible(false);
        wManager->getWindow("GameRoom/startButton")->setVisible(true);
        wManager->getWindow("GameRoom/playerListbox")->setEnabled(true);
        wManager->getWindow("GameRoom/startButton")->setEnabled(true);
      /*  if(isEveryoneReady())
        {
            wManager->getWindow("GameRoom/startButton")->setEnabled(true);
        }*/
    }
}

bool GameRoomMenu::isEveryoneReady()
{
    std::map<int, std::pair<std::string,bool> >::iterator itr;
    for(itr=playerList.begin();itr!=playerList.end();itr++)
        if( itr->second.second == false )
            return false;
    return true;
}

void GameRoomMenu::updateReady()
{
    CEGUI::Listbox* listbox = (CEGUI::Listbox*)wManager->getWindow(name+"/playerListbox");
   
    std::map<int, std::pair<std::string,bool> >::iterator itr;
    for(itr=playerList.begin();itr!=playerList.end();itr++)
    {
       for(int i=0;i<listbox->getItemCount();i++)
           if(listbox->getListboxItemFromIndex(i)->getText() == itr->second.first)
              listbox->setItemSelectState(listbox->getListboxItemFromIndex(i),itr->second.second);
    }
}

void GameRoomMenu::setMapImage(std::string mapName)
{
    std::string path = ((LobbyMenu*)parentMenu)->getMaps()[mapName];

    CEGUI::ImagesetManager::getSingleton().destroy(mapName);
    CEGUI::ImagesetManager::getSingleton().createFromImageFile(mapName,path);
    wManager->getWindow(name+"/mapImage")->setProperty("Image", "set:"+mapName+" image:full_image");
}

CEGUI::Window* GameRoomMenu::getSheet()
{
    return sheet;
}
/*
bool GameRoomMenu::itemChangedEvent(const CEGUI::EventArgs &evt)
{

    std::map<std::string, std::string> mapList = ((LobbyMenu*)parentMenu)->getMaps();
    std::cout<<mapList[((CEGUI::Combobox*)wManager->getWindow("GameRoom/mapCombobox"))->getSelectedItem()->getText().c_str()]<<std::endl;

    CEGUI::ImagesetManager::getSingleton().destroy("ImageForStaticImage");
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("ImageForStaticImage",mapList[((CEGUI::Combobox*)wManager->getWindow("GameRoom/mapCombobox"))->getSelectedItem()->getText().c_str()]);
    
    wManager->getWindow("GameRoom/mapImage")->setProperty("Image", "set:ImageForStaticImage image:full_image");

    lobbyCallbackInterface->lobbyGameRoomMapChangedEvent();
    
}
 */