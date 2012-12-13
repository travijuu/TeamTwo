/*
	Copyright (C) 2011-2012 Alican Sekerefe

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

#ifndef GAMECORE_H
#define GAMECORE_H
#include "stdafx.h"


#include "CameraController.h"
#include "GameMessageController.h"
#include "GameMessageInterface.h"
#include "NetworkUtil/UdpClient.h"
#include "NetworkUtil/UdpListener.h"
#include "NetworkUtil/TcpCallbackInterface.h"
#include "NetworkUtil/TcpConnector.h"
#include "NetworkUtil/MessageStructure.h"
#include "Util/Logger.h"
#include "Util/ConfigReader.h"
#include "Util/ConfigWriter.h"
#include "MapUtil/MazeBuilder.h"
#include "Util/Converter.h"
#include "Player.h"
#include "GameCoreInitializer.h"
#include "Menu/MenuController.h"

#include "Menu/GraphicSettingsMenu.h"
#include "Menu/NetworkSettingsMenu.h"
#include "Menu/MultiplayerSettingsMenu.h"
#include "Menu/LobbyMenu.h"
#include "Menu/CreditsMenu.h"
#include "Menu/QuitMenu.h"
#include "Menu/MenuActionCallbackInterface.h"
#include "Menu/MenuController.h"
#include "OperationScheduler.h"
#include "GameItem.h"
#include "Teleporter.h"
#include "SoundController.h"
#include "Menu/InGameGUIHandler.h"


#define MOVE_ANIMATION_SPEED .05


using namespace Ogre;
using namespace OIS;

/* Game Core
 *
 * This class is the center of the game 
 * application which controls everything
 * such as -mainly- graphics, GUI,
 * Input/Output and sounds.
 *
 * It inherits the necessary callback
 * interfaces of the game engine Ogre3D
 * for performing graphics functionality.
 * GameMessageInterface is inherited
 * to enable GameMessageController to report
 * GameCore with its callback interface at
 * every necessary event. Menu callback
 * interfaces are also inherited for handling
 * menu events such as connecting to a server
 * or changing a graphics settings that should
 * be applied immediately.
 */

class GameCore: public FrameListener,public WindowEventListener,public KeyListener,public MouseListener,public GameMessageInterface,public LobbyCallbackInterface,public SettingsCallbackInterface
{
public:
	//constructor of GameCore. takes the configuration file as the parameter
	//if the configuration file is not found, application will be terminated
    GameCore(std::string gameConfigFile);
	//starts the 3D application
    void startOgre();
	//core of the application. updates both graphics and game content
    void gameLoop();

	
	//==================================GameCore INITIALIZERS====================================
	//===========================================================================================


	//initializes the logging mechanism (Logger). takes the file name that is going to be
	//created and used for file-level logging
    void initializeLogger(std::string gameLogFile);
	//initializes the CEGUI components for GUI
    void initializeCEGUI();
	//initializes event listeners such as window and frame events
    void initializeEventListeners();
	//initializes the scene objects
    void initializeScene();
	//initializes the physics engine components
    void initializePhysics();
	//initializes GameMessageController
    void initializeNetworking();
	//loads a map to the scene according to the given map file name as the parameter
    void initializeMap(std::string mapFile);
	//initializes menu's. note that this method must be called after invoking initializeCEGUI()
    void initializeMenu();
	//initializes ingamegui components
	void initializeInGameGUI();
	//initializes players of the game. creates both visual and physical components related to
	//the character models
    void initializePlayers();
	//initializes the game in single player mode. used for debug purposes
    void _initializeSinglePlayer();
	//initializes map items (immortality, guns, health etc.) from the MapReader
    void initializeItems();
	//initializes teleporter nodes from the MapReader
    void initializeTeleporters();
	//initializes sound handlers
    void initializeSounds();
	
	//===========================================================================================
	//===========================================================================================

