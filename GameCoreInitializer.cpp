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

#include <OGRE/Terrain/OgreTerrainGroup.h>

#include "stdafx.h"
#include "GameCore.h"


GameCore::GameCore(std::string gameConfigFile)
{
    this->gameConfigFile=gameConfigFile;
}

void GameCore::initializeSounds()
{
    soundController = SoundController::getSingleton();
    //soundController->setBackgroundSound(menuBackgroundMusic);
    //soundController->playBackgroundSound();
    //soundController->setBackgroundSoundVolume(bgSoundVolume);
}


void GameCore::initializeLogger(std::string gameLogFile)
{
    logger=Logger::getSingleton();
    logger->setFileName(gameLogFile);
    logger->setConsolePrint(true);
    logger->begin();
    logger->addLine("Ogre started");
    logger->addLine("Logger initialized");
}


void GameCore::initializePhysics()
{
    //compilation from tutorial...
	
	Logger::getSingleton()->addLine("GameCore: Initializing physics");
    Ogre::Vector3 gravity = Ogre::Vector3(0, -9.82, 0);
    Ogre::AxisAlignedBox box = Ogre::AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000), Ogre::Vector3(10000, 10000, 10000));
    dynamics = new OgreBulletDynamics::DynamicsWorld(sceneManager, box, gravity);

    debugDrawer = new OgreBulletCollisions::DebugDrawer();
    debugDrawer->setDrawWireframe(true);
    dynamics->setDebugDrawer(debugDrawer);
    SceneNode *node = sceneManager->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject((SimpleRenderable*)debugDrawer);
    
    if (reader->getFieldValueAsString("Debug", "debug-shape") == "yes")
        dynamics->setShowDebugShapes(true);
	Logger::getSingleton()->addLine("GameCore: ---ok");
}

void GameCore::initializeNetworking()
{
    gmc = new GameMessageController(this, serverTcpPort, ttftpPort);
    gmc->setNickname(nickname);
	gmc->setColor(color);
    gmc->setFilePath(mapPath);
    gmc->startFileServer();
    for(int i=0;i<mapNameVector.size();i++)
        gmc->shareFile(mapNameVector.at(i));
}

