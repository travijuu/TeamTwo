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

#include "GameCore.h"

#include "PhysicsShapeHandler.h"

void GameCore::gameLoop()
{
    int maxFps=50;
    int fps=50;
    int frameCount=0;
    int count=0;
    int counter=0;
    double maxFpsTime=1000./maxFps;
    Ogre::Timer* timer=new Ogre::Timer();
    Ogre::Timer* fpsTimer=new Ogre::Timer();
    long nextTime=timer->getMilliseconds();
    time_t xTime=time(NULL);
    while(true)
    {
        while(timer->getMilliseconds()>nextTime)
        {
            count=ceil((timer->getMilliseconds())/maxFpsTime);
            //for(int i=0;i<count+1;i++)
            {
                updateGameContent();
                if(dynamics!=NULL)
                    dynamics->stepSimulation(1./(float)maxFps,(int)(Ogre::Math::Ceil(maxFps/((float)fps+.0001))+7),1./50);
				
            }
            nextTime+=maxFpsTime;
            //nextTime+=(count*maxFpsTime)+maxFpsTime;
            counter++;
            frameCount++;
        }

        counter=0;
                
        if(xTime==time(NULL))
        {
            xTime=time(NULL)+1;
            fps=frameCount;
            frameCount=0;
        }

        //if(fpsTimer->getMilliseconds()>=maxFpsTime)
        {
            //fpsTimer->reset();
            root->renderOneFrame();
        }
    }
}

void GameCore::updateGameContent()
{
    Ogre::WindowEventUtilities::messagePump();
    scheduler.run();
    keyboard->capture();
    mouse->capture();
    soundController->updateAllSoundPositions();
	if(!menuMode && gameLoaded)
    {
        static Ogre::Timer* timer=new Ogre::Timer();
        static float framesInSeconds = 0;
        static float posUpdateInterval = .5;
        static float lastFps=60;
        static double last=0;
        static double movement=0;
		 
		ingameGUI->update();
		if(localPlayer!=NULL)
		{
			ingameGUI->setHealth(localPlayer->getHealth());
			ingameGUI->setAmmo(localPlayer->getCurrentWeapon()->getClipAmmo());
			ingameGUI->setMag(localPlayer->getCurrentWeapon()->getTotalAmmo()-localPlayer->getCurrentWeapon()->getClipAmmo());
		}

		if (framesInSeconds >= 1)
        {
            posUpdateInterval = framesInSeconds / 60.0;
            framesInSeconds = 0;
        }

        static time_t currentTime = time(NULL);
        static int frames = 0;
        static double _time=0;
        static float totalSlerp = 0;
        if (currentTime == time(NULL))
            frames++;
        else
        {
            movement=0;
            last=0;
            lastFps=frames;
            frames = _time=0;
            totalSlerp = 0;
            currentTime = time(NULL);
        }

        updateItemGraphics();
        updateTeleporterGraphics();
        updatePlayerPositions();
        handleCollisions();
        updateBulletPositions();
        localPlayer->getCurrentWeapon()->update();

		static float _mv=0;
        if (keyboard->isKeyDown(KC_NUMPAD6))
		{
			_mv+=.1;
            cameraController->moveCamera(.1, 0);
		}
        else if (keyboard->isKeyDown(KC_NUMPAD4))
		{
			_mv-=.1;
			cameraController->moveCamera(-.1, 0);
		}
		else if(keyboard->isKeyDown(KC_TAB))
			ingameGUI->show(InGameGUIHandler::SCORE_BOARD);


		if (!cameraController->isFreecam() && !localPlayer->isDead() && !ingameGUI->isChatOpen())
        {
            handlePlayerMovements();
            handlePlayerActions();
        }
        localPlayer->getAnimationController()->updateAnimations(0);


        if (cameraController->isFreecam())
        {
            if (keyboard->isKeyDown(KC_W))
                cameraController->moveCamera(0, -1);
            else if (keyboard->isKeyDown(KC_S))
                cameraController->moveCamera(0, 1);

            if (keyboard->isKeyDown(KC_A))
                cameraController->moveCamera(-1, 0);
            else if (keyboard->isKeyDown(KC_D))
                cameraController->moveCamera(1, 0);

        }
        cameraController->updateCameraSlerp(0);

        if (localPlayer->getCurrentWeapon() != NULL)
            localPlayer->getCurrentWeapon()->update();

        timer->reset();
    }
}



bool GameCore::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    static int frames=0;
    static time_t currentTime=time(NULL);
    static bool showFps=reader->getFieldValueAsString("Debug","show-fps")=="yes"?true:false;
    if(showFps)
    {
        if(currentTime==time(NULL))
            frames++;
        else
        {
            Logger::getSingleton()->addLine("GameCore: FPS-> "+Converter::intToString(frames));
            frames=0;
            currentTime=time(NULL);
        }
    }
    
    return true;  
}

void GameCore::updateBulletPositions()
{
    for(int i=bulletAddQueue.size()-1;i>-1;i--)
    {
        BulletInfo_t* info = bulletAddQueue.at(i);
        bulletAddQueue.pop_back();
        Ogre::Vector3 pos(info->x, info->y, info->z);
        Quaternion q, q2;
        q.FromAngleAxis((Radian) info->angleY, Ogre::Vector3::UNIT_Y);
        q2.FromAngleAxis((Radian) info->angleX, Ogre::Vector3::UNIT_X);
        if(playerMap.count(info->header.clientID)>0)
        {
            Player* player=playerMap[info->header.clientID];
            Bullet* bullet = player->getCurrentWeapon()->createBullet(sceneManager, dynamics, pos, q * q2);
            player->getCurrentWeapon()->fire();
            bulletVector.push_back(bullet);
        }
        else
            Logger::getSingleton()->addLine("GameCore: bullet update failed! player does not exist!",true);
        delete info;
    }

    for (int i = 0; i < bulletVector.size(); i++)
    {
        Bullet* bullet = bulletVector.at(i);
        bullet->update(1);
        if (!bullet->isAlive())
            bulletVector.erase(bulletVector.begin() + i);
    }
}

void GameCore::updateTeleporterGraphics()
{
    std::map<int,Teleporter*>::iterator iter=teleporterMap.begin();
    while(iter!=teleporterMap.end())
    {
        iter->second->updateGraphics();
        iter++;
    }
}

void GameCore::updateItemGraphics()
{
    std::map<int,GameItem*>::iterator iter=itemMap.begin();
    while(iter!=itemMap.end())
    {
        iter->second->updateGraphics();
        iter++;
    }
}

void GameCore::updatePlayerPositions()
{
    std::map<unsigned int,Player*>::iterator iter=playerMap.begin();
    Player* remotePlayer;
    while(iter!=playerMap.end())
    {
        remotePlayer=iter->second;
        if (remotePlayer->needsUpdate)
        {
            remotePlayer->getSceneNode()->setPosition(remotePlayer->tempPosition);
            remotePlayer->getSceneNode()->resetOrientation();
            remotePlayer->getSceneNode()->rotate(Ogre::Vector3::UNIT_Y, (Radian) remotePlayer->tempAngleY);
            unsigned char animation = remotePlayer->tempAnimation;

            //std::cout<<(int)animation<<std::endl;
            if ((animation & PlayerAnimation::MOVING) !=0)
            {
                if ((animation & PlayerAnimation::BACKWARDS)!=0)
                    remotePlayer->getAnimationController()->updateOneFrame("Run", -moveAnimationSpeed);
                else
                    remotePlayer->getAnimationController()->updateOneFrame("Run", moveAnimationSpeed);
            }
            if ((animation & PlayerAnimation::FIRING)!=0)
            {
                //remotePlayer->getAnimationController()->enableAnimation("Fire");
                //localPlayer->getAnimationController()->playTillEnd("Fire", fireAnimationSpeed, true);
            }

            remotePlayer->needsUpdate = false;
        }
        remotePlayer->getAnimationController()->updateAnimations(0);
        remotePlayer->getCurrentWeapon()->update();
        iter++;
    }    
}


