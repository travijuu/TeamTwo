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

#ifndef _INGAMEGUIHANDLER_H
#define	_INGAMEGUIHANDLER_H
#include "CEGUI/CEGUI.h"
#include "OGRE/OgreRenderWindow.h"

#define MESSAGE_QUEUE_SIZE 8

typedef struct ScoreBoardInfo
{
    std::string playerName;
    int kill, death, team;
}ScoreBoardInfo_t;

class InGameGUIHandler {
public:
    enum MODE {MODE_PLAY=0, MODE_SPECTATOR, MODE_PAUSE };
    enum GUI {HEALTH=100, WEAPON, RADAR, AMMO, GAME_INFO, CHAT, SCORE_BOARD, SCORE_BOARD_DM, SCORE_BOARD_TM, ITEM_LIST, INGAME_TEXT, CURSOR };
    enum TEAM { ALLY_TEAM, OPPONENT_TEAM, NO_TEAM};
    enum GAMETYPE { DEATH_MATCH, TEAM_MATCH };

    InGameGUIHandler(Ogre::RenderWindow* renderWindow);
    ~InGameGUIHandler();
    void setHealth(int value);
    void setAmmo(int value);
    void setMag(int value);
    void setGun(int value);  
    void addChatMessage(std::string message,bool show=false);

    void setGameMode(int mode);
    void setGameType(int type);
    void hide(int type);
    void hideAll();
    void show(int type);
    void toggle(int type);
    void controlChat();
    bool isChatOpen();
    
    void enable(int type, bool isVisible = true);
    void disable(int type);

    void addPlayerToList(int team, int id, std::string playerName, int kill, int death);
    void updatePlayerScores(int id, std::string playerName, int kill, int death);
    void removePlayerFromList(int id);
    void resetScoreBoard(int type);
    CEGUI::MultiColumnList* getScoreBoardMCL(int type);
    void update();
    void showMessage(std::string message);
    void addGameInfo(std::string message);
    void activate();
    void addItemIcon(int type, std::string path);
    void showItem(int type);
    void hideItem(int type);
    void addWeaponIcon(int type, std::string path);
    void changeWeaponIcon(int type);
    void getShot();
    std::string getChatMessage();
    
private:
    int health, ammo, mag, gun, gameMode, gameType;
    CEGUI::WindowManager* wManager;
    CEGUI::Window* sheet, *item, *shot;
    std::string name, chatMessage;
    std::stringstream temp;
    Ogre::RenderWindow *renderWindow;
    std::vector<std::string> messageVector;
    std::map<int, std::string> inGameGuiMap;
    std::map<int, bool> inGameGuiStatusMap;
    std::map<int, ScoreBoardInfo_t> scoreBoardMap;
    std::map<std::string, int> itemListMap;
    std::map<int, std::string> staticItemList;
    std::map<int, std::string> weaponImageList;


    float gameMessageAlpha, gameInfoAlpha, shotAlpha;
    bool gameMessageFlag, gameInfoFlag, shotFlag;
	bool hiding;
	bool showing;
	bool autoHide;
	float size;
	Ogre::Timer hideTimer, shotTimer;

    int gameInfoTimer;
    
    void createGUIs();

};

    //keyReleased
 //CEGUI::KeyEventArgs* keyEvent = new CEGUI::KeyEventArgs(CEGUI::WindowManager::getSingleton().getWindow("Main"));
   //return m_pSystem->injectKeyUp(keyEvent->scancode);
#endif	/* _INGAMEGUIHANDLER_H */