void GameCore::startOgre()
{
    reader=new ConfigReader(gameConfigFile);
    if(reader->readFile())
    {
        writer=new ConfigWriter(gameConfigFile);
        writer->importConfigReader(reader);
        reader->pushConfigFile("GameSettings");
        std::string pluginsConfig = reader->getFieldValueAsString("ConfigFiles", "plugins-configfile");
        std::string resourcesConfig = reader->getFieldValueAsString("ConfigFiles", "resources-configfile");
        std::string ogreConfig = reader->getFieldValueAsString("ConfigFiles", "ogre-config");
        std::string ogreLogFile = reader->getFieldValueAsString("LogFiles", "ogre-logfile");
        std::string gameLogFile = reader->getFieldValueAsString("LogFiles", "game-logfile");

        serverTcpPort= reader->getFieldValueAsInteger("NetworkSettings", "tcp-port");
        ttftpPort = reader->getFieldValueAsInteger("NetworkSettings", "ttftp-client-port");
        nickname=reader->getFieldValueAsString("MultiplayerSettings","nickname");
        wallHeight = reader->getFieldValueAsDouble("MapSettings", "wall-height");
        mapPath = reader->getFieldValueAsString("MapSettings", "map-lookup-directory");
        shadowsEnabled = reader->getFieldValueAsBool("GraphicsSettings", "shadows");
        menuBackgroundMusic = reader->getFieldValueAsString("GameSounds", "menu-background");
        gameBackgroundMusic = reader->getFieldValueAsString("GameSounds", "game-background");
        lmgFireSound = reader->getFieldValueAsString("GameSounds", "lmg-fire");
        bgSoundVolume=reader->getFieldValueAsDouble("GameSounds","bg-sound-volume");
        bgSoundVolume=(bgSoundVolume<-1?.1:bgSoundVolume);
		(reader->getFieldValueAsString("MultiplayerSettings","player-color")=="blue"?color=0:color=1);

        maxDecals=reader->getFieldValueAsInteger("GraphicsSettings","max-decals");
        if(maxDecals==-1)
            maxDecals=30;

        sensivity=((reader->getFieldValueAsDouble("MultiplayerSettings","mouse-sensivity")<=0?5:reader->getFieldValueAsDouble("MultiplayerSettings","mouse-sensivity")));


        moveAnimationSpeed = reader->getFieldValueAsDouble("AnimationSpeeds", "move-speed");
        fireAnimationSpeed = reader->getFieldValueAsDouble("AnimationSpeeds", "fire-speed");
        moveSpeed=125;

        buildMap = true;
        singlePlayerMode = reader->getFieldValueAsBool("Debug", "singleplayer");
        if (buildMap)
            mapFileName = reader->getFieldValueAsString("MapSettings", "map-file-name");
        if (mapPath == "NULL")
            mapPath = "Map/";
        else
            mapPath = mapPath.find("/") == std::string::npos ? mapPath.substr(mapPath.find("/")) : mapPath;
		
        gameStarted = teamDM = gameLoaded=false;

        initializeLogger(gameLogFile);
        logger->addLine("game config file " + gameConfigFile + " loaded to memory & logger initialized");
		LogManager* logManager=new LogManager();
		logManager->createLog(ogreLogFile,true,false,false);
        root = OGRE_NEW Root(pluginsConfig,ogreConfig, "");
        ConfigFile configFile;
        configFile.load(resourcesConfig);

        //[start]taken from ogre framework
        Ogre::ConfigFile::SectionIterator seci = configFile.getSectionIterator();
        Ogre::String secName, typeName, archName;
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
            Ogre::ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
			}
        }
        //[END]

            logger->addLine("ogre configuration has been read");
        root->showConfigDialog();
        renderWindow=root->initialise(true,"Team-Two");
            logger->addLine("window created");
        sceneManager=root->createSceneManager(ST_EXTERIOR_FAR,"core");
        localPlayer=NULL;
        mapMode=false;



        mainCamera=sceneManager->createCamera("MainCam");
        
        mainCamera->setNearClipDistance(1);mainCamera->setLodBias(2);
        mainCamera->setFarClipDistance(100000000);
        mainCamera->setAspectRatio(renderWindow->getWidth()/(float)renderWindow->getHeight());


        menuSceneManager=root->createSceneManager(ST_GENERIC,"menu");
        menuCamera=menuSceneManager->createCamera("MenuCam");
        menuCamera->setNearClipDistance(1);
        menuMode=true;


        viewPort=renderWindow->addViewport(mainCamera);
        dynamics=NULL;


        if(shadowsEnabled)
            sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
        else
            sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

        ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

		logger->addLine("GameCore: scanning map files");
        lookupMapFiles(mapPath);
		logger->addLine("GameCore: --ok");
		logger->addLine("GameCore: initializing event listeners");
        initializeEventListeners();
		logger->addLine("GameCore: --ok");
		logger->addLine("GameCore: initializing CEGUI");
        initializeCEGUI();
		logger->addLine("GameCore: --ok");
		logger->addLine("GameCore: initializing menu");
        initializeMenu();
		menuController->showLoading();	
		root->renderOneFrame();
#if WINDOWS_PLATFORM
		//Sleep(5000);
#else 
		sleep(5);
#endif
		logger->addLine("GameCore: --ok");	
		logger->addLine("GameCore: initializing ingamemenu");
		initializeInGameGUI();
		logger->addLine("GameCore: --ok");	
		logger->addLine("GameCore: initializing networking");
        initializeNetworking();
		logger->addLine("GameCore: --ok");

        //=================================================DEBUG
        if (singlePlayerMode)
        {
            initializePhysics();
            logger->addLine("Physics initialized");
            initializeScene();
            logger->addLine("Scene initialized");
            if (buildMap)
                initializeMap(mapPath + mapFileName);
            logger->addLine("Map initialized");
            _initializeSinglePlayer();
            initializeItems();
            initializeTeleporters();

        }
             
         //=================================================
		logger->addLine("GameCore: initializing sounds");
        initializeSounds();
		logger->addLine("GameCore: --ok");
		root->addFrameListener(this);
        logger->addLine("GameLoop starting...");
		menuController->hideLoading();
        gameLoop();
    }
    else
        std::cout<<"configuration file not found!...\n";
}