void GameCore::handlePlayerMovements()
{
    Quaternion targetOrientation;
    bool move = false;
    bool backwards = false;
    bool strafe = false;
    bool walking = false;
    static bool jumped = false;
    bool onGround = false;
    float slerpSpeed = .5;

    Ogre::Vector3 start = localPlayer->getSceneNode()->getPosition();
    Ogre::Vector3 end = localPlayer->getSceneNode()->getOrientation() * Ogre::Vector3(0, -20, 0) + localPlayer->getSceneNode()->getPosition();
    Ogre::Vector3 diff(0, localPlayer->getSceneNode()->getInitialPosition().y, 0);
    Ogre::Vector3 directionVector(0,0,0);
    end -= diff;
    btCollisionObject* object;
    onGround = localPlayer->isOnGround();

    static Quaternion lastOrientation = Quaternion::IDENTITY;



    if(!mouse->getMouseState().buttonDown(MB_Middle))
        targetOrientation=cameraController->getVerticalOrientation();
    else
        targetOrientation=localPlayer->getSceneNode()->getOrientation();

    if(keyboard->isKeyDown(KC_PGUP))
	{
        sceneManager->setAmbientLight(sceneManager->getAmbientLight()+ColourValue(.1,.1,.1));
        //moveSpeed+=.1;
		std::cout<<"light: "<<sceneManager->getAmbientLight()<<std::endl;
	}
    else if(keyboard->isKeyDown(KC_PGDOWN))
	{
        sceneManager->setAmbientLight(sceneManager->getAmbientLight()+ColourValue(-.1,-.1,-.1));
        //moveSpeed+=.1;
		std::cout<<"light: "<<sceneManager->getAmbientLight()<<std::endl;
	}
    
    if(keyboard->isKeyDown(KC_HOME))
        moveAnimationSpeed+=.001;
    else if(keyboard->isKeyDown(KC_END))
        moveAnimationSpeed-=.001;





    if (keyboard->isKeyDown(KC_LSHIFT))
        walking = true;

    if (keyboard->isKeyDown(KC_W) || keyboard->isKeyDown(KC_S) || keyboard->isKeyDown(KC_A) || keyboard->isKeyDown(KC_D))
    {
        int x=(keyboard->isKeyDown(KC_A)?-1:(keyboard->isKeyDown(KC_D)?1:0));
        int z=(keyboard->isKeyDown(KC_W)?-1:(keyboard->isKeyDown(KC_S)?1:0));
        backwards=keyboard->isKeyDown(KC_S)?true:false;
        strafe=keyboard->isKeyDown(KC_A)||keyboard->isKeyDown(KC_D)?true:false;
        directionVector=Ogre::Vector3(x,0,z);
        move = true;
    }

    btTransform transform = localPlayer->getRigidBody()->getBulletRigidBody()->getWorldTransform();
    float orientationDiff = fabs(targetOrientation.getYaw().valueDegrees() - localPlayer->getSceneNode()->getOrientation().getYaw().valueDegrees());
    if (orientationDiff > .05)
    {
        Quaternion inter = Quaternion::Slerp(slerpSpeed, localPlayer->getSceneNode()->getOrientation(), targetOrientation, true);
        transform.setRotation(OgreBulletCollisions::OgreBtConverter::to(inter));
        localPlayer->getRigidBody()->getBulletRigidBody()->setWorldTransform(transform);
    }

    directionVector.normalise();

    if (!jumped && keyboard->isKeyDown(KC_SPACE))
    {
        Ogre::Vector3 impulse=Ogre::Vector3(0,60,0)*directionVector;
        btVector3 velo=localPlayer->getRigidBody()->getBulletRigidBody()->getLinearVelocity();
        velo.setY(0);
        localPlayer->getRigidBody()->getBulletRigidBody()->setLinearVelocity(velo);
        localPlayer->getRigidBody()->getBulletRigidBody()->applyCentralImpulse(btVector3(0, 65,0));
        jumped = true;
    }
    else if(onGround)
        jumped=false;

    if(jumped)
        ;
    else if (!jumped && move)
    {
        directionVector = cameraController->getVerticalOrientation() * directionVector*moveSpeed;
        //directionVector=cameraController->getVerticalOrientation()*directionVector*moveSpeed*lastFrameTime;
        //localPlayer->getRigidBody()->getBulletRigidBody()->translate(OgreBulletCollisions::OgreBtConverter::to(directionVector));
        float _y=localPlayer->getRigidBody()->getBulletRigidBody()->getLinearVelocity().getY();
        directionVector.y=(_y>0?-_y:_y);
        localPlayer->getRigidBody()->getBulletRigidBody()->setLinearVelocity(OgreBulletCollisions::OgreBtConverter::to(directionVector));
        //localPlayer->getRigidBody()->getBulletRigidBody()->setLinearVelocity(OgreBulletCollisions::OgreBtConverter::to(directionVector));
    }
    else
    {
        btVector3 _vec=localPlayer->getRigidBody()->getBulletRigidBody()->getLinearVelocity();
        if(!jumped)
        {
            _vec.setX(0);
            _vec.setZ(0);
        }
        localPlayer->getRigidBody()->getBulletRigidBody()->setLinearVelocity(_vec);
    }


    if (move)
    {
        //localPlayer->getAnimationController()->disableAnimation("Idle");
        //localPlayer->getAnimationController()->enableAnimation("Run");
        localPlayer->getAnimationController()->updateOneFrame("Run", moveAnimationSpeed * (backwards ? -1 : 1));
    }
    else
    {
        //localPlayer->getAnimationController()->disableAnimation("Run");
        //localPlayer->getAnimationController()->enableAnimation("Idle");
    }


    lastOrientation = cameraController->getVerticalOrientation();


    unsigned char animationState=0;
    if (move)
        animationState += PlayerAnimation::MOVING;
    if (backwards)
        animationState += PlayerAnimation::BACKWARDS;
    if (localPlayer->isFiring())
        animationState += PlayerAnimation::FIRING;
    gmc->sendTransformUpdate(localPlayer->getSceneNode()->getPosition(), localPlayer->getSceneNode()->getOrientation(), animationState);

}


void GameCore::handlePlayerActions()
{
    //weapon control
    if(localPlayer->isFiring())
    {
        Weapon* weapon = localPlayer->getCurrentWeapon();
        if (weapon != NULL)
        {
            if (weapon->canFire())
            {
                weapon->fire();
                //localPlayer->getAnimationController()->enableAnimation("Fire");
                //localPlayer->getAnimationController()->playTillEnd("Fire",fireAnimationSpeed,true);
                Quaternion q;
                Quaternion q2;
                float x = cameraController->getHorizontalOrientation().getPitch().valueRadians();
                float y = localPlayer->getSceneNode()->getOrientation().getYaw().valueRadians();
                q.FromAngleAxis((Radian) y, Ogre::Vector3::UNIT_Y);
                q2.FromAngleAxis((Radian) x, Ogre::Vector3::UNIT_X);
                Quaternion orientation = q*q2;
                Ogre::Vector3 position = localPlayer->getSceneNode()->getPosition();
                position += Ogre::Vector3(0, 15, 0);
                Bullet* bullet = weapon->createBullet(sceneManager, dynamics, orientation);
                bulletVector.push_back(bullet);
                gmc->sendBulletInfo(bullet->getBulletNode()->getPosition(), x, y,localPlayer->getCurrentWeapon()->getWeaponSlot());

            }
        }
    }
    
}

