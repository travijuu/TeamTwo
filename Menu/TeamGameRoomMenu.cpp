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

#include "TeamGameRoomMenu.h"
#include "LobbyMenu.h"
 
TeamGameRoomMenu::TeamGameRoomMenu(std::string name, Menu* menu, CEGUI::Window* sheet,  LobbyCallbackInterface* lobbyCallbackInterface)
{
    this->name = name;
    this->parentMenu = menu;
    this->lobbyCallbackInterface = lobbyCallbackInterface;
    this->wManager = &CEGUI::WindowManager::getSingleton();
    wManager->destroyWindow(name+"Sheet");
    this->sheet = this->wManager->createWindow("DefaultWindow",this->name+"Sheet");

    readyRequest = false;
    
    createMenu();
}


TeamGameRoomMenu::~TeamGameRoomMenu() {
}

bool TeamGameRoomMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str();
     std::cout<<actionName<<std::endl;
     if(actionName == name+"/backButton")
     {
         resetFields();
         this->lobbyCallbackInterface->lobbyGameRoomExitEvent();
         closeMenu();
         ((LobbyMenu*)parentMenu)->removeAllGames();
         parentMenu->showMenu();
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
     else if(actionName == name+"/kickAllyButton")
     {
         if(((CEGUI::Listbox*) wManager->getWindow(name+"/allyTeam"))->getFirstSelectedItem() == NULL)
             return false;
         std::string playerName = ((CEGUI::Listbox*) wManager->getWindow(name+"/allyTeam"))->getFirstSelectedItem()->getText().c_str();

         std::map<int,std::pair<std::string,bool> >::iterator itr;

         for(itr=allyList.begin();itr!=allyList.end();itr++)
           if(itr->second.first == playerName)
              playerId = itr->first;

         removePlayerFromList(playerId);
         this->lobbyCallbackInterface->lobbyGameRoomKickPlayer(playerId);

     }
     else if(actionName == name+"/kickOpponentButton")
     {
         if(((CEGUI::Listbox*) wManager->getWindow(name+"/opponentTeam"))->getFirstSelectedItem() == NULL)
             return false;
         std::string playerName = ((CEGUI::Listbox*) wManager->getWindow(name+"/opponentTeam"))->getFirstSelectedItem()->getText().c_str();
         std::cout<<"PlayerName: "<<playerName<<std::endl;
         std::map<int,std::pair<std::string,bool> >::iterator itr;

         for(itr=opponentList.begin();itr!=opponentList.end();itr++)
           if(itr->second.first == playerName)
              playerId = itr->first;

         removePlayerFromList(playerId);
         this->lobbyCallbackInterface->lobbyGameRoomKickPlayer(playerId);
     }
     else if(actionName == name+"/startButton")
     {
         this->lobbyCallbackInterface->lobbyGameRoomStartEvent();
      //   resetFields();
     }
     else if(actionName == name+"/changeButton")
     {
         this->lobbyCallbackInterface->lobbyGameRoomTeamChangedEvent();
     }

     std::cout<<actionName<<std::endl;
     
     return true;
}

