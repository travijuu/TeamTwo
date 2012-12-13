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

#include <Shapes/OgreBulletCollisionsCompoundShape.h>

#include "Player.h"
#include "CameraController.h"
#include "PhysicsShapeHandler.h"
#include "LaserMachineGun.h"
#include "Util/Logger.h"
#include "MapUtil/MapStructure.h"
#include "NetworkUtil/MessageStructure.h"
#include "Util/Converter.h"

Player::Player(int id, std::string name,bool local,bool friendly,unsigned char team, SceneManager* sceneManager)
{
    this->id=id;
    this->name=name;
    this->team = team;
    this->friendly=friendly;
    this->sceneManager=sceneManager;
    this->needsUpdate = false;
    this->onGround = false;
    this->firing=false;
    this->local=local;
    this->health=100;
    this->dead=false;
    this->nextSpawnPoint=Ogre::Vector3(0,50,0);
    this->immortal=false;
    this->oneShot=false;
	this->eagleEye=false;
    this->weaponIndex=0;


    this->animationController=NULL;
    this->playerNode=NULL;
    this->rigidBody=NULL;
    initializeInventory();
}

Player::~Player()
{
    Logger::getSingleton()->addLine("Player: deleting "+getName());


    rigidBody->getBulletDynamicsWorld()->removeRigidBody(rigidBody->getBulletRigidBody());
    delete rigidBody;

    if(playerNode!=NULL)
    {
        playerNode->detachAllObjects();
        delete animationController;
        sceneManager->destroySceneNode(playerNode);
        std::map<ItemType::ItemTypes, Weapon*>::iterator iter=weaponMap.begin();
        Weapon* weapon;
        while(iter!=weaponMap.end())
        {
            weapon=iter->second;
            if(weapon->getWeaponNode()!=NULL)
            {
                weapon->getWeaponNode()->detachAllObjects();
                sceneManager->destroySceneNode(weapon->getWeaponNode());
            }
            if(weapon->getWeaponLight())
                sceneManager->destroyLight(weapon->getWeaponLight());
            delete weapon;
            iter++;
        }
        
        weaponMap.clear();
        itemInventoryMap.clear();
    }
    
    Logger::getSingleton()->addLine("Player: "+getName()+" deleted.");
}

void Player::initializeInventory()
{
	LaserPistol* pistol=new LaserPistol(this->id,team,local);
    LaserMachineGun* mac=new LaserMachineGun(this->id,team,local);
    if(!local)
    {
        pistol->disableAmmunitionCheck();
        mac->disableAmmunitionCheck();
    }
    weaponMap[ItemType::LASER_PISTOL]=pistol;
    weaponMap[ItemType::LASER_MACHINE_GUN]=mac;
    itemInventoryMap[ItemType::EAGLE_EYE]=true;
    itemInventoryMap[ItemType::IMMORTALITY]=true;
    itemInventoryMap[ItemType::ONE_SHOT]=true;
    previousWeapon=currentWeapon=mac;
}

void Player::resetInventory()
{
    std::map<ItemType::ItemTypes,Weapon*>::iterator iter=weaponMap.begin();
    while(iter!=weaponMap.end())
    {
        iter->second->zero();
        iter++;
    }

    Weapon* weapon=weaponMap[ItemType::LASER_PISTOL];
    weapon->addClip(3);
    weapon->setClipAmmo(weapon->getClipSize());
    weapon=weaponMap[ItemType::LASER_MACHINE_GUN];
    weapon->addClip(5);
    weapon->setClipAmmo(weapon->getClipSize());

    std::map<ItemType::ItemTypes,bool>::iterator iter2=itemInventoryMap.begin();
    while(iter2!=itemInventoryMap.end())
    {
        iter2->second=false;
        iter2++;
    }

    itemInventoryMap[ItemType::EAGLE_EYE]=true;
    itemInventoryMap[ItemType::IMMORTALITY]=true;
    itemInventoryMap[ItemType::ONE_SHOT]=true;
}