void GameCore::handleCollisions()
{    
    btCollisionObject* object1;
    btCollisionObject* object2;
    btCollisionObject* bulletObject;
    btCollisionObject* hitObject;
    btCollisionObject* itemObject;
    CollisionObjectInfo* info1;
    CollisionObjectInfo* info2;
    btPersistentManifold* man;
    Bullet* bullet;
    bool _pass=false;
    btDispatcher* dispatcher = dynamics->getBulletCollisionWorld()->getDispatcher();


    //handle player-ground collisions
    if (localPlayer != NULL)
        localPlayer->setOnGround(false);

    Ogre::Vector3 playerPos = localPlayer->getBottomPoint();
    Ogre::Vector3 endPos = playerPos;
    Ogre::Vector3 hitPos;
    endPos.y -= 1;
    btCollisionObject _hitObject;
    if (PhysicsHandler::performRaycastTest(playerPos, endPos, dynamics))
        localPlayer->setOnGround(true);


    for (int i = 0; i < dispatcher->getNumManifolds(); i++)
    {
        man = dynamics->getBulletCollisionWorld()->getDispatcher()->getManifoldByIndexInternal(i);
        if(man->getNumContacts()>0)
        {
            object1 = (btCollisionObject*) man->getBody0();
            object2 = (btCollisionObject*) man->getBody1();
            info1 = (CollisionObjectInfo*) object1->getUserPointer();
            info2 = (CollisionObjectInfo*) object2->getUserPointer();

            /*
            //check ground collision for local player
            if (info1 != NULL && info2 != NULL)
                if (!localPlayer->isOnGround() && (info1->getObjectType() == CollisionObjectTypes::LOCAL_PLAYER || info2->getObjectType() == CollisionObjectTypes::LOCAL_PLAYER))
                    if (info1->getObjectType() == CollisionObjectTypes::GROUND || info2->getObjectType() == CollisionObjectTypes::GROUND)
                        localPlayer->setOnGround(true);
            */
            
            //handle bullet collisions
            _pass=false;
            for (int x = 0; x < bulletVector.size(); x++)
            {
                bullet = bulletVector.at(x);
                bulletObject = bullet->getRigidBody()->getBulletObject();
                if(bulletObject == object1 || bulletObject == object2)
                {
                    _pass=true;
                    hitObject = (bulletObject == object1 ? object2 : object1);
                    CollisionObjectInfo* info = (CollisionObjectInfo*) hitObject->getUserPointer();
                    if(info!=NULL)
                    {
                        int objectType=info->getObjectType();
                        if(objectType==CollisionObjectTypes::LOCAL_PLAYER && bullet->getOwnerID()==localPlayer->getId())
                            continue;
                        else
                        {
                            bullet->hit();
                            switch(objectType)
                            {
                                case CollisionObjectTypes::LOCAL_PLAYER:
                                {
                                    //taken hit
                                    Player* enemyPlayer = playerMap[bullet->getOwnerID()];
									if(!localPlayer->isActive(ItemType::IMMORTALITY) && !localPlayer->isDead() && !enemyPlayer->isFriendly() || (teamDM && !enemyPlayer->isFriendly() && !localPlayer->isDead()))
                                    {
										std::cout<<"player name: "<<enemyPlayer->getName()<<std::endl;
										std::cout<<"team: "<<(int)enemyPlayer->getTeam()<<std::endl;
										std::cout<<"enemy: "<<enemyPlayer->isFriendly()<<std::endl;
                                        if(enemyPlayer->isActive(ItemType::ONE_SHOT))
                                        {
											ingameGUI->getShot();
                                            localPlayer->setHealth(0);
                                            Logger::getSingleton()->addLine("GameCore: local player killed by one shot");
                                        }
                                        else
                                        {
                                            //get weapon type from the bullet and decrease health accordingly
                                            if(bullet->getWeaponType()==ItemType::LASER_PISTOL)
                                                localPlayer->decreaseHealth(20);
                                            else if(bullet->getWeaponType()==ItemType::LASER_MACHINE_GUN)
                                                localPlayer->decreaseHealth(5);
											ingameGUI->getShot();
                                        }
                                        
                                        Logger::getSingleton()->addLine("GameCore: player hit by "+playerMap[bullet->getOwnerID()]->getName());
                                        if(localPlayer->isDead())
                                        {
                                            Ogre::Vector3 pos = localPlayer->getSceneNode()->getPosition();
                                            gmc->notifyDeath(bullet->getOwnerID(), localPlayer->getCurrentWeapon()->getWeaponType(), pos.x, pos.y, pos.z);
                                            Logger::getSingleton()->addLine("GameCore: player has been killed");
                                        }
                                    }
                                    break;
                                }
                                default:
                                {
                                    //create hit decal
                                    if(objectType==CollisionObjectTypes::WALL)
                                        createDecal(OgreBulletCollisions::BtOgreConverter::to(man->getContactPoint(0).getPositionWorldOnA()),OgreBulletCollisions::BtOgreConverter::to(man->getContactPoint(0).m_normalWorldOnB));
                                }
                            }
                        }
                    }
                    break;
                }
            }
            if(_pass)
                continue;
            _pass=false;

            //handle item collisions
            GameItem* item;
            std::map<int,GameItem*>::iterator iter=itemMap.begin();
            while(iter!=itemMap.end())
            {
                item=iter->second;
                itemObject=item->getRigidBody()->getBulletObject();
                if(item->isAlive() && (itemObject==object1 || itemObject==object2))
                {
                    hitObject = (itemObject == object1 ? object2 : object1);
                    CollisionObjectInfo* info=(CollisionObjectInfo*)hitObject->getUserPointer();
                    if(info->getObjectType()==CollisionObjectTypes::LOCAL_PLAYER)
                    {
                        //player takes the item...
                        if(localPlayer->canTakeItem(item->getItemType()))
                        {
                            gmc->requestItemTake(item->getID());
                            //Logger::getSingleton()->addLine("GameCore: player can take the item.");
                            //item->hide();
                        }
                        else
                            ;
                            //Logger::getSingleton()->addLine("GameCore: player cannot take the item");
                    }
                    _pass=true;
					break;
                }
                iter++;
            }

			if(_pass)
				continue;
			_pass=false;
            
            //handle teleporter collisions
            Teleporter* teleporter;
            btCollisionObject* telObject;
            std::map<int,Teleporter*>::iterator telIter=teleporterMap.begin();
            while(telIter!=teleporterMap.end())
            {
                teleporter=telIter->second;
                telObject=teleporter->getTeleportPointRigid()->getBulletObject();
                if(teleporter->isActivated() && (telObject==object1 || telObject==object2))
                {
                    _pass=true;
                    hitObject=(telObject==object1?object2:object1);
                    CollisionObjectInfo* info=(CollisionObjectInfo*)hitObject->getUserPointer();
                    if(info->getObjectType()==CollisionObjectTypes::LOCAL_PLAYER)
                    {
                        gmc->requestTeleporterUse(teleporter->getID());
                    }
					break;
                }
                telIter++;
            }
        }
        
    }
}

void GameCore::scheduleTransformUpdate(PositionUpdateMessage_t* message)
{
	scheduler.scheduleOperation(new Operation1<GameCore,PositionUpdateMessage_t*>(this,&GameCore::_scheduleTransformUpdate,message));
	//_scheduleTransformUpdate(message);
}

void GameCore::_scheduleTransformUpdate(PositionUpdateMessage_t* message)
{
    if(gameLoaded && playerMap.count(message->header.clientID)>0)
    {
        Player* remotePlayer=playerMap[message->header.clientID];
        remotePlayer->tempPosition=Ogre::Vector3(message->x,message->y,message->z);
        remotePlayer->tempAngleY=message->angleY;
        remotePlayer->tempAnimation=(int)message->animation;
        remotePlayer->needsUpdate=true;
    }
	else
		Logger::getSingleton()->addLine("CANNOT APPLY TRANSFORM UPDATE FOR PLAYER ID: "+message->header.clientID,true);
    delete message;
}

void GameCore::addBullet(BulletInfo_t* info)
{
    bulletAddQueue.push_back(info);
}

void GameCore::windowResized(RenderWindow* rw){}
void GameCore::windowClosed(RenderWindow* rw){}

SceneNode* rifleNode;