void GameCore::initializeMenu()
{
	menuController = new MenuController("MainMenu");
    lobbyMenu = new LobbyMenu("Lobby",menuController,this);
    multiplayerSettingsMenu = new MultiplayerSettingsMenu("Multiplayer",menuController,this);
    networkSettingsMenu = new NetworkSettingsMenu("Network", menuController,this);
    graphicSettingsMenu = new GraphicSettingsMenu("Graphics", menuController,this);
    creditsMenu = new CreditsMenu("Credits",menuController,this);
    quitMenu = new QuitMenu("Quit", menuController);

    menuController->addOption("Lobby", lobbyMenu);
    //menuController->addSubOption("CreateGame","Lobby",lobbyMenu);
    menuController->addOption("Settings");
    menuController->addSubOption("Multiplayer","Settings", multiplayerSettingsMenu);
    menuController->addSubOption("Network","Settings", networkSettingsMenu );
    menuController->addSubOption("Graphics","Settings", graphicSettingsMenu);
    
    menuController->addOption("Credits", creditsMenu);
    menuController->addOption("Quit", quitMenu);

    for(int i=0;i<mapNameVector.size();i++)
        lobbyMenu->addMap(mapNameVector.at(i),"1");
    menuController->showMain();

    lobbyMenu->setIPAddress(reader->getFieldValueAsString("NetworkSettings","default-server-ip"));
    networkSettingsMenu->setServerPort(reader->getFieldValueAsString("NetworkSettings","tcp-port"));
    networkSettingsMenu->setTTFTP(reader->getFieldValueAsString("NetworkSettings","ttftp-client-port"));

    multiplayerSettingsMenu->setPlayerName(nickname);
    multiplayerSettingsMenu->setMouseSensivity(sensivity);
	multiplayerSettingsMenu->setColor(color==0?1:0);

    graphicSettingsMenu->setShadow(shadowsEnabled);
    graphicSettingsMenu->setLight(ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GraphicsSettings","generate-lights")=="no"?false:true);
    graphicSettingsMenu->setSpotLight(ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GraphicsSettings","spot-lights")=="no"?false:true);
}

void GameCore::initializeCEGUI()
{
    renderer = &CEGUI::OgreRenderer::bootstrapSystem();
    CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
}

void GameCore::initializeInGameGUI()
{	
	ingameGUI = new InGameGUIHandler(renderWindow);
    ingameGUI->enable(InGameGUIHandler::HEALTH);
    ingameGUI->enable(InGameGUIHandler::WEAPON);
    ingameGUI->enable(InGameGUIHandler::SCORE_BOARD_TM,false);
    ingameGUI->enable(InGameGUIHandler::SCORE_BOARD_DM,false);
    ingameGUI->enable(InGameGUIHandler::CHAT,false);
    ingameGUI->enable(InGameGUIHandler::INGAME_TEXT);
    ingameGUI->enable(InGameGUIHandler::GAME_INFO);
    ingameGUI->enable(InGameGUIHandler::ITEM_LIST);


    ingameGUI->addItemIcon(ItemType::EAGLE_EYE, "images/eagle_eye.png");
    ingameGUI->addItemIcon(ItemType::ONE_SHOT, "images/one_shot.png");
    ingameGUI->addItemIcon(ItemType::IMMORTALITY, "images/weapon.png");

    ingameGUI->addWeaponIcon(WeaponSlot::Primary, "images/weapon1.png");
    ingameGUI->addWeaponIcon(WeaponSlot::Secondary, "images/weapon2"
    ".png");
    ingameGUI->changeWeaponIcon(WeaponSlot::Primary);
}

class MotionListener:public btMotionState
{
private:
    OgreBulletCollisions::Object *mObject;
    btTransform mWorldTrans;
    SceneNode* node;
public:

    MotionListener(OgreBulletCollisions::Object *parent, SceneNode* node) : mObject(parent) {
        this->node = node;
    }

    virtual void getWorldTransform(btTransform& worldTrans) const {
        worldTrans.setOrigin(OgreBulletCollisions::OgreBtConverter::to(mObject->getWorldPosition()));
        worldTrans.setRotation(OgreBulletCollisions::OgreBtConverter::to(mObject->getWorldOrientation()));
    }

    virtual void setWorldTransform(const btTransform& worldTrans)
    {
        //std::cout<<"setWorldTransform...: "<<OgreBulletCollisions::BtOgreConverter::to(worldTrans.getOrigin())<<" \n";
        //std::cout<<"setWorldTransform...: "<<OgreBulletCollisions::BtOgreConverter::to(worldTrans.getRotation())<<" ";
        mObject->setTransform(worldTrans);
        mWorldTrans = worldTrans;
        //std::cout<<"ok.\n";
    }
};


void GameCore::initializeTerrain(std::string sceneFile)
{
}

void GameCore::setEnableLighting(bool value)
{
    if(value)
        sceneManager->setAmbientLight(Ogre::ColourValue(.01,.01,.01));
    else
        sceneManager->setAmbientLight(Ogre::ColourValue(.3,.3,.3));
}

void GameCore::destroyMap()
{
    if(mapMode && mazeBuilder!=NULL)
    {
        mazeBuilder->destroyAll();
        delete mazeBuilder;
    }
}

void GameCore::initializeMap(std::string mapFile)
{
    mazeBuilder=new MazeBuilder(mapFile,sceneManager,dynamics);
    mazeBuilder->readMap();
    mazeBuilder->generateMap(2,wallHeight,1000,true);
    mazeBuilder->generateLights();
    if(ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GraphicsSettings","generate-lights")=="yes")
        setEnableLighting(true);
    else
        setEnableLighting(false);

    mapMode=true;   
}

void GameCore::initializeTeleporters()
{
    MapReader* mapReader = mazeBuilder->getMapReader();
    Entity4Position_t* pos;
    int teleporterId = 0;
    Teleporter* teleporter;
    Ogre::Vector3 inPos;
    Ogre::Vector3 outPos;
    float scaleFactor = mapReader->getScaleFactor();
    while ((pos = mapReader->getTeleportEntity()) != NULL)
    {
        inPos = Ogre::Vector3(-pos->startX, 0, pos->startZ) * scaleFactor;
        outPos = Ogre::Vector3(-pos->endX, 0, pos->endZ) * scaleFactor;
        teleporter = new Teleporter(teleporterId, inPos, outPos);
        teleporter->createVisual(sceneManager);
        teleporter->createPhysics(dynamics);
        teleporterMap[teleporterId++] = teleporter;
    }
}

void GameCore::initializeItems()
{
    if(mapMode)
    {
        MapReader* mapReader = mazeBuilder->getMapReader();
        int itemId = 0;
        if (mapReader != NULL)
        {
            float scaleFactor = mapReader->getScaleFactor();
            MapEntity2_t* ent;
            std::string meshName="LaserRifle.mesh";
            std::string materialName="LaserRifleItem";
            Ogre::Vector3 meshScale(2,2,2);
            while ((ent = mapReader->getItem()) != NULL)
            {
                if (ent->type == ItemType::LASER_MACHINE_GUN)
                {
                    meshName = "LaserRifle.mesh";
                    materialName = "LaserRifleItem";
                    meshScale=Ogre::Vector3(2,2,2);
                }
                else if(ent->type==ItemType::LASER_PISTOL)
                {
                    meshName="cube.mesh";
                    materialName="LaserPistolItem";
                    meshScale=Ogre::Vector3(.05,.05,.05);
                }
                else if(ent->type==ItemType::EAGLE_EYE)
                {
                    meshName="EagleEye.mesh";
                    materialName="NULL";
                    meshScale=Ogre::Vector3(4,4,4);
                }
                else if(ent->type==ItemType::EXTRA_HEALTH)
                {
                    meshName="Medkit.mesh";
                    materialName="NULL";
                    meshScale=Ogre::Vector3(12,12,12);
                }
				else if(ent->type==ItemType::IMMORTALITY)
				{
					meshName="Immortality.mesh";
					materialName="NULL";
					meshScale=Ogre::Vector3(10,10,10);
				}
				else if(ent->type==ItemType::ONE_SHOT)
				{
					meshName="OneShot.mesh";
					materialName="NULL";
					meshScale=Ogre::Vector3(15,15,15);
				}
                else
                {
                    Logger::getSingleton()->addLine("GameCore: unidentified item read", true);
                    continue;
                }
				Ogre::Vector3 itemPos(-ent->entityPosition.startX*scaleFactor,0,ent->entityPosition.startZ*scaleFactor);
				Ogre::Vector3 startPos=itemPos;
				Ogre::Vector3 endPos=itemPos;
				startPos.y=mazeBuilder->getWallHeight()-10;
				endPos.y=-100;
				btCollisionObject object;
				Ogre::Vector3 hitPos;
				if(PhysicsHandler::performRaycastTest(startPos,endPos,dynamics,&object,&hitPos))
				{
					CollisionObjectInfo* info=(CollisionObjectInfo*)object.getUserPointer();
					std::cout<<"type: "<<info->getObjectType()<<std::endl;
					if(info->getObjectType()==CollisionObjectTypes::GROUND || info->getObjectType()==CollisionObjectTypes::STATIC_OBJECT)
						itemPos.y=hitPos.y;
					else if(info->getObjectType()==CollisionObjectTypes::WALL)
					{
						Logger::getSingleton()->addLine("GameCoreInitializer: item-wall collision detected; item removed",true);
						continue;
					}
				}
				itemPos.y+=10;
                createItem(itemId++, (ItemType::ItemTypes)ent->type, meshName, materialName, meshScale, itemPos);
            }
        }
    }
}

void GameCore::createItem(int itemID,ItemType::ItemTypes itemType,std::string meshName,std::string materialName,Ogre::Vector3 meshScale,Ogre::Vector3 position)
{
    GameItem* item = new GameItem(itemID, itemType);
    item->createVisual(sceneManager, meshName, materialName, meshScale, position);
    item->createPhysics(dynamics);
    itemMap[itemID] = item;
}

void GameCore::initializeScene()
{    
    Light* light = sceneManager->createLight("directionalLight");
    light->setType(Light::LT_DIRECTIONAL);
    light->setDirection(Ogre::Vector3(0, -1, 0));
    light->setDiffuseColour(.1, .1, .1);
    light->setSpecularColour(1, 1, 1);
    light->setPosition(-10, 10, 0);
    light->setVisible(false);
    SceneNode* lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(light);
    lightNode->setPosition(-5000, 5000, 0);
    lightNode->rotate(Ogre::Vector3::UNIT_X, (Radian) 30 * 3.14 / 180);


    float camDistance = reader->getFieldValueAsDouble("CameraSettings", "camera-distance");
    float camHeight = reader->getFieldValueAsDouble("CameraSettings", "camera-height");
    cameraController = new CameraController(sceneManager, mainCamera, camDistance, camHeight);


    Light* l = sceneManager->createLight();
    l->setDirection(0, -1, 0);
    l->setType(Light::LT_SPOTLIGHT);

    l->setDiffuseColour(0, .0, .2);
    l->setSpecularColour(0, 0, .1);
    l->setPosition(0, 100, 0);
    l->setVisible(false);

    sceneManager->setSkyBox(true, "Examples/SpaceSkyBox");


}

void GameCore::initializeEventListeners()
{
    //compiled from ogre framework
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    renderWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));




    inputManager = OIS::InputManager::createInputSystem( pl );

    keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject( OISKeyboard, true ));
    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject( OISMouse, true ));



    mouse->setEventCallback(this);
    keyboard->setEventCallback(this);

    windowResized(renderWindow);
    Ogre::WindowEventUtilities::addWindowEventListener(renderWindow, this);


}

