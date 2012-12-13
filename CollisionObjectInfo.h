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

#ifndef COLLISIONOBJECTINFO_H
#define	COLLISIONOBJECTINFO_H
#include "stdafx.h"

/* Collision Object Info
 *
 * This class is responsible for containing
 * a collision object information to the specified
 * type of an scene object. It provides some
 * information like object type and its pointer. 
 * The class is used for detecting the collided
 * object's type when a physics event happened.
 */


namespace CollisionObjectTypes
{
    enum CollisionObjectType
    {
        WALL=0,
        LOCAL_PLAYER,
        REMOTE_PLAYER,
        DYNAMIC_OBJECT,
        GROUND,
        BULLET,
        ITEM,
        TOP,
        TELEPORTER,
        STATIC_OBJECT,
        HIT_DECAL
    };
}

class CollisionObjectInfo
{
public:
    CollisionObjectInfo();
    //constructor of the class. Takes necessary objects
    //to identify the type of the object. In addition,
    //it also takes a user-specified object.
    CollisionObjectInfo(SceneNode* objectNode,OgreBulletDynamics::RigidBody* objectRigid,int objectType,void* specialObject);
    //returns the added Ogre scene node
    SceneNode* getSceneNode();
    //returns the type of the object specified in
    //CollisionObjectType enumerator
    int getObjectType();
    //returns the added rigidbody of the object
    OgreBulletDynamics::RigidBody* getRigidBody();
    //retuns a user-specified object as void*
    void* getSpecialObject();
private:
    SceneNode* objectNode;
    int objectType;
    OgreBulletDynamics::RigidBody* objectRigid;
    void* specialObject;

};

#endif	/* COLLISIONOBJECTINFO_H */