bool GameCore::keyPressed(const KeyEvent &arg)
{
    static float pushForce = 30;
    static Player* play = NULL;
    static bool x = false;
    static std::map<std::string, int> texMap;
    static std::map<std::string, int>::iterator iter;
    if (!x && !menuMode)
    {

		rifleNode = localPlayer->getWeapon(ItemType::LASER_PISTOL)->getWeaponNode();

        texMap["Ogre::SHADOWTYPE_NONE"] = Ogre::SHADOWTYPE_NONE;
        texMap["Ogre::SHADOWTYPE_STENCIL_ADDITIVE"] = Ogre::SHADOWTYPE_STENCIL_ADDITIVE;
        texMap["Ogre::SHADOWTYPE_STENCIL_MODULATIVE"] = Ogre::SHADOWTYPE_STENCIL_MODULATIVE;
        texMap["Ogre::SHADOWTYPE_TEXTURE_ADDITIVE"] = Ogre::SHADOWTYPE_TEXTURE_ADDITIVE;
        texMap["Ogre::SHADOWTYPE_TEXTURE_MODULATIVE"] = Ogre::SHADOWTYPE_TEXTURE_MODULATIVE;
        iter = texMap.begin();


        //rifleNode->attachObject(ent);
        x = true;
    }

    //temp
	if (menuMode || ingameGUI->isChatOpen())
    {
        if (arg.key == KC_ESCAPE)
            switchToMenu(false);
	
		if(!ingameGUI->isChatOpen())
		{
			CEGUI::System::getSingleton().injectKeyDown(arg.key);
			CEGUI::System::getSingleton().injectChar(arg.text);
		}
		else
		{
			CEGUI::System::getSingleton().injectKeyDown(arg.key);
			CEGUI::System::getSingleton().injectChar(arg.text);
			if(arg.key==KC_RETURN)
			{
                            lobbyGameRoomSendMessageEvent(ingameGUI->getChatMessage());
                            ingameGUI->controlChat();
			}
			
			//ingameGUI->appendChat(arg.text);
		}
    }
    else if(gameLoaded) //game mode
    {
		if(arg.key==KC_RETURN)
			ingameGUI->controlChat();
        else if (arg.key == KC_INSERT)
            pushForce += 1;
        else if (arg.key == KC_DELETE)
            pushForce -= 1;
		/*
        else if (arg.key == KC_F9)
			teleporterMap[0]->deactivate();
        else if (arg.key == KC_F10)
			teleporterMap[0]->activate();*/
        else if (arg.key == KC_E && localPlayer != NULL)//push the object
        {/*
            CollisionObjectInfo* objectInfo;
            if (localPlayer->canPush(objectInfo))
            {
                std::cout << "player can push the object\n";
                OgreBulletDynamics::RigidBody* rigidBody = objectInfo->getRigidBody();
                Ogre::Vector3 pushDirection;
                pushDirection = localPlayer->getSceneNode()->getOrientation() * (Ogre::Vector3(0, 0, 1)*60000.);
                rigidBody->getBulletRigidBody()->applyCentralForce(OgreBulletCollisions::OgreBtConverter::to(pushDirection));
            }
          * */
        }
        else if (arg.key == KC_ESCAPE)
            switchToMenu(true);
        else if (arg.key == KC_NUMPAD5)
        {
            if (cameraController->isFreecam())
                cameraController->setTarget(localPlayer->getSceneNode());
            else
                cameraController->enableFreecam();
        } 
        else if (arg.key == KC_1)
        {
            //switch to slot 1
            if (localPlayer != NULL)
            {
                if (localPlayer->switchWeapon(WeaponSlot::Primary))
                {
                    Logger::getSingleton()->addLine("GameCore: weapon switched to primary");
                    gmc->notifyWeaponChange(localPlayer->getCurrentWeapon()->getWeaponType());
                    ingameGUI->changeWeaponIcon(WeaponSlot::Primary);
                }
                else
                    Logger::getSingleton()->addLine("GameCore: cannot change primary weapon");
            }
        } 
        else if (arg.key == KC_2)
        {
            //switch to slot 2
            if (localPlayer != NULL)
            {
                if (localPlayer->switchWeapon(WeaponSlot::Secondary))
                {
                    Logger::getSingleton()->addLine("GameCore: weapon switched to secondary");
                    gmc->notifyWeaponChange(localPlayer->getCurrentWeapon()->getWeaponType());
                    ingameGUI->changeWeaponIcon(WeaponSlot::Secondary);
                }
                else
                    Logger::getSingleton()->addLine("GameCore: cannot change secondary weapon");
            }
        } 
        else if (arg.key == KC_5)
        {
            //eagle eye
            if (localPlayer != NULL)
            {
				if (localPlayer->hasItem(ItemType::EAGLE_EYE) && !localPlayer->isActive(ItemType::EAGLE_EYE))
                {
                    setEagleEyeMode(true);
					ingameGUI->showMessage("EagleEye mode activated");
					ingameGUI->hideItem(ItemType::EAGLE_EYE);
					localPlayer->activateSpecialItem(ItemType::EAGLE_EYE);
                    gmc->notifySpecialItemUse(ItemType::EAGLE_EYE);
                }
            }
        } 
        else if (arg.key == KC_6)
        {
            if (localPlayer != NULL && localPlayer->hasItem(ItemType::IMMORTALITY) && !localPlayer->isActive(ItemType::IMMORTALITY))
            {
				ingameGUI->showMessage("Immortality mode activated");
				ingameGUI->hideItem(ItemType::IMMORTALITY);
				localPlayer->activateSpecialItem(ItemType::IMMORTALITY);
                gmc->notifySpecialItemUse(ItemType::IMMORTALITY);
				Logger::getSingleton()->addLine("GameCore: local player has activated immortality");
            }
        } 
        else if (arg.key == KC_7)
        {
            if (localPlayer != NULL && localPlayer->hasItem(ItemType::ONE_SHOT) && !localPlayer->isActive(ItemType::ONE_SHOT))
            {
				ingameGUI->showMessage("OneShot mode activated");
				ingameGUI->hideItem(ItemType::ONE_SHOT);
				localPlayer->activateSpecialItem(ItemType::ONE_SHOT);
                gmc->notifySpecialItemUse(ItemType::ONE_SHOT);
				Logger::getSingleton()->addLine("GameCore: local player has activated one_shot");
            }

        } 
        else if (arg.key == KC_Q)
        {
            if (localPlayer != NULL)
            {
                if (localPlayer->switchToPreviousWeapon())
                {
                    Logger::getSingleton()->addLine("GameCore: switched to the previous");
                    gmc->notifyWeaponChange(localPlayer->getCurrentWeapon()->getWeaponType());
                }
                else
                    Logger::getSingleton()->addLine("GameCore: cannot change primary weapon");
            }
        } 
        else if (arg.key == KC_R)
        {
            if (localPlayer != NULL)
                localPlayer->getCurrentWeapon()->reload();
        }
        else if (arg.key == KC_F1)
            dynamics->setShowDebugShapes(!dynamics->getShowDebugShapes());
        else if (arg.key == KC_F4)
        {
            static bool st = false;
            setEagleEyeMode(st);
            st = !st;
        }
        else if (arg.key == KC_I)
			std::cout<<rifleNode->getPosition()<<std::endl;
        else if (arg.key == KC_H)
            rifleNode->translate(-.1, 0, 0);
        else if (arg.key == KC_K)
            rifleNode->translate(.1, 0, 0);
        else if (arg.key == KC_U)
            rifleNode->translate(0, 0, -.1);
        else if (arg.key == KC_J)
            rifleNode->translate(0, 0, .1);
        else if (arg.key == KC_P)
            rifleNode->translate(0, .1, 0);
        else if (arg.key == KC_L)
            rifleNode->translate(0, -.1, 0);
        else if (arg.key == KC_X)
		{
            rifleNode->setScale(rifleNode->getScale() + Ogre::Vector3(.01, .01, .01));
			std::cout<<rifleNode->getScale()<<std::endl;
		}
        else if (arg.key == KC_Z)
		{
            rifleNode->setScale(rifleNode->getScale() + Ogre::Vector3(-.01, -.01, -.01));
			std::cout<<rifleNode->getScale()<<std::endl;
		}
        else if (arg.key == KC_C)
            rifleNode->setScale(rifleNode->getScale() + Ogre::Vector3(-.1, 0, 0));
        else if (arg.key == KC_V)
            rifleNode->setScale(rifleNode->getScale() + Ogre::Vector3(.1, 0, 0));
        else if (arg.key == KC_O)
            rifleNode->rotate(Ogre::Vector3::UNIT_Y, Degree(1));
        else if (arg.key == KC_P)
            rifleNode->rotate(Ogre::Vector3::UNIT_Y, Degree(-1));
        else if (arg.key == KC_F10)//Vector3(3.9, 1.7, 3.2
			gmc->changeColor(0);
        else if (arg.key == KC_F11)
			gmc->changeColor(1);
        else if (arg.key == KC_F12)
        {
            if (iter == texMap.end())
                iter = texMap.begin();
            std::cout << "ShadowType: " << iter->first << std::endl;
            sceneManager->setShadowTechnique((Ogre::ShadowTechnique)iter->second);
            iter++;
        }
//        std::cout << "pos: " << rifleNode->getPosition() << std::endl;
//        std::cout << "rot: " << rifleNode->getOrientation().getYaw().valueDegrees() << std::endl;
//        std::cout << "scale: " << rifleNode->getScale() << std::endl;
			


    }

return true;
}


void GameCore::switchToMenu(bool value)
{
    if(value)
    {
        menuController->showMain();
        CEGUI::MouseCursor::getSingleton().show();
        CEGUI::MouseCursor::getSingletonPtr()->setImage("TaharezLook","MouseArrow");
        menuMode = true;
        //renderWindow->removeAllViewports();
        //viewPort = renderWindow->addViewport(menuCamera);
    }
    else if(gameLoaded)
    {
	ingameGUI->activate();
        menuController->clearMenu();
        CEGUI::MouseCursor::getSingleton().show();
        CEGUI::MouseCursor::getSingletonPtr()->setImage("TaharezLook","MouseTarget");

        float height=CEGUI::MouseCursor::getSingletonPtr()->getImage()->getHeight()/2.;
        float width=CEGUI::MouseCursor::getSingletonPtr()->getImage()->getWidth()/2.;
        CEGUI::MouseCursor::getSingletonPtr()->setPosition(CEGUI::Point((renderWindow->getWidth()/2.)-width,(renderWindow->getHeight()/2.)+height));
        menuMode = false;
        //renderWindow->removeAllViewports();
        //viewPort = renderWindow->addViewport(mainCamera);
    }
}

bool GameCore::keyReleased(const KeyEvent &arg)
{
    CEGUI::System::getSingleton().injectKeyUp(arg.key);
	if(arg.key == KC_TAB)
		ingameGUI->hide(InGameGUIHandler::SCORE_BOARD);

    return true;
}

