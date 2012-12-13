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


#include "InGameGUIHandler.h"
#include "Converter.h"
#include "ItemStructure.h"
#include "Weapon.h"
#include <iostream>
#include <string>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <stdlib.h>
#include <OGRE/OgreRenderTarget.h>
#include <stack>
#include <OIS/OISKeyboard.h>


InGameGUIHandler::InGameGUIHandler(Ogre::RenderWindow* renderWindow) {
    name = "InGameGUI";
    wManager = &CEGUI::WindowManager::getSingleton();
    //this->sheet = sheet;
    this->sheet =  wManager->createWindow("DefaultWindow","InGame");
    this->health = 100;
    this->gun = 30;
    this->mag = 120;
    this->renderWindow = renderWindow;
      CEGUI::FontManager::getSingleton().create( "Nimbus-9.font" );
      CEGUI::FontManager::getSingleton().create( "Nimbus-12.font" );
      CEGUI::FontManager::getSingleton().create( "Nimbus-20.font" );
      CEGUI::FontManager::getSingleton().create( "Nimbus-24.font" );

    gameType = DEATH_MATCH;
    createGUIs();
    gameMessageAlpha = 0;
    gameMessageFlag = false;

    gameInfoAlpha = 0.7;
    gameInfoFlag = false;
	showing=hiding=false;
	autoHide=false;
	size=-.36;
}

InGameGUIHandler::~InGameGUIHandler() {
    delete &inGameGuiMap;
    delete &inGameGuiStatusMap;
}
void InGameGUIHandler::activate()
{
     CEGUI::System::getSingleton().setGUISheet( sheet );
}
void InGameGUIHandler::setHealth(int value)
{
    health = value;
    
    temp.str("");
    temp<<this->health;
    std::string color = "[colour='FF00FF00']";
    this->item = CEGUI::WindowManager::getSingleton().getWindow("HealthInfo/healthText");
    if(value<40)
        color = "[colour='FFFF0000']";
    else if(value<70)
        color = "[colour='FFFFFF00']";
    this->item->setText(color+temp.str());
}
void InGameGUIHandler::setAmmo(int value)
{
    ammo = value;
    temp.str("");
    temp<<this->ammo<<" / "<<this->mag;
    this->item = CEGUI::WindowManager::getSingleton().getWindow("WeaponInfo/weaponText");
    this->item->setText(temp.str());
}
void InGameGUIHandler::setMag(int value)
{
    mag = value;
}
void InGameGUIHandler::setGun(int value)
{
    gun = value;
}
void InGameGUIHandler::setGameMode(int mode)
{
    gameMode = mode;
    hideAll();
    switch(gameMode)
    {
        case MODE_PLAY:
            show(HEALTH);
            show(RADAR);
            show(GAME_INFO);
            break;
        case MODE_SPECTATOR:
            show(RADAR);
            show(GAME_INFO);
            break;
    }
}

void InGameGUIHandler::setGameType(int type)
{
    gameType = type;
}