void GameCore::createPlayer(GameClient* client)
{
	unsigned char team=(teamDM?client->getTeam():(client->getColor()==0?0:1));
	bool friendly=false;
	if(teamDM && gmc->getLocalClient()->getTeam()==client->getTeam())
		friendly=true;
    Player* player = new Player(client->getID(), client->getName(), false, friendly,team, sceneManager);
    player->createVisual("SpaceMarine.mesh", Quaternion::IDENTITY, Ogre::Vector3(2, 2, 2), Ogre::Vector3(-100, -100, -500));
    player->enablePhysics(dynamics);
    player->getAnimationController()->enableAnimation("Idle");
    player->getAnimationController()->addIdleAnimation("Run", "Idle");
    player->getAnimationController()->enableAnimation("Run");
    playerMap[client->getID()] = player;
}

void GameCore::destroyPlayer(unsigned int playerID)
{
    if(playerMap.count(playerID)>0)
    {
        delete playerMap[playerID];
        playerMap.erase(playerID);
        Bullet* bullet;
        std::vector<Bullet*>::iterator iter=bulletVector.begin();
        while(iter!=bulletVector.end())
        {
            if((*iter)->getOwnerID()==playerID)
                (*iter)->hit();
            iter++;
        }
    }
}

void GameCore::destroyLocalPlayer()
{
    cameraController->enableFreecam();
    delete localPlayer;
    localPlayer=NULL;
}

