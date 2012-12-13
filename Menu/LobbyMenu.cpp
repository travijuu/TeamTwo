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

#include "LobbyMenu.h"
#include "LobbyCallbackInterface.h"
#include "MenuController.h"
#include "Util/Converter.h"

LobbyMenu::LobbyMenu(std::string name, MenuControllerInterface* menuControllerInterface, LobbyCallbackInterface* lobbyCallbackInterface)
{
    
    wManager = &CEGUI::WindowManager::getSingleton();
    sheet = wManager->createWindow("DefaultWindow",this->name+"Sheet");
    this->lobbyCallbackInterface = lobbyCallbackInterface;
    this->name = name;
    this->menuControllerInterface = menuControllerInterface;

    subMenuVector.push_back("Discover");
    subMenuVector.push_back("ServerConnection");
    subMenuVector.push_back("Lobby");
    subMenuVector.push_back("Connecting");
    currentMenu = "Discover";
    current = 0;
    
    createGameMenu = new CreateGameMenu("CreateGame", this, this->sheet, lobbyCallbackInterface);
    gameRoomMenu = new GameRoomMenu("GameRoom", this, this->sheet, lobbyCallbackInterface);
    teamGameRoomMenu = new TeamGameRoomMenu("TeamGameRoom",this,this->sheet, lobbyCallbackInterface);
    
    createMenu();


    setIPAddress("192.168.1.1");
}

LobbyMenu::~LobbyMenu()
{
    delete &gameList;
    delete &serverList;
    delete &subMenuVector;
}
 
std::string LobbyMenu::getRoomName()
{
    return roomName;
}

void LobbyMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
    subMenuCleaner();
}