void InGameGUIHandler::enable(int type, bool isVisible)
{
    switch(type)
    {
        case HEALTH:
        {
            inGameGuiMap[type] = "HealthInfo";
            item = wManager->loadWindowLayout("layouts/HealthInfo.layout");
            wManager->getWindow("HealthInfo")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0), CEGUI::UDim(0.84f, 0)));
            wManager->getWindow("HealthInfo/healthText")->setText("85");
            wManager->getWindow("HealthInfo/healthImage")->setProperty("Image", "set:HealthImage image:full_image");
            wManager->getWindow("HealthInfo")->setVisible(isVisible);
            break;
        }
        case WEAPON:
        {
            inGameGuiMap[type] = "WeaponInfo";
            item = wManager->loadWindowLayout("layouts/WeaponInfo.layout");
            wManager->getWindow("WeaponInfo")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.65f, 0), CEGUI::UDim(0.84f, 0)));
            wManager->getWindow("WeaponInfo/weaponText")->setText("30/120");
            wManager->getWindow("WeaponInfo/weaponImage")->setProperty("Image", "set:WeaponImage image:full_image");
            wManager->getWindow("WeaponInfo")->setVisible(isVisible);
            break;
        }
        case GAME_INFO:
        {
            inGameGuiMap[type] = "GameInfo";
            item = wManager->loadWindowLayout("layouts/GameInfo.layout");
            wManager->getWindow("GameInfo")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0), CEGUI::UDim(0.0f, 0)));
            wManager->getWindow("GameInfo")->setVisible(isVisible);
            wManager->getWindow("GameInfo")->setAlpha(0.7);

            for(int i=0;i<MESSAGE_QUEUE_SIZE;i++)
                messageVector.push_back(" ");
            break;
        }
        case CHAT:
        {
            inGameGuiMap[type] = "ChatPanel";
            item = wManager->loadWindowLayout("layouts/chat.layout");
            wManager->getWindow("ChatPanel")->setPosition(CEGUI::UVector2(CEGUI::UDim(-2.0f, 0), CEGUI::UDim(0.5f, 0)));
            wManager->getWindow("ChatPanel")->setSize(CEGUI::UVector2(CEGUI::UDim(0.36f, 0), CEGUI::UDim(0.3f, 0)));
            wManager->getWindow("ChatPanel")->setVisible(true);
            wManager->getWindow("ChatPanel/editbox")->setVisible(true);
            break;
        }
        case SCORE_BOARD_TM:
        {
            inGameGuiMap[type] = "ScoreBoardTeam";
            item = wManager->loadWindowLayout("layouts/ScoreBoardTeam.layout");
            wManager->getWindow("ScoreBoardTeam")->setVisible(isVisible);
            wManager->getWindow("ScoreBoardTeam")->setAlpha(0.6);

            CEGUI::MultiColumnList *mcl = ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/allyScore" ) ) ;
            mcl->addColumn( "Player Name", 1,  cegui_reldim(0.33f) ) ;
            mcl->addColumn( "Kill", 2,  cegui_reldim(0.33f) ) ;
            mcl->addColumn( "Death", 3, cegui_reldim(0.33f) );
            
            mcl = ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/opponentScore" ) ) ;
            mcl->addColumn( "Player Name", 1,  cegui_reldim(0.33f) ) ;
            mcl->addColumn( "Kill", 2,  cegui_reldim(0.33f) ) ;
            mcl->addColumn( "Death", 3, cegui_reldim(0.33f) );
            
            mcl = ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/opponentScore" ) ) ;
            mcl->setSortDirection(CEGUI::ListHeaderSegment::Ascending);
            mcl->setSortColumn(1);
            
            mcl = ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/allyScore" ) ) ;
            mcl->setSortDirection(CEGUI::ListHeaderSegment::Ascending);
            mcl->setSortColumn(1);
          //  updatePlayerScores(2,"Setra",13,5);
            break;
        }
        case SCORE_BOARD_DM:
        {
            inGameGuiMap[type] = "ScoreBoard";
            item = wManager->loadWindowLayout("layouts/ScoreBoard.layout");
            wManager->getWindow("ScoreBoard")->setVisible(isVisible);
            wManager->getWindow("ScoreBoard")->setAlpha(0.6);

            CEGUI::MultiColumnList *mcl = ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoard/score" ) ) ;
            mcl->addColumn( "Player Name", 1,  cegui_reldim(0.33f) ) ;
            mcl->addColumn( "Kill", 2,  cegui_reldim(0.33f) ) ;
            mcl->addColumn( "Death", 3, cegui_reldim(0.33f) );
            //removePlayerFromList(6);            
            break;
        }
        case ITEM_LIST:
        {
            inGameGuiMap[type] = "ItemList";
            item = wManager->loadWindowLayout("layouts/ItemList.layout");
            wManager->getWindow("ItemList")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.90f, 0), CEGUI::UDim(0.22f, 0)));
            wManager->getWindow("ItemList")->setVisible(isVisible);
            itemListMap["ItemList/item1"] = -1;
            itemListMap["ItemList/item2"] = -1;
            itemListMap["ItemList/item3"] = -1;
            break;
        }
        case INGAME_TEXT:
        {
            inGameGuiMap[type] = "IngameText";
            item = wManager->loadWindowLayout("layouts/InGameText.layout");
            wManager->getWindow("IngameText")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
            wManager->getWindow("IngameText")->setVisible(isVisible);
            break;
        }
    }
    sheet->addChildWindow(item);
    //std::cout<<inGameGuiMap[type]<<std::endl;
    inGameGuiStatusMap[type] = isVisible;
    //CEGUI::WindowManager::getSingleton().getWindow(inGameGuiMap[type].c_str())->setVisible(true);
}
void InGameGUIHandler::disable(int type)
{
    wManager->destroyWindow(inGameGuiMap[type].c_str());
    inGameGuiMap.erase(type);
}