	//scans the given directory in order to find TTM encoded map files. then, loads them into a map
    void lookupMapFiles(std::string path);
	//handles transformations of the local player and updates the related graphics
    void handlePlayerMovements();
	//handles actions(fire events etc.) of the local player and updates the related graphics
    void handlePlayerActions();
	//processes the collision events of the current simulation step
    void handleCollisions();	
	//updates game content; graphics, transform updates etc. are handled by this method
	//usually, called 50 times per second by the gameLoop()
    void updateGameContent();
	//creates a player by considering the information provided with GameClient object
    void createPlayer(GameClient* client);
	//destroys a player from the game; deletes its physical and graphical units
    void destroyPlayer(unsigned int playerID);
	//destroys the local player graphics and physics. in addition to destroyPlayer()
	//this method also handles the CameraController
    void destroyLocalPlayer();
	//creates an item by considering the given information as parameters
	//this method creates both physical and graphical contents of an item
    void createItem(int itemID, ItemType::ItemTypes itemType, std::string meshName, std::string materialName, Ogre::Vector3 meshScale, Ogre::Vector3 position);
    //creates a decal at the given point by cosidering its orientation by taking normals
	void createDecal(Ogre::Vector3 position,Ogre::Vector3 normal);
	//destorys the graphical and physical contents of an item with the given ID
    void destroyItem(int itemID);
	//destroys all items from the scene. uses destroyItem()
    void destroyItems();
	//destroys all decals on the scene.
    void destroyDecals();
	//destroys the built map contents from the scene.
    void destroyMap();
	//sets the value of gameLoaded boolean. this method created for operation scheduler
	void setGameLoaded(bool value);
	//enables/disables eagle eye mode
    void setEagleEyeMode(bool status);
	//applies the incoming player transfomation updates of remote players
    void updatePlayerPositions();
	//updates the graphics of items
    void updateItemGraphics();
	//updates the graphics of teleporters
    void updateTeleporterGraphics();
	//updates bullet positions per bullet
    void updateBulletPositions();
	//switches the application states between menu-mode and game-mode
    void switchToMenu(bool value);
	//enables/disables lighting in the game. modifies the ambient light
    void setEnableLighting(bool value);

	//schedules a transform update for a remote player. used by threads other than the 
	//graphics(main) thread
    void scheduleTransformUpdate(PositionUpdateMessage_t* message);
    void _scheduleTransformUpdate(PositionUpdateMessage_t* message);
	//adds a bullet to the waiting list. called by networking threads
    void addBullet(BulletInfo_t* info);
	//enables or disables loading menu
	void setLoadingMenu(bool enabled);

    //=========================================GMC EVENTS========================================
	//===========================================================================================
	