void GameCore::destroyItems()
{
    GameItem* item;
    std::map<int,GameItem*>::iterator iter=itemMap.begin();
    while(iter!=itemMap.end())
    {
        item=iter->second;
        delete item;
        iter++;
    }
    itemMap.clear();
}

void GameCore::destroyItem(int itemID)
{
    if(itemMap.count(itemID)>0)
    {
        GameItem* item=itemMap[itemID];
        itemMap.erase(itemID);
        delete item;
    }
}

void GameCore::createDecal(Ogre::Vector3 position,Ogre::Vector3 normal)
{/*
    static int decalID=0;
    btCollisionObject object;
    Ogre::Vector3 hitPos;
    CollisionObjectInfo* info;
  */
    Ogre::Vector3 scaleFactor(8,8,8);
    Entity* ent=sceneManager->createEntity("Decal.mesh");
    /*
    float x=ent->getBoundingBox().getSize().x/2.*scaleFactor.x;
    float y=ent->getBoundingBox().getSize().y/2.*scaleFactor.y;
    Ogre::Vector3 pointArr[]={Ogre::Vector3(-x,-y,0),Ogre::Vector3(-x,y,0),Ogre::Vector3(x,-y,0),Ogre::Vector3(x,y,0)
            ,Ogre::Vector3(-x/2.,-y/2.,0),Ogre::Vector3(-x/2.,y/2.,0),Ogre::Vector3(x/2.,-y/2.,0),Ogre::Vector3(x/2.,y/2.,0)};
    SceneNode* temp=sceneManager->getRootSceneNode()->createChildSceneNode();
    temp->setDirection(normal);
    for(int i=0;i<8;i++)
    {
        pointArr[i]=temp->getOrientation()*pointArr[i];
        if(PhysicsHandler::performRaycastTest(position+(normal*2)+pointArr[i],position-(normal*2)+pointArr[i],&object,dynamics,&hitPos))
        {
            info=(CollisionObjectInfo*)object.getUserPointer();
            if(info->getObjectType()==CollisionObjectTypes::HIT_DECAL)
            {
                temp->setPosition(position);
                temp->translate(0,0,.1,Ogre::Node::TS_LOCAL);
                decalRigidMap[info->getSceneNode()]->setPosition(temp->getPosition());
                return;
            }
        }
    }
  */

    SceneNode* decalNode=sceneManager->getRootSceneNode()->createChildSceneNode();
    ent->setMaterialName("BulletDamageDecal");
    ent->setCastShadows(false);
    decalNode->attachObject(ent);
    decalNode->scale(scaleFactor);
    decalNode->setPosition(position);
    decalNode->setDirection(normal);
    decalNode->translate(0,0,.1,Ogre::Node::TS_LOCAL);
    Ogre::Vector3 newPosition=decalNode->getPosition();
    decalNode->rotate(Ogre::Vector3::UNIT_X,Degree(90));
    hitDecalQueue.push(decalNode);
    if(hitDecalQueue.size()>maxDecals)
    {
        SceneNode* _decalNode=hitDecalQueue.front();
        hitDecalQueue.pop();
        sceneManager->destroySceneNode(_decalNode);
    }
/*
    OgreBulletCollisions::BoxCollisionShape* boxShape=PhysicsHandler::createBoxShape(ent,decalNode->getScale());
    OgreBulletDynamics::RigidBody* decalRigid=PhysicsHandler::addRigidBody("decal_"+Converter::intToString(decalID),decalNode,boxShape,dynamics,newPosition,0,decalNode->getOrientation());
    decalRigid->setKinematicObject(true);
    decalRigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(decalNode, decalRigid, CollisionObjectTypes::HIT_DECAL, NULL));
    decalRigidMap[decalNode]=decalRigid;
    decalID++;
 * */
}