void Player::respawn()
{
    //animationController->disableAnimation("Death2");
    animationController->disableAnimation("Die");
    animationController->enableAnimation("Run");
    animationController->enableAnimation("Idle");
    if(rigidBody!=NULL)
    {
        btTransform tr=rigidBody->getBulletRigidBody()->getWorldTransform();
        tr.setOrigin(OgreBulletCollisions::OgreBtConverter::to(getNextSpawnPoint()));
        tr.setRotation(OgreBulletCollisions::OgreBtConverter::to(Quaternion::IDENTITY));
        rigidBody->getBulletRigidBody()->setWorldTransform(tr);
    }
    setHealth(100);
    resetInventory();
    selectWeapon(ItemType::LASER_MACHINE_GUN);
}

void Player::die()
{
	Logger::getSingleton()->addLine("Player: dead");
    //perform death animation
    animationController->disableAnimation("Run");
    animationController->disableAnimation("Idle");
    animationController->enableAnimation("Die");
    animationController->playTillEnd("Die",.01);
    resetInventory();
	getCurrentWeapon()->hide();
}

int Player::getHealth()
{
    return health;
}

std::string Player::getName()
{
    return name;
}

bool Player::isDead()
{
    return dead;
}

void Player::setNextSpawnPoint(Ogre::Vector3 nextSpawnPoint)
{
    this->nextSpawnPoint = nextSpawnPoint;
    if(!isDead())
        resetPosition();
}

Ogre::Vector3 Player::getNextSpawnPoint()
{
    return nextSpawnPoint;
}

void Player::decreaseHealth(int dec)
{
    health-=dec;
	if(health<=0)
	{
		dead=true;
		health=0;
		die();
	}
	else
		dead=false;
}

void Player::setHealth(int health)
{
    this->health=health;
    dead=health>0?false:true;
    if(dead)
        die();
}

void Player::changePlayerColor(unsigned char color)
{
	if(color==0)
	{
		playerEntity->getSubEntity(0)->setMaterialName("BlueTeamCamo");
		playerEntity->getSubEntity(1)->setMaterialName("BlueTeamCamoBar");
		playerEntity->getSubEntity(3)->setMaterialName("BlueTeamSeq");
	}
	else
	{
		playerEntity->getSubEntity(0)->setMaterialName("RedTeamCamo");
		playerEntity->getSubEntity(1)->setMaterialName("RedTeamCamoBar");
		playerEntity->getSubEntity(3)->setMaterialName("RedTeamSeq");
	}	
	weaponMap[ItemType::LASER_MACHINE_GUN]->changeColor(color);
	weaponMap[ItemType::LASER_PISTOL]->changeColor(color);
}

void Player::createVisual(std::string entityName, Quaternion orientation, Ogre::Vector3 scale,Ogre::Vector3 position)
{
    if(playerNode==NULL)
    {
        //playerEntity=sceneManager->createEntity("Player_"+name,entityName);
        playerEntity=sceneManager->createEntity(entityName);
		if(local)
		 playerEntity->setRenderQueueGroup(RENDER_QUEUE_7);
		else
		 playerEntity->setRenderQueueGroup(RENDER_QUEUE_6);
		/*
		if(getTeam()==Teams::BLUE)
		{
			playerEntity->getSubEntity(0)->setMaterialName("BlueTeamCamo");
			playerEntity->getSubEntity(1)->setMaterialName("BlueTeamCamoBar");
			playerEntity->getSubEntity(3)->setMaterialName("BlueTeamSeq");
		}
		else
		{
			playerEntity->getSubEntity(0)->setMaterialName("RedTeamCamo");
			playerEntity->getSubEntity(1)->setMaterialName("RedTeamCamoBar");
			playerEntity->getSubEntity(3)->setMaterialName("RedTeamSeq");
		}
		*/

        playerNode=sceneManager->getRootSceneNode()->createChildSceneNode("Player_"+name);
        playerNode->attachObject(playerEntity);
        playerNode->scale(scale);
        playerNode->setOrientation(orientation);
        playerNode->setPosition(nextSpawnPoint);
        animationController = new AnimationController(playerEntity);

        std::map<ItemType::ItemTypes,Weapon*>::iterator iter;
        for(iter=weaponMap.begin();iter!=weaponMap.end();iter++)
        {
            iter->second->createVisual(sceneManager,playerNode);
            iter->second->hide();
        }
        currentWeapon->show();
		changePlayerColor(getTeam());

        for (int i = 0; i < playerEntity->getNumSubEntities(); i++)
            mats.push_back(playerEntity->getSubEntity(i)->getMaterial());
    }
    else
        std::cout<<"ERROR! createVisual failed for Player! It already exists!"<<std::endl;

}