bool GameCore::mouseMoved(const MouseEvent &arg)
{
    if(menuMode)
        CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
    else
    {
        if (cameraController->isFreecam())
            cameraController->rotateCamera(arg.state.X.rel*(sensivity/5.), arg.state.Y.rel*(sensivity/5.));
        else
        {
            cameraController->slerpCamera(arg.state.X.rel*(sensivity/5.), arg.state.Y.rel*(sensivity/5.));

        }
        cameraController->setZoom(arg.state.Z.rel / 30.);
        return true;
    }

}

bool GameCore::mousePressed( const MouseEvent &arg, MouseButtonID id)
{
    if(menuMode)
    {
        if(id==0)
            CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
    }
    else
    {
        if (id == 0)
        {
            localPlayer->setFiring(true);

        }
        if (id == 2)
        {
            cameraController->resetZoom();
            cameraController->moveCamera(-reader->getFieldValueAsDouble("CameraSettings","camera-shift"),0);
        }
    }

    return true;
}

bool GameCore::mouseReleased( const MouseEvent &arg, MouseButtonID id)
{
    if(menuMode)
    {
        if(id==0)
            CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
    }
    else
    {
        if(id==0)
            localPlayer->setFiring(false);
    }

    return true;
}

void GameCore::setEagleEyeMode(bool status)
{
    std::map<int, GameItem*>::iterator iter = itemMap.begin();
    while (iter != itemMap.end())
    {
        iter->second->setEagleEyeMode(status);
        iter++;
    }
    std::map<unsigned int, Player*>::iterator pIter = playerMap.begin();
    while (pIter != playerMap.end())
    {
        pIter->second->setEagleEyeMode(status);
        pIter++;
    }
}

void GameCore::disposePlayer(unsigned int id)
{
    if(playerMap.count(id)>0)
    {
        Player* player=playerMap[id];
        playerMap.erase(id);
        player->getSceneNode()->setVisible(false);
        //delete player;
    }
}

void GameCore::setGameLoaded(bool value)
{
	gameLoaded=value;
}

void GameCore::localDiscoveryFinalized(std::map<std::string,std::string> serverList)
{
    int i=0;
    for(std::map<std::string,std::string>::iterator iter=serverList.begin();iter!=serverList.end();iter++)
        scheduler.scheduleOperation(new Operation3<LobbyMenu,int,std::string,std::string>(lobbyMenu,&LobbyMenu::addServerToList,i++,iter->second,iter->first));
}


void GameCore::connectionAccepted()
{
    Logger::getSingleton()->addLine("GameCore lobby connection request accepted");
    Logger::getSingleton()->addLine("GameCore client id is "+Converter::intToString(gmc->getLocalClient()->getID()));
    Operation<LobbyMenu>* op=new Operation<LobbyMenu>(lobbyMenu,&LobbyMenu::showLobby);
    Operation<LobbyMenu>* op2=new Operation<LobbyMenu>(lobbyMenu,&LobbyMenu::hideConnectionMenu);
    scheduler.scheduleOperation(op);
    scheduler.scheduleOperation(op2);
    gmc->requestGameRooms();
}

void GameCore::connectionFailed()
{
    scheduler.scheduleOperation(new Operation1<MenuController, std::string > (menuController, &MenuController::showMessage, "Connection failed"));
}

void GameCore::gameRoomInfoReceived(GameRoom* gameRoom)
{
    Logger::getSingleton()->addLine("GameCore game room received");
    Logger::getSingleton()->addLine("---ID: "+Converter::intToString(gameRoom->getRoomID()));
    Logger::getSingleton()->addLine("---Name: "+gameRoom->getRoomName());
    int id=gameRoom->getRoomID();
    std::string name=gameRoom->getRoomName();
    std::string capacity=Converter::intToString(gameRoom->getPlayerCount())+"/"+Converter::intToString(gameRoom->getCapacity());
    std::string mapName=gameRoom->getMapName();
    std::string gameType=(gameRoom->getGameType()==0?"Death-Match":"Team Match");
	std::cout<<"scheduling..\n";
    roomID=gameRoom->getRoomID();
    Operation5<LobbyMenu,int,std::string,std::string,std::string,std::string>* op=new Operation5<LobbyMenu,int,std::string,std::string,std::string,std::string>(lobbyMenu,&LobbyMenu::addGameToList,id,name,mapName,gameType,capacity);
    scheduler.scheduleOperation(op);
}

void GameCore::gameRoomStateChanged(GameRoom* room)
{
    std::string capacity=Converter::intToString(room->getPlayerCount())+"/"+Converter::intToString(room->getCapacity());
    std::string gameType=(room->getGameType()==0?"Death-Match":"Team Match");
    scheduler.scheduleOperation(new Operation1<LobbyMenu,int>(lobbyMenu,&LobbyMenu::removeGameFromList,room->getRoomID()));
    scheduler.scheduleOperation(new Operation5<LobbyMenu,int,std::string,std::string,std::string,std::string>(lobbyMenu,&LobbyMenu::addGameToList,room->getRoomID(),room->getRoomName(),room->getMapName(),gameType,capacity));
}

void GameCore::gameRoomCreated()
{
    GameRoom* gameRoom=gmc->getCurrentRoom();
    Logger::getSingleton()->addLine("GameCore game room created");
    Logger::getSingleton()->addLine("GameCore room id: "+Converter::intToString(gameRoom->getRoomID()));
    Logger::getSingleton()->addLine("GameCore room TCP: "+Converter::intToString(gameRoom->getTcpPort()));
    Logger::getSingleton()->addLine("GameCore room UDP: "+Converter::intToString(gameRoom->getUdpServerPort()));
    Logger::getSingleton()->addLine("GameCore game Type: "+Converter::intToString(gameRoom->getGameType()),true);
	if(gameRoom->getGameType()==0)
		scheduler.scheduleOperation(new Operation<GameRoomMenu>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::showMenu));
	else
		scheduler.scheduleOperation(new Operation<TeamGameRoomMenu>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::showMenu));

}

void GameCore::gameRoomConnectionEstablished()
{
	GameRoom* gameRoom=gmc->getCurrentRoom();
    Logger::getSingleton()->addLine("GameCore connection established with the game room "+gmc->getCurrentRoom()->getRoomName());
    Logger::getSingleton()->addLine("GameCore game Type: "+Converter::intToString(gameRoom->getGameType()),true);
	if(gameRoom->getGameType()==GameType::DM)
	{
		scheduler.scheduleOperation(new Operation<GameRoomMenu>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::showMenu));
		scheduler.scheduleOperation(new Operation4<GameRoomMenu,std::string,std::string,std::string,int>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setGameInfo,gameRoom->getRoomName(),gameRoom->getMapName(),"Death-Match",gameRoom->getCapacity()));
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,std::string>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setTitle,gmc->getCurrentRoom()->getRoomName()+" Room"));
		scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::setGameType,InGameGUIHandler::DEATH_MATCH));
		teamDM=false;
	}
	else
	{
		scheduler.scheduleOperation(new Operation<TeamGameRoomMenu>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::showMenu));
		scheduler.scheduleOperation(new Operation4<TeamGameRoomMenu,std::string,std::string,std::string,int>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setGameInfo,gameRoom->getRoomName(),gameRoom->getMapName(),"Team Match",gameRoom->getCapacity()));
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,std::string>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setTitle,gmc->getCurrentRoom()->getRoomName()+" Team Deathmatch Room"));
		scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::setGameType,InGameGUIHandler::TEAM_MATCH));
		teamDM=true;
	}
    
    gmc->requestGameRoomClients();
    gmc->requestGameRoomOwner();
    gameRoomClientJoined(gmc->getLocalClient());
}

void GameCore::gameRoomConnectionTerminated()
{
	Logger::getSingleton()->addLine("GameCore: room connection terminated",true);
	//hide menu
}

void GameCore::gameRoomConnectionDenied()
{
	scheduler.scheduleOperation(new Operation1<MenuController,std::string>(menuController,&MenuController::showMessage,"Cannot connect to the room; Connection denied."));
}