bool LobbyMenu::controlButtonEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
    
    CEGUI::MultiColumnList* mcl =  ( (CEGUI::MultiColumnList*) wManager->getWindow( mouseEventArgs.window->getName() ) ) ;
    if(mouseEventArgs.window->getName() == name+"/gameMCL")
    {
        if(mcl->getSelectedCount() != 0)
            wManager->getWindow(name+"/connectButton")->setEnabled(true);
        else
            wManager->getWindow(name+"/connectButton")->setEnabled(false);
    }
    else if(mouseEventArgs.window->getName() == "ServerConnection/serverList")
    {
        if(mcl->getSelectedCount() != 0)
            wManager->getWindow("ServerConnection/connectButton")->setEnabled(true);
        else
            wManager->getWindow("ServerConnection/connectButton")->setEnabled(false);
    }
    
    return true;
}
void LobbyMenu::createMenu()
{
    CEGUI::Window* guiLayout = wManager->loadWindowLayout("layouts/Discover.layout");
    //wManager->getWindow("Discover")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow("Discover")->setVisible(false);
    wManager->getWindow("Discover/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow("Discover/discoverButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow("Discover/connectButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    sheet->addChildWindow(guiLayout);

    guiLayout = wManager->loadWindowLayout("layouts/Connecting.layout");
    wManager->getWindow("Connecting")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow("Connecting")->setVisible(false);
    sheet->addChildWindow(guiLayout);

    guiLayout = wManager->loadWindowLayout("layouts/ServerConnection.layout");
    wManager->getWindow("ServerConnection")->setVisible(false);
    wManager->getWindow("ServerConnection/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow("ServerConnection/connectButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow("ServerConnection/refreshButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow("ServerConnection/connectButton")->setEnabled(false);
    wManager->getWindow("ServerConnection/serverList")->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged, CEGUI::Event::Subscriber(&LobbyMenu::controlButtonEvent,this));
    wManager->getWindow("ServerConnection/serverList")->subscribeEvent(CEGUI::PushButton::EventMouseDoubleClick, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    sheet->addChildWindow(guiLayout);
    
    guiLayout = wManager->loadWindowLayout("layouts/Lobby.layout");
    wManager->getWindow(name)->setVisible(false);
    wManager->getWindow(name+"/disconnectButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/connectButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/connectButton")->setEnabled(false);
    wManager->getWindow(name+"/refreshButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/createGameButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    sheet->addChildWindow(guiLayout);

    CEGUI::MultiColumnList *mcl = ((CEGUI::MultiColumnList*) wManager->getWindow(name+"/gameMCL" ) ) ;
    mcl->subscribeEvent(CEGUI::PushButton::EventMouseDoubleClick, CEGUI::Event::Subscriber(&LobbyMenu::buttonClickEvent,this));
    mcl->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged, CEGUI::Event::Subscriber(&LobbyMenu::controlButtonEvent,this));
    mcl->addColumn( "Name", 1,  cegui_reldim(0.25f) ) ;
    mcl->addColumn( "Map", 2,  cegui_reldim(0.25f) ) ;
    mcl->addColumn( "Type", 4, cegui_reldim(0.25f) );
    mcl->addColumn( "Capacity",  3,  cegui_reldim(0.25f) ) ;
    

    mcl = ((CEGUI::MultiColumnList*) wManager->getWindow("ServerConnection/serverList" ) ) ;
    mcl->addColumn("Discovered Servers",1,cegui_reldim(0.5f));
    mcl->setFont("Nimbus-9");
    mcl->addColumn("IP Address",2,cegui_reldim(0.5f));
 

}
void LobbyMenu::subMenuCleaner()
{
    for(int i=0;i<subMenuVector.size();i++)
        CEGUI::WindowManager::getSingleton().getWindow(subMenuVector.at(i))->setVisible(false);
}
void LobbyMenu::showMenu()
{
    subMenuCleaner();
    CEGUI::WindowManager::getSingleton().getWindow(subMenuVector.at(current))->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);

}
int getIteratorKeyofValue(int value, std::map<int,int> itrMap)
{
    std::map<int,int>::iterator itr;
    for(itr=itrMap.begin();itr!=itrMap.end();itr++)
        if(itr->second == value)
            return itr->first;
    return -1;
}
bool LobbyMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str();
     
     if(actionName == subMenuVector.at(current)+"/backButton")
     {
        if (current > 0)
        {
            current--;
            showMenu();
        }
        else
            menuControllerInterface->back();
        removeAllServers();
        removeAllGames();
         
     }
     else if(actionName == name+"/disconnectButton")
     {
         current--;
         removeAllServers();
         showMenu();
         this->lobbyCallbackInterface->lobbyExitEvent();
     }
     else if(actionName == name+"/refreshButton")
     {
         removeAllGames();
         this->lobbyCallbackInterface->lobbyGameRoomRefreshEvent();
     }
     else if(actionName == name+"/createGameButton" )
     {
         createGameMenu->showMenu();
     }
     else if (actionName == name+"/connectButton" || actionName == name+"/gameMCL")
     {
         CEGUI::MultiColumnList* mcl = ((CEGUI::MultiColumnList*) wManager->getWindow(name+"/gameMCL" ) ) ;

         if(mcl->getFirstSelectedItem() != NULL)
         {
             int rowId = mcl->getItemRowIndex(mcl->getFirstSelectedItem());
             gameId = getIteratorKeyofValue(rowId,gameList);
             this->lobbyCallbackInterface->lobbyGameRoomConnectEvent(gameId);
           
             
             if(mcl->getNextSelected(mcl->getNextSelected(mcl->getFirstSelectedItem()))->getText()=="Death-Match")
             {
                 
                 gameRoomMenu->setHost(false);
                 gameRoomMenu->setGameInfo(mcl->getFirstSelectedItem()->getText().c_str(),mcl->getNextSelected(mcl->getFirstSelectedItem())->getText().c_str(),mcl->getNextSelected(mcl->getNextSelected(mcl->getFirstSelectedItem()))->getText().c_str(),Converter::stringToInt(mcl->getNextSelected(mcl->getNextSelected(mcl->getNextSelected(mcl->getFirstSelectedItem())))->getText().c_str()));
                 gameRoomMenu->showMenu();
             }
             else
             {
                 teamGameRoomMenu->setHost(false);
                 teamGameRoomMenu->setGameInfo(mcl->getFirstSelectedItem()->getText().c_str(),mcl->getNextSelected(mcl->getFirstSelectedItem())->getText().c_str(),mcl->getNextSelected(mcl->getNextSelected(mcl->getFirstSelectedItem()))->getText().c_str(),Converter::stringToInt(mcl->getNextSelected(mcl->getNextSelected(mcl->getNextSelected(mcl->getFirstSelectedItem())))->getText().c_str()));
                 teamGameRoomMenu->showMenu();
             }
         }
     }
     else if (actionName == "Discover/discoverButton")
     {
        // removeAllServers();
         this->lobbyCallbackInterface->lobbyEnableLocalServerDiscoveryEvent();
         current++;
         showMenu();
     }
     else if(actionName == "Discover/connectButton")
     {
         CEGUI::String ip = wManager->getWindow("Discover/ipAddressEditbox")->getText();
          if(ip != "")
          {
              for(int i=0;i<ip.length();i++)
                  if((ip.at(i) < '0' || ip.at(i) > '9') && ip.at(i) != '.' )
                      return false;
              //wManager->getWindow("Discover")->setVisible(false);
              current+=2;
			  std::cout<<"IP::: "<<ip<<std::endl;
              this->lobbyCallbackInterface->lobbyServerConnectEvent( wManager->getWindow("Discover/ipAddressEditbox")->getText().c_str() );
          }
         

         //current++;
         //showMenu();
     }
     else if(actionName == "ServerConnection/connectButton" || actionName == "ServerConnection/serverList")
     {
         CEGUI::MultiColumnList* mcl =  ((CEGUI::MultiColumnList*) wManager->getWindow("ServerConnection/serverList" ) );

         if(mcl->getFirstSelectedItem() != NULL)
         {
			 std::cout<<"IP:: "<<mcl->getNextSelected(mcl->getFirstSelectedItem())->getText().c_str()<<std::endl;
             this->lobbyCallbackInterface->lobbyServerConnectEvent(mcl->getNextSelected(mcl->getFirstSelectedItem())->getText().c_str());
             
             current++;
             showMenu();
         }
     }
     else if(actionName == "ServerConnection/refreshButton")
     {
         removeAllServers();
         this->lobbyCallbackInterface->lobbyRefreshServerListEvent();
     }
     else
         std::cout<<actionName<<std::endl;

     return true;
}
void LobbyMenu::addGameToList( int gameId, std::string gameName, std::string mapName, std::string type, std::string gameCapacity)
{
	std::cout<<"adding game to the list\n";
    CEGUI::MultiColumnList* mcl =  ((CEGUI::MultiColumnList*) wManager->getWindow(name+"/gameMCL") ) ;
    try
    {
        int index=mcl->getRowWithID(gameId);
    }
    catch(CEGUI::InvalidRequestException e)
    {

        CEGUI::ListboxTextItem* column1 = new CEGUI::ListboxTextItem( gameName ) ;
        CEGUI::ListboxTextItem* column2 = new CEGUI::ListboxTextItem( mapName ) ;
        CEGUI::ListboxTextItem* column3 = new CEGUI::ListboxTextItem( gameCapacity ) ;
        CEGUI::ListboxTextItem* column4 = new CEGUI::ListboxTextItem( type ) ;

        column1->setSelectionBrushImage( &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ) ) ;
        column2->setSelectionBrushImage( &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ) ) ;
        column3->setSelectionBrushImage( &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ) ) ;
        column4->setSelectionBrushImage( &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ) ) ;

        int row=mcl->addRow(gameId);
        //int row = mcl->addRow( column1, 1 ) ;
        mcl->setItem( column1, 1, row ) ;
        mcl->setItem( column2, 2, row ) ;
        mcl->setItem( column3, 3, row ) ;
        mcl->setItem( column4, 4, row ) ;

        gameList[gameId] = row;
    }
	std::cout<<"added\n";

}