void InGameGUIHandler::hideAll()
{
    std::map<int, std::string>::const_iterator itr;

    for(itr=inGameGuiMap.begin();itr != inGameGuiMap.end();++itr)
    {
        item = CEGUI::WindowManager::getSingleton().getWindow((*itr).second);
        item->setVisible(false);
        inGameGuiStatusMap[(*itr).first] = false;
    }
}
void InGameGUIHandler::hide(int type)
{
    if(type==SCORE_BOARD)
    {
        gameType == DEATH_MATCH ? type = SCORE_BOARD_DM : type = SCORE_BOARD_TM;
    }

    const CEGUI::Image* image=CEGUI::MouseCursor::getSingletonPtr()->getImage();
    CEGUI::WindowManager::getSingleton().getWindow(inGameGuiMap[type].c_str())->setVisible(false);
    CEGUI::MouseCursor::getSingletonPtr()->setImage(image);
    inGameGuiStatusMap[type] = false;
}
void InGameGUIHandler::show(int type)
{
    if(type==CURSOR)
    {
        CEGUI::MouseCursor::getSingleton().setImage("TaharezLook","MouseTarget");
        CEGUI::MouseCursor::getSingleton().setPosition( CEGUI::Point(renderWindow->getWidth()/2-CEGUI::MouseCursor::getSingleton().getImage()->getWidth()/2,
                                                                     renderWindow->getHeight()/2+CEGUI::MouseCursor::getSingleton().getImage()->getHeight()*2));
        CEGUI::MouseCursor::getSingleton().show();
        return;
    }
    else if(type==SCORE_BOARD)
    {
        gameType == DEATH_MATCH ? type = SCORE_BOARD_DM : type = SCORE_BOARD_TM;
    }

    const CEGUI::Image* image=CEGUI::MouseCursor::getSingletonPtr()->getImage();
    CEGUI::WindowManager::getSingleton().getWindow(inGameGuiMap[type].c_str())->setVisible(true);
    CEGUI::MouseCursor::getSingletonPtr()->setImage(image);
    inGameGuiStatusMap[type] = true;
    //CEGUI::System::getSingleton().setGUISheet( sheet );
}

void InGameGUIHandler::toggle(int type)
{
    item = CEGUI::WindowManager::getSingleton().getWindow(inGameGuiMap[type].c_str());
    
    if(inGameGuiStatusMap[type] == true)
        item->setVisible(false);
    else
        item->setVisible(true);

    if(type == CHAT)
        ((CEGUI::Editbox*)wManager->getWindow("ChatPanel/editbox"))->activate();

    inGameGuiStatusMap[type] = !inGameGuiStatusMap[type];

}

std::string InGameGUIHandler::getChatMessage()
{
	return std::string(wManager->getWindow("ChatPanel/editbox")->getText().c_str());
}

void InGameGUIHandler::addChatMessage(std::string message,bool show)
{
    wManager->getWindow("ChatPanel/chatArea")->appendText(message);
    ((CEGUI::MultiLineEditbox*)wManager->getWindow("ChatPanel/chatArea"))->setCaratIndex(wManager->getWindow("ChatPanel/chatArea")->getText().length());
	if(show)
	{
		showing=true;
                if(inGameGuiStatusMap[CHAT] == false)
                    autoHide=true;
		hideTimer.reset();
	}
	else
		hiding=true;
}

void InGameGUIHandler::controlChat()
{ 
    
    
    if( inGameGuiStatusMap[CHAT] == false )
    {
		showing=true;
                autoHide=false;
		std::cout<<"active\n";
        wManager->getWindow("ChatPanel")->setAlpha(1.0);
		wManager->getWindow("ChatPanel")->setVisible(true);
        ((CEGUI::Editbox*)wManager->getWindow("ChatPanel"))->activate();
        ((CEGUI::Editbox*)wManager->getWindow("ChatPanel/editbox"))->activate();
        //((CEGUI::Editbox*)wManager->getWindow("ChatPanel/editbox"))->setVisible(true);		
        wManager->getWindow("ChatPanel/editbox")->setText(" ");
        wManager->getWindow("ChatPanel/editbox")->setText("");
    }
    else
    {
		hiding=true;
        wManager->getWindow("ChatPanel")->setAlpha(0.3); 
        ((CEGUI::Editbox*)wManager->getWindow("ChatPanel"))->deactivate();
        ((CEGUI::Editbox*)wManager->getWindow("ChatPanel/editbox"))->deactivate();
        wManager->getWindow("ChatPanel/editbox")->setText("");
    }

    inGameGuiStatusMap[CHAT] = ! inGameGuiStatusMap[CHAT];
	wManager->getWindow("ChatPanel/editbox")->setText("");
}

bool InGameGUIHandler::isChatOpen()
{
    return inGameGuiStatusMap[CHAT];
}