bool TeamGameRoomMenu::keyUpEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::KeyEventArgs& keyEventArgs = (CEGUI::KeyEventArgs&)evt;
    if(keyEventArgs.scancode == 28)
    {
        std::string message = wManager->getWindow(name+"/chatEditbox")->getText().c_str();
        if(message != "")
         {
            this->lobbyCallbackInterface->lobbyGameRoomSendMessageEvent(message);
             wManager->getWindow(name+"/chatEditbox")->setText("");
        }
    }
    return true;
}
bool TeamGameRoomMenu::controlButtonEvent(const CEGUI::EventArgs& evt)
{
    if(!hostStatus)
        return true;

    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;

    if(mouseEventArgs.window->getName() == name+"/allyTeam")
    {
        CEGUI::Listbox* listbox = (CEGUI::Listbox*) wManager->getWindow(name+"/allyTeam");

        if(listbox->getFirstSelectedItem() != NULL)
        {
            wManager->getWindow(name+"/kickAllyButton")->setEnabled(true);
        }
        else
        {
            updateReady(PLAYER_LIST_TYPE::ALLY_LIST);
            wManager->getWindow(name+"/kickAllyButton")->setEnabled(false);
        }

    }
    else
    {
        CEGUI::Listbox* listbox = (CEGUI::Listbox*) wManager->getWindow(name+"/opponentTeam");
        if(listbox->getFirstSelectedItem() != NULL)
        {
            wManager->getWindow(name+"/kickOpponentButton")->setEnabled(true);
        }
        else
        {
            updateReady(PLAYER_LIST_TYPE::OPPONENT_LIST);
            wManager->getWindow(name+"/kickOpponentButton")->setEnabled(false);
        }
    }
    return true;
}
bool TeamGameRoomMenu::emptyEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
    if(mouseEventArgs.window->getName() == name+"/allyTeam")
    {
        ((CEGUI::Listbox*) wManager->getWindow(name+"/allyTeam"))->clearAllSelections();
    }
    else
    {
        ((CEGUI::Listbox*) wManager->getWindow(name+"/opponentTeam"))->clearAllSelections();
    }
    return true;
}
void TeamGameRoomMenu::createMenu()
{
    wManager->destroyWindow(name);
    
    CEGUI::Window* menuLayout = wManager->loadWindowLayout("layouts/GameRoomTM.layout");
    wManager->getWindow(name)->setVisible(false);
    wManager->getWindow(name+"/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/kickAllyButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/kickAllyButton")->setEnabled(false);
    wManager->getWindow(name+"/kickOpponentButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/kickOpponentButton")->setEnabled(false);
    wManager->getWindow(name+"/changeButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/readyButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/startButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/sendButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TeamGameRoomMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/chatEditbox")->subscribeEvent(CEGUI::Editbox::EventKeyUp, CEGUI::Event::Subscriber(&TeamGameRoomMenu::keyUpEvent,this));
    ((CEGUI::Editbox*)wManager->getWindow(name+"/chatEditbox"))->setMaxTextLength(32);
    wManager->getWindow(name+"/allyTeam")->subscribeEvent(CEGUI::Listbox::EventMouseClick, CEGUI::Event::Subscriber(&TeamGameRoomMenu::controlButtonEvent,this));
    wManager->getWindow(name+"/allyTeam")->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick, CEGUI::Event::Subscriber(&TeamGameRoomMenu::emptyEvent,this));
    wManager->getWindow(name+"/allyTeam")->subscribeEvent(CEGUI::Listbox::EventMouseTripleClick, CEGUI::Event::Subscriber(&TeamGameRoomMenu::emptyEvent,this));
    wManager->getWindow(name+"/opponentTeam")->subscribeEvent(CEGUI::Listbox::EventMouseClick, CEGUI::Event::Subscriber(&TeamGameRoomMenu::controlButtonEvent,this));
    wManager->getWindow(name+"/opponentTeam")->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick, CEGUI::Event::Subscriber(&TeamGameRoomMenu::emptyEvent,this));
    wManager->getWindow(name+"/opponentTeam")->subscribeEvent(CEGUI::Listbox::EventMouseTripleClick, CEGUI::Event::Subscriber(&TeamGameRoomMenu::emptyEvent,this));

    sheet->addChildWindow(menuLayout);
}

void TeamGameRoomMenu::showMenu()
{
	resetFields();
    wManager->getWindow(name)->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);
}

void TeamGameRoomMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}

void TeamGameRoomMenu::resetFields()
{
    wManager->getWindow(name+"/chatArea")->setText("");
    wManager->getWindow(name+"/chatEditbox")->setText("");
    wManager->getWindow(name+"/gameInfoText")->setText("");
    wManager->getWindow(name+"/readyButton")->setText("Ready To Play");
    readyRequest = false;
    ((CEGUI::Listbox*)wManager->getWindow(name+"/allyTeam"))->resetList();
    ((CEGUI::Listbox*)wManager->getWindow(name+"/opponentTeam"))->resetList();
	opponentList.clear();
	allyList.clear();
}
 
CEGUI::Window* TeamGameRoomMenu::getSheet()
{
    return sheet;
}