bool Player::canPush(CollisionObjectInfo* objectInfo)
{
    CollisionObjectInfo* tempInfo;
    Ogre::Vector3 playerPosition=playerNode->getPosition();
    playerPosition.z-=playerEntity->getBoundingBox().getSize().z/2.*playerNode->getScale().z;
    Ogre::Vector3 endPosition=(playerNode->getOrientation()*Ogre::Vector3(0,0,-20))+playerPosition;
    btCollisionObject collisionObject;
    Ogre::Vector3 hitPos;
    bool result=false;
    if(PhysicsHandler::performRaycastTest(playerPosition,endPosition,dynamics,&collisionObject,&hitPos))
    {
        if((tempInfo=(CollisionObjectInfo*)collisionObject.getUserPointer())!=NULL)
        {
            *objectInfo=*tempInfo;
            if(objectInfo->getSceneNode()!=NULL)
                std::cout<<objectInfo->getSceneNode()->getName()<<std::endl;
            if(objectInfo->getObjectType()==CollisionObjectTypes::DYNAMIC_OBJECT)
                result=true;
        }
    }

    return result;
}

Ogre::Vector3 Player::getBottomPoint()
{
    Ogre::Vector3 pos=playerNode->getPosition();
    if(playerEntity!=NULL)
        pos.y-=playerEntity->getBoundingBox().getSize().y/2.*playerNode->getScale().y;
    return pos;
}

void Player::enablePhysics(OgreBulletDynamics::DynamicsWorld* dynamics)
{
    this->dynamics=dynamics;
    Ogre::Vector3 scaleFactor(playerNode->getScale().x,playerNode->getScale().y,playerNode->getScale().z);
    Ogre::Vector3 size=playerEntity->getBoundingBox().getSize();

    float scaleVal=playerNode->getScale().x+playerNode->getScale().y+playerNode->getScale().z;

    //OgreBulletCollisions::BoxCollisionShape* box=PhysicsShapeHandler::createBoxShape(playerEntity,scaleFactor);
    OgreBulletCollisions::CapsuleCollisionShape* capsule=PhysicsHandler::createCapsuleShape(playerEntity,scaleFactor);
    //OgreBulletCollisions::CompoundCollisionShape* compound=PhysicsHandler::createCompoundShapeAndAddChild(capsule,Ogre::Vector3(0,9,0));
    rigidBody=new OgreBulletDynamics::RigidBody(name,dynamics);
	rigidBody->setShape(playerNode, capsule, 1,1, 1, Ogre::Vector3(0,0,0), Quaternion::IDENTITY);
    if(local)
    {
        rigidBody->getBulletRigidBody()->setAngularFactor(btVector3(0, 0, 0));
        rigidBody->getBulletRigidBody()->setLinearFactor(btVector3(1, 1, 1));
        rigidBody->getBulletRigidBody()->setGravity(btVector3(0, -150, 0));
        //rigidBody->getBulletRigidBody()->setCcdMotionThreshold(.0001);
        //rigidBody->getBulletRigidBody()->setCcdSweptSphereRadius(.0001);
        
    }
    else
    {
        rigidBody->setKinematicObject(true);
        rigidBody->getBulletRigidBody()->setCollisionFlags(rigidBody->getBulletRigidBody()->getCollisionFlags()|btCollisionObject::CO_GHOST_OBJECT);
    }
    
    rigidBody->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->getBulletObject()->setUserPointer(new CollisionObjectInfo(playerNode,rigidBody,local?CollisionObjectTypes::LOCAL_PLAYER:CollisionObjectTypes::REMOTE_PLAYER,this));
    physicsEnabled=true;
}