void GameCore::initializePlayers()
{
	teamDM=(gmc->getCurrentRoom()->getGameType()==0?false:true);
	unsigned char clientTeam=(teamDM?gmc->getLocalClient()->getTeam():(gmc->getLocalClient()->getColor()==0?0:1));

    std::map<unsigned int,GameClient*> clientMap=gmc->getCurrentRoom()->getClientMap();
    std::map<unsigned int,GameClient*>::iterator iter=clientMap.begin();
    GameClient* client;
    while(iter!=clientMap.end())
    {
        client=iter->second;
        if(client!=gmc->getLocalClient())
            createPlayer(client);
        iter++;
    }
    client=gmc->getLocalClient();
	localPlayer=new Player(client->getID(),client->getName(),true,true,clientTeam,sceneManager);
    localPlayer->createVisual("SpaceMarine.mesh", Quaternion::IDENTITY, Ogre::Vector3(2, 2, 2), Ogre::Vector3(-100, -100, -500));
    localPlayer->enablePhysics(dynamics);
	localPlayer->changePlayerColor(clientTeam);
    localPlayer->getAnimationController()->enableAnimation("Idle");
    localPlayer->getAnimationController()->addIdleAnimation("Run","Idle");
    localPlayer->getAnimationController()->enableAnimation("Run");

    cameraController->setTarget(localPlayer->getSceneNode());
    float cameraShift =-reader->getFieldValueAsDouble("CameraSettings", "camera-shift");
	cameraController->moveCamera(cameraShift,0);

    cameraController->rotateCamera(90, 0);
    cameraController->rotateCamera(0, 15);
    gmc->requestNextSpawnPoint();
    soundController->setListener(localPlayer->getSceneNode());

	//ingameGUI->setGameType(InGameGUIHandler::TEAM_MATCH);
	this->ingameGUI->showItem(ItemType::IMMORTALITY);
	this->ingameGUI->showItem(ItemType::EAGLE_EYE);
	this->ingameGUI->showItem(ItemType::ONE_SHOT);
}