void TeamGameRoomMenu::setTitle(std::string title)
{
    wManager->getWindow(name)->setText(title);
}

void TeamGameRoomMenu::setGameInfo(std::string gameName, std::string gameMap, std::string type, int gameCapacity)
{
    std::string info = "Game Name: ";
    info.append(gameName).append("\nGame Map: ").append(gameMap).append("\nGame Type: ").append(type).append("\nCapacity: ").append(Converter::intToString(gameCapacity));

    wManager->getWindow(name+"/gameInfoText")->setText(info);
}

void TeamGameRoomMenu::setHost(bool state)
{
    hostStatus = state;

    if(!hostStatus)
    {

        wManager->getWindow(name + "/startButton")->setEnabled(false);
        wManager->getWindow(name + "/startButton")->setVisible(false);
        wManager->getWindow(name+"/allyTeam")->setEnabled(false);
        wManager->getWindow(name+"/opponentTeam")->setEnabled(false);
        wManager->getWindow(name+"/readyButton")->setVisible(true);
    }
    else
    {

        wManager->getWindow(name + "/startButton")->setEnabled(true);
        wManager->getWindow(name + "/startButton")->setVisible(true);
        wManager->getWindow(name+"/readyButton")->setVisible(false);
        wManager->getWindow(name+"/allyTeam")->setEnabled(true);
        wManager->getWindow(name+"/opponentTeam")->setEnabled(true);
    }
}

void TeamGameRoomMenu::addPlayerToList(int listType, int playerId, std::string playerName, bool ready)
{
    //if(allyList.count(playerId)==0 & opponentList.count(playerId)==0)
    {
        CEGUI::ListboxItem* listboxItem = new CEGUI::ListboxTextItem(playerName);
        listboxItem->setSelectionBrushImage(&CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ));
        std::cout<<"ADDED PLAYER: "<<playerId<<std::endl;
        switch(listType)
        {
            case PLAYER_LIST_TYPE::ALLY_LIST:
            {
                ((CEGUI::Listbox*)wManager->getWindow(name+"/allyTeam"))->addItem(listboxItem);
                std::pair<std::string, bool>playerPair(playerName,ready);
                allyList[playerId] = playerPair;
                setReady(playerId,ready);
                break;
            }
            case PLAYER_LIST_TYPE::OPPONENT_LIST:
            {
                ((CEGUI::Listbox*)wManager->getWindow(name+"/opponentTeam"))->addItem(listboxItem);
                std::pair<std::string, bool>playerPair(playerName,ready);
                opponentList[playerId] = playerPair;
                setReady(playerId,ready);
                break;
            }

        }
    }
    
}

void TeamGameRoomMenu::removePlayerFromList(int playerId)
{
    std::map<int, std::pair<std::string, bool> >::iterator itr;
    for(itr=allyList.begin();itr!=allyList.end();itr++)
    {
		bool found=false;
        if(itr->first == playerId)
        {
            CEGUI::Listbox* listbox = (CEGUI::Listbox*) wManager->getWindow(name+"/allyTeam");
            for(int i =0;i<listbox->getItemCount();i++)
            {
                if( listbox->getListboxItemFromIndex(i)->getText() == itr->second.first )
                {
                    listbox->removeItem(listbox->getListboxItemFromIndex(i));
                    wManager->getWindow(name+"/kickAllyButton")->setEnabled(false);
                    allyList.erase(playerId);
					found=true;
					break;
                }
            }
			if(found)
				break;
        }
    }

    for(itr=opponentList.begin();itr!=opponentList.end();itr++)
    {
		bool found=false;
        if(itr->first == playerId)
        {
            CEGUI::Listbox* listbox = (CEGUI::Listbox*) wManager->getWindow(name+"/opponentTeam");
            for(int i =0;i<listbox->getItemCount();i++)
            {
                if( listbox->getListboxItemFromIndex(i)->getText() == itr->second.first )
                {
                    listbox->removeItem(listbox->getListboxItemFromIndex(i));
                    wManager->getWindow(name+"/kickOpponentButton")->setEnabled(false);
                    opponentList.erase(playerId);
					found=true;
					break;
                }
            }
			if(found)
				break;
        }
    }

    updateReady(PLAYER_LIST_TYPE::ALLY_LIST);
    updateReady(PLAYER_LIST_TYPE::OPPONENT_LIST);
        wManager->getWindow(name+"/startButton")->setEnabled(true);
}