	//event method of GMC. called automatically when the local server discovery is finalized.
    void localDiscoveryFinalized(std::map<std::string,std::string> serverList);
	//GMC event method. called when the connection with the given server IP is established
	//and authenticated
    void connectionAccepted();
	//GMC event method. called whenever the server connection attempt is failed
	void connectionFailed();
	//GMC event method. called when a GameRoom information is received from the server
    void gameRoomInfoReceived(GameRoom* gameRoom);
	//GMC event method. called when a GameRoom is created by the local user.
    void gameRoomCreated();
	//GMC event method. called when the connection with a GameRoom is established
    void gameRoomConnectionEstablished();
	//GMC event method. called when the connection with the game room has been terminated
	void gameRoomConnectionTerminated();
	//GMC event method. called when the requested join is denied by the lobby
	void gameRoomConnectionDenied();
	//GMC event method. called when a client has joined to the current GameRoom
    void gameRoomClientJoined(GameClient* client);
	//GMC event method. called when a client has left from the current GameRoom
    void gameRoomClientLeft(GameClient* client);
	//GMC event method. called when the status of a client has changed
    void gameRoomClientReadyStatusChanged(GameClient* client);
	//GMC event method. called when a chat message from a client
    void gameRoomClientMessageReceived(GameClient* client, std::string message, bool teamChat);
	//GMC event method. called when the nickname of a client has changed
    void gameRoomClientNickChanged(GameClient* client,std::string oldNick);
	//GMC event method. called when a GameRoom has been destroyed by the server
    void gameRoomRemoved(unsigned int id);
	//GMC event method. called when the game is starting
    void gameStarting();
	//GMC event method. called when an item has been taken
    void itemTakeEvent(int itemID);
	//GMC event method. called when an item's status has been changed.
	//parameters indicate the status of an item with the provided ID
    void itemStatusChanged(int itemID,bool alive);
	//GMC event method. called when a player has been killed. killer's ID is also sent
    void playerKilledEvent(int playerID,int killerID);
	//GMC event method. called when a player has spawned
    void playerResurrectedEvent(int playerID);
	//GMC event method. called when a player has switched he/she's weapon
    void playerWeaponSwitchEvent(int playerID,int weaponType);
	//GMC event method. called when a new spawn point has been received from the server
    void nextSpawnPointEvent(double x, double y, double z);
	//GMC event method. called when the current owner of a GameRoom has been changed
    void gameRoomOwnerChanged(GameClient* client);
	//GMC event method. called when the server requests to reset the position of the player
	//this method is generally used for synchronizing the game start
    void resetPositionEvent();
	//GMC event method. called when an special item that the local player using is expired
    void itemExpiredEvent(int itemID);
	//GMC event method. called when the teleporter with the given ID is activated
    void teleporterActivated(int teleporterID);
	//GMC event method. called when the teleporter with the given ID is deactivated
    void teleporterDeactivated(int teleporterID);
	//GMC event method. called when the teleport usage with the given ID is accepted
    void teleporterUsageAccepted(int teleporterID);
	//GMC event method. called when a special item is used by a player with the given ID
    void playerSpecialItemUsedEvent(unsigned int playerID,int itemID);
	//GMC event method. called when a special item was expired that was used by a 
	//player with the given ID
    void playerSpecialItemExpiredEvent(unsigned int playerID,int itemID);
	//GMC event method. called when the score of the given player is updated
    void playerScoreUpdated(GameClient* client);
	//GMC event method. called when the state of a game room has been changed
	//this changes are usually the player count of the current game
    void gameRoomStateChanged(GameRoom *room);
	//GMC event method. called whenever a player changes his/her team
	void playerTeamChanged(GameClient* client);
	//GMC event method. called whenever a player  has been kicked by the room owner
	void gameRoomClientKicked(GameClient* client);
	//GMC event method. called when the local client has been kicked from the room
	void gameRoomLocalClientKicked();
	//GMC event method. called when a client changes his/her color
	void playerColorChanged(GameClient* client);

	
	//===========================================================================================
	//===========================================================================================



    //=========================================MENU EVENTS=======================================
	//===========================================================================================

	//MenuController event method. connects to the given IP address
	void lobbyServerConnectEvent(std::string ipAddress);
	//MenuController event method. enables local server discovery protocol
    void lobbyEnableLocalServerDiscoveryEvent();
	//MenuController event method. disconnects from the server
    void lobbyExitEvent();
	//MenuController event method. sends a connection request of a game room to the server
    void lobbyGameRoomConnectEvent(int gameId);
	//MenuController event method. sends a game room creation request to the server
	//with the given parameters such as its name, capacity, game type etc.
    void lobbyGameRoomCreateEvent(std::string gameName, std::string gameMap,unsigned char gameType, int gameCapacity);
	//MenuController event method. refreshes the gameroom list located on the lobby
    void lobbyGameRoomRefreshEvent();
	//MenuController event method. sends a chat message to the server
    void lobbyGameRoomSendMessageEvent(std::string message);
	//MenuController event method. notifies the ready-status changed of the local player to the server
    void lobbyGameRoomReadyStatusChangeEvent(bool ready);
	//MenuController event method. requests to kick a player with the given ID from the server
    void lobbyGameRoomKickPlayer(int playerId);
	//MenuController event method. disconnects from the current game room
    void lobbyGameRoomExitEvent();
	//MenuController event method. sends a gamelist refresh request to the server
    void lobbyRefreshServerListEvent();
	//MenuController event method. sends a game start request to the server
    void lobbyGameRoomStartEvent();
	//MenuController event method. called when the network settings are changed
    void networkSettingsChangedEvent();
	//MenuController event method. called when the graphics settings are changed
    void graphicSettingsChangedEvent();
	//MenuController event method. called when the multiplayer settings are changed
    void multiplayerSettingsChangedEvent();
	//MenuController event method. called when the current map of the game room is changed
    virtual void lobbyGameRoomMapChangedEvent(){};
	//MenuController event method. called whenever the local player changes his/her team
	void lobbyGameRoomTeamChangedEvent();