void InGameGUIHandler::addPlayerToList(int team, int id, std::string playerName, int kill, int death)
{
    ScoreBoardInfo_t playerInfo;
    playerInfo.playerName = playerName;
    playerInfo.kill = kill;
    playerInfo.death = death;
    playerInfo.team = team;

    scoreBoardMap[id] = playerInfo;
    
    CEGUI::MultiColumnList *mcl = getScoreBoardMCL(team);
    std::cout<<"TEAM: "<<team<<" - "<<mcl->getName()<<std::endl;
  //  exit(1);
   // mcl->setSortDirection(CEGUI::ListHeaderSegment::Descending);
  //  mcl->setSortColumn(2);
    CEGUI::ListboxTextItem* column1 = new CEGUI::ListboxTextItem( playerName ) ;
    CEGUI::ListboxTextItem* column2 = new CEGUI::ListboxTextItem( Converter::intToString(kill) ) ;
    CEGUI::ListboxTextItem* column3 = new CEGUI::ListboxTextItem( Converter::intToString(death) ) ;

    int row = mcl->addRow( column1, 1 ) ;
    mcl->setItem( column2,  2, row ) ;
    mcl->setItem( column3,  3, row ) ;
    

    mcl->setSortColumn(1);
    mcl->setSortDirection(CEGUI::ListHeaderSegment::None);
    mcl->setSortDirection(CEGUI::ListHeaderSegment::Descending);
    
}

 

void InGameGUIHandler::updatePlayerScores(int id, std::string playerName, int kill, int death)
{
    scoreBoardMap[id].kill = kill;
    scoreBoardMap[id].death = death;
    scoreBoardMap[id].playerName = playerName;
    ScoreBoardInfo_t tmp = scoreBoardMap[id];
    
    removePlayerFromList(id);
    addPlayerToList(tmp.team,id, tmp.playerName, tmp.kill, tmp.death);
 

    /*
    resetScoreBoard(scoreBoardMap[id].team);
    reloadScoreBoard();*/
}

void InGameGUIHandler::removePlayerFromList(int id)
{
	std::cout<<"removing player"<<std::endl;
   CEGUI::MultiColumnList* mcl = getScoreBoardMCL(scoreBoardMap[id].team);
   for (int x = 0; x < mcl->getRowCount(); x++)
   {
       for (int y = 0; y < mcl->getColumnCount(); y++)
       {
           CEGUI::ListboxItem *item = mcl->getItemAtGridReference(CEGUI::MCLGridRef(x, y));
           if(item->getText()==scoreBoardMap[id].playerName)
           {
               mcl->removeRow(mcl->getItemRowIndex(item));
               scoreBoardMap.erase(id);
               return;
           }
       }
   }
   
    /*
    resetScoreBoard(scoreBoardMap[id].team);
    scoreBoardMap.erase(id);
    reloadScoreBoard();*/
}

void InGameGUIHandler::resetScoreBoard(int type)
{
    switch(type)
    {
        case ALLY_TEAM: 
        case OPPONENT_TEAM:
            ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/allyScore" ) )->resetList();
            ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/opponentScore" ) )->resetList(); break;
        case NO_TEAM: ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoard/score" ) )->resetList(); break;
    }
}

CEGUI::MultiColumnList* InGameGUIHandler::getScoreBoardMCL(int type)
{
     switch(type)
    {
        case ALLY_TEAM: return ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/allyScore" )); break;
        case OPPONENT_TEAM: return ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoardTeam/opponentScore" )); break;
        case NO_TEAM: return ((CEGUI::MultiColumnList*) wManager->getWindow("ScoreBoard/score" )); break;
    }
}

void InGameGUIHandler::showMessage(std::string message)
{
    
     wManager->getWindow("IngameText/text")->setText(message);
    // wManager->getWindow("IngameText")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.15f, 0)));
     wManager->getWindow("IngameText")->setAlpha(gameMessageAlpha);
     gameMessageFlag = false;
     gameMessageAlpha = 0;
  
}

void InGameGUIHandler::addItemIcon(int type, std::string path)
{
    if(staticItemList.count(type)>0)
        return;
    staticItemList[type] = path;
}

void InGameGUIHandler::showItem(int type)
{
    if(staticItemList.count(type)==0)
        return;
    std::map<std::string, int>::iterator itr;
    bool isAdded = false;
    for(itr=itemListMap.begin();itr!=itemListMap.end();itr++)
    {
        if(itr->second == -1 && !isAdded)
        {
            itemListMap[itr->first] = type;
            wManager->getWindow(itr->first)->setVisible(true);
            CEGUI::ImagesetManager::getSingleton().destroy(itr->first);
            CEGUI::ImagesetManager::getSingleton().createFromImageFile(itr->first,staticItemList[type]);
            wManager->getWindow(itr->first)->setProperty("Image", "set:"+itr->first+" image:full_image");
            isAdded = true;
        }
    }
}