void LobbyMenu::removeGameFromList(int gameId)
{
	
	std::cout<<"removing game, gameid: "<<gameId<<std::endl;
    CEGUI::MultiColumnList* mcl =  ((CEGUI::MultiColumnList*) wManager->getWindow(name+"/gameMCL") );
    if (gameList.count(gameId) > 0)
    {
        mcl->removeRow(mcl->getRowWithID(gameId));
        //mcl->removeRow(gameList[gameId]);
        gameList.erase(gameId);
    }
    std::cout<<"game removed from the list\n";
}

void LobbyMenu::addServerToList(int serverId, std::string serverName, std::string serverIP)
{
    CEGUI::MultiColumnList* mcl =  (CEGUI::MultiColumnList*) wManager->getWindow("ServerConnection/serverList") ;

    CEGUI::ListboxTextItem* column1 = new CEGUI::ListboxTextItem( serverName ) ;
    CEGUI::ListboxTextItem* column2 = new CEGUI::ListboxTextItem( serverIP ) ;
    
    column1->setSelectionBrushImage( &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ) ) ;
    column2->setSelectionBrushImage( &CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage( "MultiListSelectionBrush" ) ) ;
    int row = mcl->addRow(serverId);

    //int row=mcl->insertRow(0,serverId);
    mcl->setItem( column1,  1, row ) ;
    mcl->setItem( column2,  2, row ) ;
 
    serverList[serverId] = row;
}
void LobbyMenu::removeServerFromList(int serverId)
{
    CEGUI::MultiColumnList* mcl =  ((CEGUI::MultiColumnList*) wManager->getWindow("ServerConnection/serverList") ) ;
    if(mcl->getRowCount() > gameList[serverId])
    {
        mcl->removeRow(mcl->getRowWithID(serverId));
        serverList.erase(serverId);
    }
}
void LobbyMenu::resetFields()
{
    removeAllGames();
    removeAllServers();
}