void GameCore::gameRoomClientJoined(GameClient* client)
{
	Logger::getSingleton()->addLine("GameCore: new client joined with id :"+Converter::intToString(client->getID()));
    std::string message="#Player "+client->getName()+" has joined the room";
	Logger::getSingleton()->addLine("GameCore: client joins: "+Converter::intToString(gmc->getCurrentRoom()->getGameType()),true);
	if(client==gmc->getLocalClient() && gmc->getCurrentRoom()->getOwner()==client->getID())
		client->setReady(true);
	if(gmc->getCurrentRoom()->getGameType()==GameType::DM)//death match
	{
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,std::string>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addMessageToChat,message));
		scheduler.scheduleOperation(new Operation4<GameRoomMenu,int,int,std::string,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addPlayerToList,-1,client->getID(),client->getName(),client->isReady()));
		if(client==gmc->getLocalClient() && gmc->getCurrentRoom()->getOwner()==client->getID())
                {
			scheduler.scheduleOperation(new Operation1<GameRoomMenu,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setHost,true));
                        scheduler.scheduleOperation(new Operation2<GameRoomMenu,int,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setReady,client->getID(),true));
                }
	}
	else
	{
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,std::string>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addMessageToChat,message));
		//bool ready=(client==gmc->getLocalClient() && client->getID()==gmc->getCurrentRoom()->getOwner()?true:client->isReady());
		std::cout<<"!*!*!*!* PLAYER TEAM: "<<(int)client->getTeam()<<std::endl;
		scheduler.scheduleOperation(new Operation4<TeamGameRoomMenu,int,int,std::string,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addPlayerToList,client->getTeam(),client->getID(),client->getName(),client->isReady()));
		if(client==gmc->getLocalClient() && gmc->getCurrentRoom()->getOwner()==client->getID())
                {
			scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setHost,true));
                        scheduler.scheduleOperation(new Operation2<TeamGameRoomMenu,int,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setReady,client->getID(),true));
                }
	}

	if(teamDM)
		scheduler.scheduleOperation(new Operation5<InGameGUIHandler,int,int,std::string,int,int>(ingameGUI,&InGameGUIHandler::addPlayerToList,client->getTeam(),client->getID(),client->getName(),client->getKillCount(),client->getDeathCount()));
	else
		scheduler.scheduleOperation(new Operation5<InGameGUIHandler,int,int,std::string,int,int>(ingameGUI,&InGameGUIHandler::addPlayerToList,InGameGUIHandler::NO_TEAM,client->getID(),client->getName(),client->getKillCount(),client->getDeathCount()));

    if(gameLoaded)
    {
		scheduler.scheduleOperation(new Operation1<InGameGUIHandler,std::string>(ingameGUI,&InGameGUIHandler::addGameInfo,client->getName()+" has joined the game"));
        Operation1<GameCore,GameClient*>* op3=new Operation1<GameCore,GameClient*>(this,&GameCore::createPlayer,client);
        scheduler.scheduleOperation(op3);
    }
}

void GameCore::gameRoomClientLeft(GameClient* client)
{
    Logger::getSingleton()->addLine("GameCore client has left the game:"+Converter::intToString(client->getID()));
    std::string message="#Player "+client->getName()+" has left the room";

	if(!teamDM)
	{
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,std::string>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addMessageToChat,message));
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,int>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::removePlayerFromList,client->getID()));
	}
	else
	{
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,std::string>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addMessageToChat,message));
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,int>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::removePlayerFromList,client->getID()));
	}
	
	scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::removePlayerFromList,client->getID()));

    if(gameLoaded)
	{
        scheduler.scheduleOperation(new Operation1<GameCore,unsigned int>(this,&GameCore::destroyPlayer,client->getID()));
		scheduler.scheduleOperation(new Operation1<InGameGUIHandler,std::string>(ingameGUI,&InGameGUIHandler::addGameInfo,client->getName()+" has left the game"));
	}
}

void GameCore::gameRoomClientReadyStatusChanged(GameClient* client)
{
    std::string status=(client->isReady()?"ready":"not ready");
    Logger::getSingleton()->addLine("GameCore client's status has been changed to :"+status);
	if(!teamDM)
		scheduler.scheduleOperation(new Operation2<GameRoomMenu,int,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setReady,client->getID(),client->isReady()));
	else
		scheduler.scheduleOperation(new Operation2<TeamGameRoomMenu,int,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setReady,client->getID(),client->isReady()));
}


void GameCore::gameRoomClientMessageReceived(GameClient* client,std::string message,bool teamChat)
{
    std::string msg;
    if(client==gmc->getLocalClient())
    {
        Logger::getSingleton()->addLine("GameCore chat message has been sent");
        msg="You: ";
    }
    else
    {
        Logger::getSingleton()->addLine("GameCore chat message has been received from client "+client->getName());
        msg=client->getName()+": ";
    }
    msg+=message;
	if(!teamDM)
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,std::string>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addMessageToChat,msg));
	else
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,std::string>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addMessageToChat,msg));
	if(client==gmc->getLocalClient())
		scheduler.scheduleOperation(new Operation2<InGameGUIHandler,std::string,bool>(ingameGUI,&InGameGUIHandler::addChatMessage,client->getName()+": "+message,false));
	else
		scheduler.scheduleOperation(new Operation2<InGameGUIHandler,std::string,bool>(ingameGUI,&InGameGUIHandler::addChatMessage,client->getName()+": "+message,true));

}

void GameCore::gameRoomClientNickChanged(GameClient* client,std::string oldNick)
{
    std::string message="#Player "+oldNick+" has changed his nickname as "+client->getName();
    GameRoomMenu* gameRoomMenu=lobbyMenu->getGameRoomMenu();
	if(!teamDM)
	{
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,int>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::removePlayerFromList,client->getID()));
		scheduler.scheduleOperation(new Operation4<GameRoomMenu,int,int,std::string,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addPlayerToList,-1,client->getID(),client->getName(),client->isReady()));
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,std::string>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addMessageToChat,message));
	}
	else
	{
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,int>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::removePlayerFromList,client->getID()));
		scheduler.scheduleOperation(new Operation4<TeamGameRoomMenu,int,int,std::string,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addPlayerToList,-1,client->getID(),client->getName(),client->isReady()));
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,std::string>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addMessageToChat,message));
	}
	if(client->getID()==gmc->getLocalClient()->getID())
	{
		writer->setValue("MultiplayerSettings", "nickname", nickname);
		writer->saveFile();
	}
}

void GameCore::gameRoomRemoved(unsigned int id)
{
    Operation1<LobbyMenu,int>* op=new Operation1<LobbyMenu,int>(lobbyMenu,&LobbyMenu::removeGameFromList,id);
    scheduler.scheduleOperation(op);
}

void GameCore::gameStarting()
{
	Logger::getSingleton()->addLine("GameCore: game starting...");
	Operation1<GameCore,bool>* menuShowOp =new Operation1<GameCore,bool>(this,&GameCore::setLoadingMenu,true);
	Operation1<GameCore,bool>* menuHideOp =new Operation1<GameCore,bool>(this,&GameCore::setLoadingMenu,false);
    Operation<GameCore>* op =new Operation<GameCore>(this,&GameCore::initializePhysics);
    Operation<GameCore>* op2 =new Operation<GameCore>(this,&GameCore::initializeScene);
    Operation1<GameCore,std::string>* op3 =new Operation1<GameCore,std::string>(this,&GameCore::initializeMap,mapPath+gmc->getCurrentRoom()->getMapName());
    
    Operation<GameCore>* op4=new Operation<GameCore>(this,&GameCore::initializeItems);
    Operation<GameCore>* op5 =new Operation<GameCore>(this,&GameCore::initializePlayers);
    Operation1<GameCore,bool>* op6=new Operation1<GameCore,bool>(this,&GameCore::switchToMenu,false);
    Operation1<GameCore,bool>* op7=new Operation1<GameCore,bool>(this,&GameCore::switchToMenu,true);
    Operation1<GameCore,bool>* op8=new Operation1<GameCore,bool>(this,&GameCore::switchToMenu,false);
    Operation<GameCore>* op9 =new Operation<GameCore>(this,&GameCore::initializeTeleporters);
    Operation1<GameCore,bool>* op10 =new Operation1<GameCore,bool>(this,&GameCore::setGameLoaded,true);
    scheduler.scheduleOperation(menuShowOp);
    scheduler.scheduleOperation(op);
    scheduler.scheduleOperation(op2);
	scheduler.scheduleOperation(op3);
    scheduler.scheduleOperation(op4);
    scheduler.scheduleOperation(op5);
    scheduler.scheduleOperation(menuHideOp);
    scheduler.scheduleOperation(op9);
    scheduler.scheduleOperation(op10);
    scheduler.scheduleOperation(op6);
    scheduler.scheduleOperation(op7);
    scheduler.scheduleOperation(op8);
}

void GameCore::itemTakeEvent(int itemID)
{
	if(gameLoaded)
	{
		if(itemMap.count(itemID)>0)
		{
			Operation1<Player,ItemType::ItemTypes>* op=new Operation1<Player,ItemType::ItemTypes>(localPlayer,&Player::takeItem,itemMap[itemID]->getItemType());
			scheduler.scheduleOperation(op);
			int itemType=itemMap[itemID]->getItemType();
			if(itemType==ItemType::EAGLE_EYE || itemType==ItemType::IMMORTALITY || itemType==ItemType::ONE_SHOT)
				scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::showItem,itemType));
			Logger::getSingleton()->addLine("GameCore: taking item");
		}
		else
			Logger::getSingleton()->addLine("GameCore: Item for 'item take event' not found!",true);
	}
}