void InGameGUIHandler::hideItem(int type)
{
    std::map<std::string, int>::iterator itr;
    for(itr=itemListMap.begin();itr!=itemListMap.end();itr++)
    {
        if(itr->second == type)
        {
            wManager->getWindow(itr->first)->setVisible(false);
            itemListMap[itr->first] = -1;
        }
    }
}

void InGameGUIHandler::addWeaponIcon(int type, std::string path)
{
    CEGUI::ImagesetManager::getSingleton().createFromImageFile(path,path);
    weaponImageList[type] = path;
}

void InGameGUIHandler::changeWeaponIcon(int type)
{
    wManager->getWindow("WeaponInfo/weaponImage")->setProperty("Image", "set:"+weaponImageList[type]+" image:full_image");
}

void InGameGUIHandler::getShot()
{
    shotFlag=true;
    shotAlpha = 0.7;
    const CEGUI::Image* image=CEGUI::MouseCursor::getSingletonPtr()->getImage();
    CEGUI::System::getSingleton().getGUISheet()->addChildWindow(shot);
    CEGUI::MouseCursor::getSingletonPtr()->setImage(image);
}

void InGameGUIHandler::createGUIs()
{
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("HealthImage","images/health.png");
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("WeaponImage","images/weapon.png"); 
    
    shot = wManager->createWindow("TaharezLook/StaticImage","ShotImage");
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("ShotImage","images/shot.png");
    shot->setProperty("FrameEnabled","False");
    shot->setProperty("BackgroundEnabled","False");
    shot->setAlpha(0.0);
    shot->setProperty("Image", "set:ShotImage image:full_image");
    shot->setVisible(true);
}

void InGameGUIHandler::update()
{
    if(!gameMessageFlag)
    {
        if(gameMessageAlpha<0.8)
            gameMessageAlpha += .015;
        else
            gameMessageFlag = !gameMessageFlag;
    }
    else
    {
        if(gameMessageAlpha>=0)
            gameMessageAlpha -= .015;
       // else
         //   flag = !flag;
    }
    wManager->getWindow("IngameText")->setAlpha(gameMessageAlpha);

    if(gameInfoFlag)
    {
        ((CEGUI::Listbox*)wManager->getWindow("GameInfo"))->resetList();
        ((CEGUI::Listbox*)wManager->getWindow("GameInfo"))->setVisible(true);
        ((CEGUI::Listbox*)wManager->getWindow("GameInfo"))->setAlpha(gameInfoAlpha);
        for(int i=0;i<MESSAGE_QUEUE_SIZE;i++)
        {
            ((CEGUI::Listbox*)wManager->getWindow("GameInfo"))->addItem(new CEGUI::ListboxTextItem(messageVector.at(i)));
        }
        if(gameInfoTimer>150)
            gameInfoFlag = false;
        gameInfoTimer++;
    }
    else
    {
        if(gameInfoAlpha>0)
        {
            gameInfoAlpha -= .015;
            ((CEGUI::Listbox*)wManager->getWindow("GameInfo"))->setAlpha(gameInfoAlpha);
        }
    }

	if(showing)
	{
            if(size<=-0.03)
            {
                size+=.03;
                wManager->getWindow("ChatPanel")->setPosition(CEGUI::UVector2(CEGUI::UDim(size, 0), CEGUI::UDim(0.5f, 0)));
            }
            else
                showing=false;
	}
	else if(hiding)
        {
            if(size>=-.36)
            {
                size-=.03;
                wManager->getWindow("ChatPanel")->setPosition(CEGUI::UVector2(CEGUI::UDim(size, 0), CEGUI::UDim(0.5f, 0)));
            }
            else
                hiding=false;
	}

	if(autoHide)
	{
            if(hideTimer.getMilliseconds()>4000)
            {
                hiding=true;
                autoHide=false;
            }
	}

    if(shotFlag)
    {
        if(shotAlpha>=0)
        {
            shotAlpha -= 0.05;
            shot->setAlpha(shotAlpha);
        }
         else
             shotFlag = false;
     }
}

void InGameGUIHandler::addGameInfo(std::string message)
{
    
    messageVector.push_back(">"+message);
    messageVector.erase(messageVector.begin());
    gameInfoFlag = true;
    gameInfoTimer = 0;
    gameInfoAlpha = 0.7;
    
}