void TeamGameRoomMenu::setReady(int playerId, bool ready)
{
    std::pair<std::string, bool> playerPair = allyList[playerId];
    if(playerPair.first == "")
    {
        playerPair = opponentList[playerId];
        playerPair.second = ready;
        opponentList[playerId] = playerPair;
        CEGUI::Listbox* listbox = (CEGUI::Listbox*)wManager->getWindow(name+"/opponentTeam");

        for(int i=0;i<listbox->getItemCount();i++)
            if( listbox->getListboxItemFromIndex(i)->getText().c_str() == playerPair.first )
                listbox->setItemSelectState(listbox->getListboxItemFromIndex(i),ready);
    }
    else
    {
        playerPair.second = ready;
        allyList[playerId] = playerPair;
        CEGUI::Listbox* listbox = (CEGUI::Listbox*)wManager->getWindow(name+"/allyTeam");

        for(int i=0;i<listbox->getItemCount();i++)
            if( listbox->getListboxItemFromIndex(i)->getText().c_str() == playerPair.first )
                listbox->setItemSelectState(listbox->getListboxItemFromIndex(i),ready);
    }
    
        wManager->getWindow(name+"/startButton")->setEnabled(true);
}

bool TeamGameRoomMenu::isEveryoneReady()
{
    std::map<int, std::pair<std::string,bool> >::iterator itr;
    for(itr=allyList.begin();itr!=allyList.end();itr++)
        if( itr->second.second == false )
            return false;

    for(itr=opponentList.begin();itr!=opponentList.end();itr++)
        if( itr->second.second == false )
            return false;

    return true;
}

void TeamGameRoomMenu::updateReady(int type)
{
    switch(type)
    {
        case PLAYER_LIST_TYPE::ALLY_LIST:
        {
            CEGUI::Listbox* listbox = (CEGUI::Listbox*)wManager->getWindow(name+"/allyTeam");
            std::map<int, std::pair<std::string,bool> >::iterator itr;
            for(itr=allyList.begin();itr!=allyList.end();itr++)
            {
               for(int i=0;i<listbox->getItemCount();i++)
                   if(listbox->getListboxItemFromIndex(i)->getText() == itr->second.first)
                      listbox->setItemSelectState(listbox->getListboxItemFromIndex(i),itr->second.second);
            }
            break;
        }
        case PLAYER_LIST_TYPE::OPPONENT_LIST:
        {
            CEGUI::Listbox* listbox = (CEGUI::Listbox*)wManager->getWindow(name+"/opponentTeam");
            std::map<int, std::pair<std::string,bool> >::iterator itr;
            for(itr=opponentList.begin();itr!=opponentList.end();itr++)
            {
               for(int i=0;i<listbox->getItemCount();i++)
                   if(listbox->getListboxItemFromIndex(i)->getText() == itr->second.first)
                      listbox->setItemSelectState(listbox->getListboxItemFromIndex(i),itr->second.second);
            }
            break;
        }
    }
}

void TeamGameRoomMenu::addMessageToChat(std::string chatMessage)
{
    if(chatMessage != "")
    {
        wManager->getWindow(name+"/chatArea")->appendText(chatMessage+"\n");
        ((CEGUI::MultiLineEditbox*)wManager->getWindow(name+"/chatArea"))->setCaratIndex(wManager->getWindow(name+"/chatArea")->getText().length());
    }
}

void TeamGameRoomMenu::clearChatEditbox()
{
     wManager->getWindow(name+"/chatEditbox")->setText("");
}

void TeamGameRoomMenu::setMapImage(std::string mapName)
{
    std::string path = ((LobbyMenu*)parentMenu)->getMaps()[mapName];
    
    CEGUI::ImagesetManager::getSingleton().destroy(mapName);
    CEGUI::ImagesetManager::getSingleton().createFromImageFile(mapName,path);
    wManager->getWindow(name+"/mapImage")->setProperty("Image", "set:"+mapName+" image:full_image");
}