void GameCore::itemStatusChanged(int itemID,bool alive)
{
	if(gameLoaded)
	{
		if(itemMap.count(itemID)>0)
		{
			Operation<GameItem>* op;
			if(alive)
				op=new Operation<GameItem>(itemMap[itemID],&GameItem::show);
			else
				op=new Operation<GameItem>(itemMap[itemID],&GameItem::hide);
			scheduler.scheduleOperation(op);
		}
		else
			Logger::getSingleton()->addLine("GameCore: Item for 'item status changed' not found!",true);
	}
}

void GameCore::playerKilledEvent(int playerID, int killerID)
{
	Logger::getSingleton()->addLine("GameCore: player killed event occured");
	if(gameLoaded)
	{
		Player* player;
		Player* killer;
		if(localPlayer->getId()==playerID)
		{
			player=localPlayer;
			killer=playerMap[killerID];
		}
		else if(localPlayer->getId()==killerID)
		{
			player=playerMap[playerID];
			killer=localPlayer;
		}
		else
		{
			player=playerMap[playerID];
			killer=playerMap[killerID];
		}

		Logger::getSingleton()->addLine("GameCore: player "+player->getName()+" has been killed by "+killer->getName());
	    
		if(player!=localPlayer)
		{
			Operation<Player>* op=new Operation<Player>(player,&Player::die);
			scheduler.scheduleOperation(op);
		}
		scheduler.scheduleOperation(new Operation1<InGameGUIHandler,std::string>(ingameGUI,&InGameGUIHandler::addGameInfo,killer->getName()+" killed "+player->getName()));
	}
}

void GameCore::playerResurrectedEvent(int playerID)
{
	if(gameLoaded)
	{
		Player* player;
		if(localPlayer->getId()==playerID)
		{
			Logger::getSingleton()->addLine("GameCore: local player resurrected");
			Operation<CameraController>* op2 = new Operation<CameraController > (cameraController, &CameraController::resetOrientation);
			scheduler.scheduleOperation(op2);
			scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::showItem,ItemType::EAGLE_EYE));
			scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::showItem,ItemType::IMMORTALITY));
			scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::showItem,ItemType::ONE_SHOT));
			player=localPlayer;
			//make other settings
			//reset weaponary
		}
		else if(playerMap.count(playerID)>0)
			player=playerMap[playerID];
	    
		Operation<Player>* op = new Operation<Player>(player, &Player::respawn);
		scheduler.scheduleOperation(op);
	}
}

void GameCore::playerColorChanged(GameClient* client)
{
	if(gameLoaded)
	{
		Player* player;
		if(client->getID()==gmc->getLocalClient()->getID())
			player=localPlayer;
		else
			player=playerMap[client->getID()];
		scheduler.scheduleOperation(new Operation1<Player,unsigned char>(player,&Player::changePlayerColor,client->getColor()));
	}
}


void GameCore::itemExpiredEvent(int itemType)
{
	if(gameLoaded)
	{
		Logger::getSingleton()->addLine("GameCore: Special item expired");
		if(itemType==ItemType::EAGLE_EYE)
			scheduler.scheduleOperation(new Operation1<GameCore,bool>(this, &GameCore::setEagleEyeMode,false));
		std::string message=(itemType==ItemType::EAGLE_EYE?"EagleEye":(itemType==ItemType::IMMORTALITY?"Immortality":"OneShot"));
		message+=" mode deactivated";
		scheduler.scheduleOperation(new Operation1<InGameGUIHandler,std::string>(ingameGUI, &InGameGUIHandler::showMessage,message));
	    
		Operation1<Player,ItemType::ItemTypes>* op2 = new Operation1<Player,ItemType::ItemTypes>(localPlayer, &Player::deactivateSpecialItem,(ItemType::ItemTypes)itemType);
		scheduler.scheduleOperation(op2);
	}
}

void GameCore::playerWeaponSwitchEvent(int playerID, int weaponType)
{
	if(gameLoaded)
	{
		if(playerMap.count(playerID)>0)
		{
			Player* player=playerMap[playerID];
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(player,&Player::selectWeapon,(ItemType::ItemTypes)weaponType));
			Logger::getSingleton()->addLine("GameCore: player "+player->getName()+" has switched a weapon");
		}
		else
			Logger::getSingleton()->addLine("GameCore: player not found for weapon switch event!",true);
	}
}

void GameCore::nextSpawnPointEvent(double x, double y, double z)
{
    if(localPlayer!=NULL)
    {
        Ogre::Vector3 sp(x,y,z);
        Logger::getSingleton()->addLine("GameCore: new spawn point received");
        Operation1<Player,Ogre::Vector3>* op=new Operation1<Player,Ogre::Vector3>(localPlayer,&Player::setNextSpawnPoint,sp);
        scheduler.scheduleOperation(op);
    }
}

void GameCore::resetPositionEvent()
{
    if(localPlayer!=NULL)
    {
        Operation<Player>* op=new Operation<Player>(localPlayer,&Player::resetPosition);
        scheduler.scheduleOperation(op);
    }
}

void GameCore::itemCreateEvent(int itemID, int itemType, float posX, float posY, float posZ)
{
	//scheduler.scheduleOperation(new Operation6<GameCore,int,ItemType::ItemTypes,std::string,std::string,Ogre::Vector3,Ogre::Vector3>(this,&GameCore::createItem,itemID,(ItemType::ItemTypes)itemType,"LaserRifle.mesh","LaserRifleItem",Ogre::Vector3(2,2,2),Ogre::Vector3(posX,posY,posZ)));
}


void GameCore::playerSpecialItemUsedEvent(unsigned int playerID, int itemType)
{
    if(playerMap.count(playerID)>0)
    {
        if(itemType==ItemType::ONE_SHOT)
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(playerMap[playerID], &Player::activateSpecialItem,ItemType::ONE_SHOT));
		else if(itemType==ItemType::IMMORTALITY)
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(playerMap[playerID], &Player::activateSpecialItem,ItemType::IMMORTALITY));
		else if(itemType==ItemType::EAGLE_EYE)
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(playerMap[playerID], &Player::activateSpecialItem,ItemType::EAGLE_EYE));
    }
    else
        Logger::getSingleton()->addLine("GameCore: player id not found!",true);
}

void GameCore::playerSpecialItemExpiredEvent(unsigned int playerID, int itemType)
{
    if(playerMap.count(playerID)>0)
    {
        if(itemType==ItemType::ONE_SHOT)
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(playerMap[playerID], &Player::deactivateSpecialItem,ItemType::ONE_SHOT));
		else if(itemType==ItemType::IMMORTALITY)
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(playerMap[playerID], &Player::deactivateSpecialItem,ItemType::IMMORTALITY));
		else if(itemType==ItemType::EAGLE_EYE)
			scheduler.scheduleOperation(new Operation1<Player,ItemType::ItemTypes>(playerMap[playerID], &Player::deactivateSpecialItem,ItemType::EAGLE_EYE));
    }
    else
        Logger::getSingleton()->addLine("GameCore: player id not found!",true);
}


void GameCore::teleporterDeactivated(int teleporterID)
{
    if(teleporterMap.count(teleporterID)>0)
        scheduler.scheduleOperation(new Operation<Teleporter>(teleporterMap[teleporterID],&Teleporter::deactivate));
    else
        Logger::getSingleton()->addLine("GameCore: teleporter not found!",true);
}

void GameCore::teleporterActivated(int teleporterID)
{
    if(teleporterMap.count(teleporterID)>0)
        scheduler.scheduleOperation(new Operation<Teleporter>(teleporterMap[teleporterID],&Teleporter::activate));
    else
        Logger::getSingleton()->addLine("GameCore: teleporter not found!",true);
}

void GameCore::teleporterUsageAccepted(int teleporterID)
{
    Logger::getSingleton()->addLine("GameCore: teleporter usage accepted for tid "+Converter::intToString(teleporterID));
    if(teleporterMap.count(teleporterID)>0)
        scheduler.scheduleOperation(new Operation1<Player,Ogre::Vector3>(localPlayer,&Player::teleport,teleporterMap[teleporterID]->getOutPosition()));
    else
        Logger::getSingleton()->addLine("GameCore: teleporter not found!",true);
}