std::vector<Weapon*> Player::getWeaponsBySlot(WeaponSlot::WeaponSlots weaponSlot)
{
    std::vector<Weapon*> _weaponVector;
    if(weaponSlot!=WeaponSlot::None)
    {
        std::map<ItemType::ItemTypes,Weapon*>::iterator iter=weaponMap.begin();
        while(iter!=weaponMap.end())
        {
            if(iter->second->getWeaponSlot()==weaponSlot)
                _weaponVector.push_back(iter->second);
            iter++;
        }
    }
    return _weaponVector;
}

bool Player::canTakeItem(ItemType::ItemTypes itemType)
{
    bool result=false;
    switch(itemType)
    {
        case ItemType::LASER_MACHINE_GUN:
        {
            //Logger::getSingleton()->addLine("Player: item type machine gun");
            Weapon* weapon=weaponMap[ItemType::LASER_MACHINE_GUN];
            result=weapon->canTakeClips();
            break;
        }
        case ItemType::LASER_PISTOL:
        {
            //Logger::getSingleton()->addLine("Player: item type laser pistol");
            Weapon* weapon=weaponMap[ItemType::LASER_PISTOL];
            result=weapon->canTakeClips();
            break;
        }
        case ItemType::EXTRA_HEALTH:
        {
            if(getHealth()<100)
                result=true;
            break;
        }
        default:
        {
            if(itemInventoryMap.count(itemType)>0 && !itemInventoryMap[itemType])
                result=true;
        }
    }

    return result;
}

void Player::takeItem(ItemType::ItemTypes itemType)
{
    switch(itemType)
    {
        case ItemType::LASER_PISTOL:
        {
            //add 5 clips
            Logger::getSingleton()->addLine("Player: 5 mags taken for laser pistol");
            Weapon* weapon=weaponMap[ItemType::LASER_PISTOL];
            weapon->addClip(5);
            break;
        }
        case ItemType::LASER_MACHINE_GUN:
        {
            //add 3 clips
            Logger::getSingleton()->addLine("Player: 3 mags taken for laser machine gun");
            Weapon* weapon=weaponMap[ItemType::LASER_MACHINE_GUN];
            weapon->addClip(3);
            break;
        }
        case ItemType::EXTRA_HEALTH:
        {
            Logger::getSingleton()->addLine("Player: healed to "+Converter::intToString(getHealth()));
            health=(health+20>100?100:health+20);
            break;
        }
        default:
        {
            Logger::getSingleton()->addLine("Player: special item has been added to the player's inventory");
            itemInventoryMap[itemType]=true;
        }
    }
}

bool Player::switchWeapon(WeaponSlot::WeaponSlots weaponSlot)
{
    bool result=false;
    int index=0;
    std::vector<Weapon*> weapons=getWeaponsBySlot(weaponSlot);
    if(currentWeapon->getWeaponSlot()==weaponSlot)
        index=(weaponIndex+1)%weapons.size();
    int startIndex=index;
    Weapon* weapon;
    do
    {
        weapon=weapons.at(index);
        if(weapon->getClipAmmo()<=0 && weapon->getClipCount()<=0)
            index=(index+1)%weapons.size();
        else
        {
            result=true;
            break;
        }
    }while(startIndex!=index);

    if(weapon!=currentWeapon)
    {
        previousWeapon=currentWeapon;
        currentWeapon=weapon;
        weaponIndex=index;
        previousWeapon->hide();
        currentWeapon->show();
        //perform change animations
        //perform auto-reload if ammo is zero
    }    
    return result;
}

bool Player::switchToPreviousWeapon()
{
    if(previousWeapon!=currentWeapon && previousWeapon->getClipAmmo()<=0 && previousWeapon->getClipCount()<=0)
        return false;
    Weapon* temp=currentWeapon;
    currentWeapon=previousWeapon;
    previousWeapon=temp;
    previousWeapon->hide();
    currentWeapon->show();
    return true;
}

