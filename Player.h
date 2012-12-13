/*
	Copyright (C) 2011-2012  Alican Sekerefe 

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

#ifndef PLAYER_H
#define PLAYER_H
#include "stdafx.h"
#include "Weapon.h"
#include "AnimationController.h"
#include "LaserPistol.h"

namespace PlayerAnimation
{
	enum PlayerAnimations
	{
		MOVING=2,
		BACKWARDS=4,
		FIRING=8,
		CROUCHING=16,
	};
}

/*	PlayerClass

	This class is used for creating visual and
	physical parts of a player, managing them,
	managing health status, inventory etc.

*/
class Player
{

public:
	//consturtor of the class. it requires the id, name of the player. in addition,
	//information of whether the client is local or not, friendly, his/her team must be passed
    Player(int id, std::string name, bool local, bool friendly, unsigned char team,SceneManager* sceneManager);
	//destructor of the class
    ~Player();
	//creates the visual parts of the player. requires orientation, position and an entity name
    void createVisual(std::string entityName, Quaternion orientation, Ogre::Vector3 scale, Ogre::Vector3 position = Ogre::Vector3::ZERO);
    //retunrs the AnimationController of the player
	AnimationController* getAnimationController();
	//sets the position of the player directly
    void setPosition(float x, float y, float z);
	//enables physical processing on the player object
    void enablePhysics(OgreBulletDynamics::DynamicsWorld* dynamics);
	//returns the scenenode of the player
    SceneNode* getSceneNode();
	//retunrs the entity of the player
    Entity* getEntity();
	//returns the rigidbody of the player
    OgreBulletDynamics::RigidBody* getRigidBody();
	//returns the current weapon that the player is holding
    Weapon* getCurrentWeapon();
	//returns a weapon vector by cosidering the given slot
    std::vector<Weapon*> getWeaponsBySlot(WeaponSlot::WeaponSlots slot);
	//returns true if the player is currently firing
    bool isFiring();
	//sets the firing status
    void setFiring(bool firing);
	//returns true if the player is friendly
    bool isFriendly();
	//returns the id of the player
    int getId();
	//decreases the health of the player by the given amount.
	//if it falls below zero, sets the dead flag as true
    void decreaseHealth(int dec);
	//returns true if the player is currently dead
    bool isDead();
	//returns the current health of the player
    int getHealth();
	//sets the health to the given number
    void setHealth(int health);
	//sets the next spawn point that the player will start
    void setNextSpawnPoint(Ogre::Vector3 nextSpawnPoint);
	//returns the next spawn point
    Ogre::Vector3 getNextSpawnPoint();
	//returns the lowest boundary position of the player
    Ogre::Vector3 getBottomPoint();
	//respawns the player, resets its positon etc.
    void respawn();
	//kills the player; plays the death animation
    void die();
	//returns the name of the player
    std::string getName();
	//removes the given item from the inventory
    void removeSpecialItem(ItemType::ItemTypes item);
	//activates the given item for the player
	void activateSpecialItem(ItemType::ItemTypes item);
	//deactivates the given item for the player
	void deactivateSpecialItem(ItemType::ItemTypes item);
	//returns true if the given item has been activated
	bool isActive(ItemType::ItemTypes item);
	//returns true if the player is already holding the passed item
    bool hasItem(ItemType::ItemTypes itemType);
	//returns true if player can take the given item
    bool canTakeItem(ItemType::ItemTypes itemType);
	//adds the given item to the player's item inventory
    void takeItem(ItemType::ItemTypes itemType);
	//switches to the next weapon on the given weapon slot
    bool switchWeapon(WeaponSlot::WeaponSlots weaponSlot);
	//selects the given weapon
    void selectWeapon(ItemType::ItemTypes itemType);
	//switches to the previous weapon
    bool switchToPreviousWeapon();
	//resets the position of the player
    void resetPosition();
	//true is passed if the player is currently colliding with the ground
    void setOnGround(bool onGround);
	//returns true if the player is currently standing on the ground
    bool isOnGround();
	//resets the inventory of the player
    void resetInventory();
	//teleports the player to the given teleport point
    void teleport(Ogre::Vector3 teleportPoint);
	//enables eagle eye mode on the player
    void setEagleEyeMode(bool on);
	//changes the character, weapon and bullet color of the player
	void changePlayerColor(unsigned char color);
	//returns the team of the player
	unsigned char getTeam();
	//returns the weapon with the given type
    Weapon* getWeapon(ItemType::ItemTypes weaponType);
    bool canPush(CollisionObjectInfo* info);


    bool needsUpdate;
    Ogre::Vector3 tempPosition;
    float tempAngleY;
    unsigned char tempAnimation;

private:
	//initializes the inventory by adding default attributes
    void initializeInventory();

    int id;
	unsigned char team;
    std::string name;
    bool firing;
    SceneManager* sceneManager;
    OgreBulletDynamics::DynamicsWorld* dynamics;
    OgreBulletDynamics::RigidBody* rigidBody;
    AnimationController* animationController;
    Weapon* currentWeapon;
    Weapon* previousWeapon;
    SceneNode* playerNode;
    Entity* playerEntity;
    bool friendly;
    bool local;
    bool physicsEnabled;
    bool onGround;
    int weaponIndex;
    std::map<ItemType::ItemTypes, Weapon*> weaponMap;
    std::map<ItemType::ItemTypes, bool> itemInventoryMap;
    int health;
    bool dead;
    Ogre::Vector3 nextSpawnPoint;
    bool immortal;
    bool oneShot;
	bool eagleEye;
    std::vector<MaterialPtr> mats;

};


#endif