void GameCore::playerScoreUpdated(GameClient* client)
{
    Logger::getSingleton()->addLine("GameCore: player: "+client->getName());
	Logger::getSingleton()->addLine("GameCore: ----deaths: '"+Converter::intToString((int)client->getDeathCount())+"'");
    Logger::getSingleton()->addLine("GameCore: ----kills: '"+Converter::intToString((int)client->getKillCount())+"'");
    
	scheduler.scheduleOperation(new Operation4<InGameGUIHandler,int,std::string,int,int>(ingameGUI,&InGameGUIHandler::updatePlayerScores,client->getID(),client->getName(),client->getKillCount(),client->getDeathCount()));
}

void GameCore::playerTeamChanged(GameClient* client)
{
	Logger::getSingleton()->addLine("GameCore: player "+client->getName()+" has changed his/her team");
	scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,int>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::removePlayerFromList,client->getID()));
	scheduler.scheduleOperation(new Operation4<TeamGameRoomMenu,int,int,std::string,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addPlayerToList,client->getTeam(),client->getID(),client->getName(),client->isReady()));
	scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::removePlayerFromList,client->getID()));
	scheduler.scheduleOperation(new Operation5<InGameGUIHandler,int,int,std::string,int,int>(ingameGUI,&InGameGUIHandler::addPlayerToList,client->getTeam(),client->getID(),client->getName(),client->getKillCount(),client->getDeathCount()));
}

void GameCore::gameRoomClientKicked(GameClient* client)
{
	std::string message="#player "+client->getName()+" has been kicked";
	if(!teamDM)
	{
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,std::string>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::addMessageToChat,message));
		scheduler.scheduleOperation(new Operation1<GameRoomMenu,int>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::removePlayerFromList,client->getID()));	
	}
	else
	{
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,std::string>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::addMessageToChat,message));
		scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,int>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::removePlayerFromList,client->getID()));
	}
}

void GameCore::gameRoomLocalClientKicked()
{
	Logger::getSingleton()->addLine("GameCore: local client has been kicked");
	if(!teamDM)
		scheduler.scheduleOperation(new Operation<GameRoomMenu>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::closeMenu));
	else
		scheduler.scheduleOperation(new Operation<TeamGameRoomMenu>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::closeMenu));

	Operation<LobbyMenu>* op=new Operation<LobbyMenu>(lobbyMenu,&LobbyMenu::showLobby);
	scheduler.scheduleOperation(new Operation<LobbyMenu>(lobbyMenu,&LobbyMenu::removeAllGames));
	scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::resetScoreBoard,0));
	scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::resetScoreBoard,1));
	scheduler.scheduleOperation(new Operation1<InGameGUIHandler,int>(ingameGUI,&InGameGUIHandler::resetScoreBoard,2));
	
    scheduler.scheduleOperation(op);
    gmc->requestGameRooms();
}

void GameCore::lobbyServerConnectEvent(std::string ipAddress)
{
	Logger::getSingleton()->addLine("GameCore: connecting to the server");
    gmc->setServerIP(ipAddress);
    gmc->connectToServer();
}

void GameCore::lobbyEnableLocalServerDiscoveryEvent()
{
    gmc->startLocalServerDiscoverer();
}
void GameCore::lobbyExitEvent()
{
    gmc->leaveServer();
}

void GameCore::lobbyGameRoomConnectEvent(int gameId)
{
    gmc->joinGameRoom(gameId);
}
void GameCore::lobbyGameRoomCreateEvent(std::string gameName, std::string mapName,unsigned char gameType, int gameCapacity)
{
    gmc->createGameRoom(gameName,gameType,mapName,(unsigned char)gameCapacity);
}
void GameCore::lobbyGameRoomRefreshEvent()
{
    Operation<LobbyMenu>* op=new Operation<LobbyMenu>(lobbyMenu,&LobbyMenu::removeAllGames);
    scheduler.scheduleOperation(op);
    gmc->requestGameRooms();
}
void GameCore::lobbyGameRoomSendMessageEvent(std::string message)
{
	if(message.find("!nick ")!=std::string::npos && message.length()>8)
	{
		std::string nickname=message.substr(message.find("!nick ")+6);
		Logger::getSingleton()->addLine("GameCore: requesting to change nick as "+nickname);
		gmc->changeNickName(nickname);
		this->nickname=nickname;
	}
        else if(message.length()>0)
		gmc->sendChatMessage(message,false);
}
void GameCore::lobbyGameRoomReadyStatusChangeEvent(bool ready)
{
    if(ready)
        gmc->sendReady();
    else
        gmc->sendNotReady();
}

void GameCore::lobbyGameRoomStartEvent()
{
	if(gmc->getCurrentRoom()->getOwner()==gmc->getLocalClient()->getID())
	{
		root->renderOneFrame();
		gmc->requestGameStart();
	}
}

void GameCore::lobbyGameRoomKickPlayer(int playerID)
{
    if(gmc->getLocalClient()->getID()!=playerID)
	gmc->requestPlayerKick(playerID);
}
void GameCore::lobbyGameRoomExitEvent()
{
    gmc->leaveGameRoom();
	lobbyMenu->removeAllGames();
	gmc->requestGameRooms();
	ingameGUI->resetScoreBoard(0);
	ingameGUI->resetScoreBoard(1);
	ingameGUI->resetScoreBoard(2);

}

void GameCore::lobbyRefreshServerListEvent()
{
    gmc->startLocalServerDiscoverer();
}


void GameCore::gameRoomOwnerChanged(GameClient* client)
{
    if(gmc->getLocalClient()==client)
    {
		if(!teamDM)
			scheduler.scheduleOperation(new Operation1<GameRoomMenu,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setHost,true));
		
		else if(teamDM)
			scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setHost,true));
    }
	else
	{
		if(!teamDM)
			scheduler.scheduleOperation(new Operation1<GameRoomMenu,bool>(lobbyMenu->getGameRoomMenu(),&GameRoomMenu::setHost,false));
		
		else if(teamDM)
			scheduler.scheduleOperation(new Operation1<TeamGameRoomMenu,bool>(lobbyMenu->getTeamGameRoomMenu(),&TeamGameRoomMenu::setHost,false));
	}
}

void GameCore::graphicSettingsChangedEvent()
{
    if(gameLoaded)
    {
        if(graphicSettingsMenu->getSpotLight())
            mazeBuilder->enableSpotLights();
        else
            mazeBuilder->disableSpotLights();
    }

    if (graphicSettingsMenu->getShadow())
        sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    else
        sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

    if (graphicSettingsMenu->getLight())
        setEnableLighting(true);
    else
        setEnableLighting(false);

    writer->setValue("GraphicsSettings", "shadows", (graphicSettingsMenu->getShadow() ? "yes" : "no"));
    writer->setValue("GraphicsSettings","generate-lights",(graphicSettingsMenu->getLight()?"yes":"no"));
    writer->setValue("GraphicsSettings","spot-lights",(graphicSettingsMenu->getSpotLight()?"yes":"no"));
    writer->saveFile();
}

void GameCore::multiplayerSettingsChangedEvent()
{
    nickname = multiplayerSettingsMenu->getPlayerName();
    sensivity = (multiplayerSettingsMenu->getMouseSensivity() <= 0 ? 5 : multiplayerSettingsMenu->getMouseSensivity());
    multiplayerSettingsMenu->setMouseSensivity(sensivity);
    gmc->changeNickName(nickname);
	if(!gameLoaded)
		gmc->setNickname(nickname);
	color=multiplayerSettingsMenu->getColor();
	gmc->setColor(color);
	gmc->changeColor(color);
    writer->setValue("MultiplayerSettings", "nickname", nickname);
	writer->setValue("MultiplayerSettings", "player-color", (color==0?"blue":"red"));
    writer->setValue("MultiplayerSettings", "mouse-sensivity", Converter::intToString(sensivity));
    writer->saveFile();
}

void GameCore::networkSettingsChangedEvent()
{
    gmc->setServerTcp(Converter::stringToInt(networkSettingsMenu->getServerPort()));
    gmc->setClientTTFTP(Converter::stringToInt(networkSettingsMenu->getTTFTP()));
    
    writer->setValue("NetworkSettings", "tcp-port",networkSettingsMenu->getServerPort());
    writer->setValue("NetworkSettings", "ttftp-client-port",networkSettingsMenu->getTTFTP());
    writer->saveFile();
}

void GameCore::lobbyGameRoomTeamChangedEvent()
{
	Logger::getSingleton()->addLine("GameCore: sending team change request");
	gmc->sendTeamChangeRequest(gmc->getLocalClient()->getTeam()==Teams::BLUE?Teams::RED:Teams::BLUE);
}

void GameCore::setLoadingMenu(bool enabled)
{
	if(enabled)
		menuController->showLoading();
	else
		menuController->hideLoading();
	root->renderOneFrame();
}