GameRoomMenu* LobbyMenu::getGameRoomMenu()
{
    return gameRoomMenu;
}
TeamGameRoomMenu* LobbyMenu::getTeamGameRoomMenu()
{
    return teamGameRoomMenu;
}

void LobbyMenu::showLobby()
{
    wManager->getWindow(name)->setVisible(true);
	CEGUI::System::getSingleton().setGUISheet(sheet);
	resetFields();
}
void LobbyMenu::hideLobby()
{
    wManager->getWindow(name)->setVisible(false);
}

void LobbyMenu::removeAllGames()
{
    ((CEGUI::MultiColumnList*)wManager->getWindow(name+"/gameMCL"))->resetList();
	gameList.clear();
    wManager->getWindow(name+"/connectButton")->setEnabled(false);
}
void LobbyMenu::removeAllServers()
{
    ((CEGUI::MultiColumnList*)wManager->getWindow("ServerConnection/serverList"))->resetList();
    serverList.clear();
    wManager->getWindow("ServerConnection/connectButton")->setEnabled(false);
}
CEGUI::Window* LobbyMenu::getSheet()
{
    return this->sheet;
}

void LobbyMenu::setIPAddress(std::string ipAddress)
{
    this->ipAddress = ipAddress;
    wManager->getWindow("Discover/ipAddressEditbox")->setText(ipAddress);
}

void LobbyMenu::modifyGameEntry(int gameId, std::string gameName, std::string mapName, std::string type, std::string gameCapacity)
{
    removeGameFromList(gameId);
    addGameToList(gameId, gameName, mapName, type, gameCapacity);
}

void LobbyMenu::modifyServerEntry(int serverId, std::string serverName, std::string serverIP)
{
    removeServerFromList(serverId);
    addServerToList(serverId, serverName, serverIP);
}

void LobbyMenu::addMap(std::string mapName, std::string path)
{
    mapList[mapName] = path;
    CEGUI::ListboxItem *item = new CEGUI::ListboxTextItem(mapName);
    ((CEGUI::Combobox*)wManager->getWindow("CreateGame/mapCombobox"))->addItem(item);
    ((CEGUI::Combobox*)wManager->getWindow("CreateGame/mapCombobox"))->setSortingEnabled(true);
    ((CEGUI::Combobox*)wManager->getWindow("CreateGame/mapCombobox"))->setReadOnly(true);
    ((CEGUI::Combobox*)wManager->getWindow("CreateGame/mapCombobox"))->setItemSelectState((size_t)0,true);
    
}

void LobbyMenu::removeMap(std::string mapName)
{
    mapList.erase(mapName);
    CEGUI::ListboxItem *item = new CEGUI::ListboxTextItem(mapName);
    ((CEGUI::Combobox*)wManager->getWindow("CreateGame/mapCombobox"))->removeItem(item);
}
std::map<std::string, std::string> LobbyMenu::getMaps()
{
    return mapList;
}

void LobbyMenu::setTitle(std::string title)
{
    wManager->getWindow(name)->setText(title);
}

void LobbyMenu::hideConnectionMenu()
{
	wManager->getWindow("Discover")->setVisible(false);
}