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

#ifndef GAMEITEM_H
#define	GAMEITEM_H

#include "stdafx.h"
#include "CollisionObjectInfo.h"
#include "Util/Logger.h"
#include "PhysicsShapeHandler.h"
#include "Util/Converter.h"
#include "ItemStructure.h"

/* Game Item
 *
 * This class is used for holding data about
 * a game item that can be taken by a player
 * -such as a health-kit- and provides its
 * visuality and physics.
 */

class GameItem
{
public:
    //constructor. takes an item id and its type
    GameItem(int id,ItemType::ItemTypes type);
	//destructor
    ~GameItem();
    //creates a sceneNode and attaches an entity by the given entityName.
    //it also scales the sceneNode and sets its position.
    void createVisual(SceneManager* sceneManager,std::string entityName,std::string materialName,Ogre::Vector3 scaleFactor,Ogre::Vector3 position);
    //creates a rigidbody on the object in order to make it possible
    //to detect whether the player is colliding with the item
    void createPhysics(OgreBulletDynamics::DynamicsWorld* dynamics);
    //rotates the item one frame. this method must be called
    //at the every frame of the graphics application
    void updateGraphics();
    //hides the rigidbody and scene node from the scene
    void hide();
    //shows the rigidbody and scene node on the scene. enabled by default
    void show();
    //returns the rigidbody of the item
    OgreBulletDynamics::RigidBody* getRigidBody();
    //returns true if the item is in "show()" mode
    bool isAlive();
    //returns the id of the item
    int getID();
    //returns the type of the item
    ItemType::ItemTypes getItemType();
	//enables or disables eagle eye mode on the item object. changes its materials
    void setEagleEyeMode(bool value);

private:
    int id;
    ItemType::ItemTypes type;
    SceneNode* itemNode;
    SceneNode* boundaryNode;
    SceneManager* sceneManager;
    Entity* entity;
    bool visible;
    Ogre::Vector3 scaleFactor;
    Ogre::Vector3 position;
    OgreBulletDynamics::RigidBody* rigidBody;
    std::vector<MaterialPtr> mats;
    std::string itemTakeSoundID;
};

#endif	/* GAMEITEM_H */