    //===========================================================================================
	//===========================================================================================

	
    //=========================================OTHER EVENTS======================================
	//===========================================================================================

    //FrameListener method of OGRE framework. called before the frame is created
	//currently, it calculates the FPS 
    bool frameRenderingQueued(const FrameEvent& evt);
    //OGRE framework event method. called after the size of the current window is changed
    void windowResized(RenderWindow* rw);
    //OGRE framework event method. called after the window is closed
    void windowClosed(RenderWindow* rw);
    //keyboard key-press event of OIS
    bool keyPressed(const KeyEvent &arg);
    //keyboard key-release event of OIS
    bool keyReleased(const KeyEvent &arg);	
    //mouse move event of OIS
    bool mouseMoved(const MouseEvent &arg);
    //mouse key-press event of OIS
    bool mousePressed(const MouseEvent &arg, MouseButtonID id);
    //mouse key-release event of OIS
    bool mouseReleased(const MouseEvent &arg, MouseButtonID id);
	
	//===========================================================================================
	//===========================================================================================


    //===================================VARIABLES===============================================
	//===========================================================================================

	InGameGUIHandler* ingameGUI;
    std::string gameConfigFile;
    Logger* logger;
    CameraController* cameraController;
    SoundController* soundController;
    MenuController* menuController;
    LobbyMenu* lobbyMenu;
    GraphicSettingsMenu* graphicSettingsMenu;
    NetworkSettingsMenu* networkSettingsMenu;
    MultiplayerSettingsMenu* multiplayerSettingsMenu;
    CreditsMenu* creditsMenu;
    QuitMenu* quitMenu;
    CEGUI::OgreRenderer* renderer;
    Root* root;
    Camera* mainCamera;
    Camera* menuCamera;
    SceneManager* sceneManager;
    SceneManager* menuSceneManager;
    RenderWindow* renderWindow;
    InputManager* inputManager;
    MazeBuilder* mazeBuilder;
    Mouse* mouse;
    Keyboard* keyboard;
    ConfigReader* reader;
    ConfigWriter* writer;
    btCollisionObject* terrainObject;
    std::string nickname;
    int ttftpPort;
    int serverTcpPort;
    float wallHeight;
    bool singlePlayerMode;
    bool buildMap;
    bool shadowsEnabled;
    std::string mapFileName;
    std::string menuBackgroundMusic;
    std::string gameBackgroundMusic;
    std::string lmgFireSound;
    float moveSpeed;
    float moveAnimationSpeed;
    float fireAnimationSpeed;
    bool gameStarted;
    int maxDecals;
    Player* localPlayer;
    std::map<unsigned int,Player*> playerMap;
    std::map<int,GameItem*> itemMap;
    std::map<int,Teleporter*> teleporterMap;
    std::vector<Bullet*> bulletVector;
    std::vector<BulletInfo_t*> bulletAddQueue;
    std::queue<SceneNode*> hitDecalQueue;
    std::map<SceneNode*,OgreBulletDynamics::RigidBody*> decalRigidMap;
    Viewport* viewPort;
    std::string _serverIP;
    GameMessageController* gmc;
    bool firing;
    bool menuMode;
    float sensivity;
    OgreBulletDynamics::DynamicsWorld* dynamics;
    OgreBulletCollisions::DebugDrawer* debugDrawer;    
    std::vector<std::string> mapNameVector;
    std::string mapPath;
    bool mapMode;
    OperationScheduler scheduler;
    bool teamDM;
    float bgSoundVolume;
	bool gameLoaded;
	unsigned char color;
	
	//===========================================================================================
	//===========================================================================================
	


	
	
	//====================================FIX========================================
	//====================================FIX========================================
	//====================================FIX========================================
	//====================================FIX========================================
	
    void createTerrain();
	//=====================
    void initializeTerrain(std::string sceneFile);
	//
    void disposePlayer(unsigned int id);
    SceneNode* createWall(float x, float y, float horizontalRotation, Ogre::Vector3 size);
    //temp
    unsigned int roomID;
    void itemCreateEvent(int itemID, int itemType, float posX, float posY, float posZ);


};

#endif