void Player::selectWeapon(ItemType::ItemTypes itemType)
{
    if(weaponMap.count(itemType)>0)
    {
        previousWeapon=currentWeapon;
        previousWeapon->hide();
        currentWeapon=weaponMap[itemType];
        currentWeapon->show();
    }
    else
        Logger::getSingleton()->addLine("Player: weapon not found for changing 'selectWeapon'!",true);
}

void Player::resetPosition()
{
    if(rigidBody!=NULL)
    {
        btTransform tr=rigidBody->getBulletRigidBody()->getWorldTransform();
        tr.setOrigin(OgreBulletCollisions::OgreBtConverter::to(nextSpawnPoint));
        rigidBody->getBulletRigidBody()->setWorldTransform(tr);
    }
}

void Player::setEagleEyeMode(bool on)
{
    if(playerNode!=NULL && !local)
    {
        if(on)
        {
            if(friendly)
                playerEntity->setMaterialName("eagleeye_friendly");
            else
                playerEntity->setMaterialName("eagleeye_enemy");
        }
        else
        {
			for (int i = 0; i < playerEntity->getNumSubEntities(); i++)
                playerEntity->getSubEntity(i)->setMaterial(mats.at(i));
        }
    }
}

bool Player::hasItem(ItemType::ItemTypes itemType)
{
    return (itemInventoryMap.count(itemType)>0?(itemInventoryMap[itemType]?true:false):false);
}

void Player::removeSpecialItem(ItemType::ItemTypes item)
{
    itemInventoryMap[item]=false;
}

void Player::activateSpecialItem(ItemType::ItemTypes item)
{
	if(itemInventoryMap.count(item)>0 && itemInventoryMap[item])
	{
		itemInventoryMap[item]=false;
		if(item==ItemType::EAGLE_EYE)
			eagleEye=true;
		else if(item==ItemType::ONE_SHOT)
			oneShot=true;
		else if(item==ItemType::IMMORTALITY)
			immortal=true;
	}
}

void Player::deactivateSpecialItem(ItemType::ItemTypes item)
{
	if(item==ItemType::EAGLE_EYE)
		eagleEye=false;
	else if(item==ItemType::ONE_SHOT)
		oneShot=false;
	else if(item==ItemType::IMMORTALITY)
		immortal=false;
}

bool Player::isActive(ItemType::ItemTypes item)
{
	bool result=false;
	if(itemInventoryMap.count(item)>0)
	{	
		if(item==ItemType::EAGLE_EYE)
			result=eagleEye;
		else if(item==ItemType::ONE_SHOT)
			result=oneShot;
		else if(item==ItemType::IMMORTALITY)
			result=immortal;
	}
	return result;
}

void Player::teleport(Ogre::Vector3 teleportPoint)
{
    btTransform& tr=rigidBody->getBulletRigidBody()->getWorldTransform();
    teleportPoint.y=tr.getOrigin().getY();
    tr.setOrigin(OgreBulletCollisions::OgreBtConverter::to(teleportPoint));
}

Weapon* Player::getWeapon(ItemType::ItemTypes weaponType)
{
    return weaponMap[weaponType];
}

OgreBulletDynamics::RigidBody* Player::getRigidBody()
{
    return rigidBody;
}

SceneNode* Player::getSceneNode()
{
    return playerNode;
}

void Player::setPosition(float x, float y, float z)
{
    playerNode->setPosition(x,y,z);
}

AnimationController* Player::getAnimationController()
{
    return animationController;
}

Weapon* Player::getCurrentWeapon()
{
    return currentWeapon;
}

Entity* Player::getEntity()
{
    return playerEntity;
}

bool Player::isFriendly()
{
    return friendly;
}

int Player::getId()
{
    return id;
}

void Player::setOnGround(bool onGround)
{
    this->onGround = onGround;
}

bool Player::isOnGround()
{
    return onGround;
}

bool Player::isFiring()
{
    return firing;
}

void Player::setFiring(bool firing)
{
    this->firing = firing;
}

unsigned char Player::getTeam()
{
	return team;
}