void GameCore::_initializeSinglePlayer()
{
	localPlayer=new Player(0,"SinglePlayer",true,true,Teams::BLUE,sceneManager);
    localPlayer->createVisual("SpaceMarine.mesh", Quaternion::IDENTITY, Ogre::Vector3(2, 2, 2), Ogre::Vector3(-100, -100, -500));
    localPlayer->enablePhysics(dynamics);
    localPlayer->getAnimationController()->enableAnimation("Idle");
    localPlayer->getAnimationController()->addIdleAnimation("Run","Idle");
    localPlayer->getAnimationController()->enableAnimation("Run");
    cameraController->setTarget(localPlayer->getSceneNode());
    float cameraShift = -reader->getFieldValueAsDouble("CameraSettings", "camera-shift");
    cameraController->moveCamera(cameraShift, 0);
    cameraController->rotateCamera(90, 0);
    cameraController->rotateCamera(0, 15);
    SoundController::getSingleton()->setListener(localPlayer->getSceneNode());
	gameLoaded=true;

    /*
    Entity* _ent=sceneManager->createEntity("ev.mesh");
    float y=_ent->getBoundingBox().getSize().y;
    SceneNode* _node=sceneManager->getRootSceneNode()->createChildSceneNode();
    _node->attachObject(_ent);
    _node->scale(15,15,15);
    OgreBulletDynamics::RigidBody* _rigid=PhysicsHandler::addStaticMeshRigidBody("ev",PhysicsHandler::createScaledTriangleMeshShape(_ent,_node->getScale()),
                                                                        dynamics,Ogre::Vector3(0,0,0),Quaternion::IDENTITY);
        _rigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(NULL, _rigid, CollisionObjectTypes::GROUND, NULL));
        //_node->setPosition(Ogre::Vector3(0,y*15./2.,0));
     * */
    
}

void GameCore::lookupMapFiles(std::string path)
{
    mapNameVector.clear();
#ifdef WINDOWS_PLATFORM

    WIN32_FIND_DATA fData;
    HANDLE fileHandle = FindFirstFileA((path + "*").c_str(), &fData);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        std::string fileName;
        do
        {
            fileName = std::string(fData.cFileName);

#else

    DIR* directory = opendir(path.c_str());
    if (directory != NULL)
    {
        struct dirent* dir = readdir(directory);
        std::string fileName;
        while (dir != NULL)
        {
            fileName = std::string(dir->d_name);
#endif

            if (fileName.find("ttm") != std::string::npos)
            {
                if (MapReader::isTTM(path + fileName)) 
                {
                    Logger::getSingleton()->addLine("GameCore: map file " + fileName + " added to the map list");
                    mapNameVector.push_back(fileName);
                }
                else
                    Logger::getSingleton()->addLine("GameCore: WARNING file " + fileName + " is not a TTM file!");
            }

#ifdef WINDOWS_PLATFORM

        }
        while (FindNextFileA(fileHandle, &fData) > 0);
    }
    else
        Logger::getSingleton()->addLine("GameCore: error opening map directory: " + path, true);

#else

            dir = readdir(directory);
        }
    }
    else
        Logger::getSingleton()->addLine("GameCore: WARNING map directory " + path + " is empty!");
#endif
}
