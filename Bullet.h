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

#ifndef BULLET_H
#define	BULLET_H
#include "PhysicsShapeHandler.h"
#include "NetworkUtil/MessageStructure.h"
#include "stdafx.h"

/* Bullet
 *
 * This class provides a bullet interface for
 * fire events. Bullet can be visualized different
 * from weapons to weapons. Therefore this class
 * is inherited by other type of bullets and the
 * features like TTL calculating and handling
 * hit events are done in this class.
 */

class Bullet
{
public:
    //constructor of the class. takes necessary objects to create the
    //bullet visuality and its physics. TTL defines the maximum distance
    //that a bullet can go. ownerID indicates the owner of the bullet
    Bullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,int TTL,double speed,int ownerID);
    //returns the Ogre3d scene node
    SceneNode* getBulletNode();
    //returns the rigid body
    OgreBulletDynamics::RigidBody* getRigidBody();
    //returns true if bullet has not hit
    bool isAlive();
    //returns the current distance from the fire origin
    int getTTL();
    //returns the speed of the bullet
    int getSpeed();
    //sets the maximum distance (TTL)
    void setTTL(int TTL);
    //sets the speed of the bullet
    void setSpeed(double speed);
    //returns true if the hit() method has been called
    bool isHit();
    //notices the bullet that it has hit a target.
    //applies the death event on the bullet object
    void hit();
    //returns the owner id of the bullet 
    int getOwnerID();
    //updates the bullet. this method must be called
    //at the every frame of the game
    virtual void update(double time)=0;
    //creates the visual part of the bullet according
    //to the given position and orientation
    virtual void createVisual(Ogre::Vector3 pos, Quaternion orientation)=0;
    //creates the physics for the object according to
    //the given size and its name
    virtual void createPhysics(Ogre::Vector3 hitboxSize,std::string name)=0;
	//changes the color of the laser beam
	virtual void changeColor(unsigned char color)=0;
    //sets type of the weapon that the bullet fired
    void setWeaponType(int weaponType);
    //returns weapon type
    int getWeaponType();

protected:
    Ogre::Vector3 position;
    Quaternion orientation;
    Ogre::Vector3 hitBoxSize;
    double initialYaw;
    bool _hit;
    bool alive;
    int ownerID;
    SceneNode* bulletNode;
    OgreBulletDynamics::RigidBody* bulletRigid;
    int TTL;
    int maxDistance;
    double speed;
    SceneManager* sceneManager;
    OgreBulletDynamics::DynamicsWorld* dynamics;
    Light* light;
    ColourValue diffColor;
    std::string materialName;
    Ogre::Vector3 scaleValue;
    int weaponType;
	BillboardSet* bbset;
};

#endif	/* BULLET_H */

