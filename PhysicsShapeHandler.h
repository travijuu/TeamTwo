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

#ifndef PHYSICSSHAPEHANDLER_H
#define PHYSICSSHAPEHANDLER_H

#include "stdafx.h"
#include "CollisionObjectInfo.h"

/* PhysicsHandler

   PhysicsHandler is a physics utility
   based on Bullet Physics engine with
   OgreBullet. It simplifies and specifies
   some common actions such as creating
   a rigidbody or performing a raycast test.
   Since all of the methods are static, it
   can be called anywhere in the code. However,
   it requires a DynamicsWorld object for physics
   operations.

*/

class PhysicsHandler
{
public:
	//creates and returns a BoxCollisionShape by taking Ogre::Entity and a scale factor for the box shape
    static OgreBulletCollisions::BoxCollisionShape* createBoxShape(Ogre::Entity* entity,Ogre::Vector3 scaleFactor=Ogre::Vector3(1,1,1));
	//creates and returns a RigidBody object by taking necessary attributes of the body
    static OgreBulletDynamics::RigidBody* addRigidBody(std::string bodyName,SceneNode* sceneNode,
                                OgreBulletCollisions::CollisionShape* shape,OgreBulletDynamics::DynamicsWorld* dynamics,
                                            Ogre::Vector3 position=Ogre::Vector3::ZERO,float mass=1,Quaternion orientation=Quaternion::IDENTITY);
	//creates and returns a CapsuleShape by taking Entity and a scalefactor for the entity
    static OgreBulletCollisions::CapsuleCollisionShape* createCapsuleShape(Ogre::Entity* entity,Ogre::Vector3 scaleFactor);
	//creates a compound shape, adds the given collision object and shifts it by considering its position 
    static OgreBulletCollisions::CompoundCollisionShape* createCompoundShapeAndAddChild(OgreBulletCollisions::CollisionShape* shape,Ogre::Vector3 position);
    //performs a raycast test by taking the starting and ending points. it stores the collision data to the passed last two parameters
	static bool performRaycastTest(Ogre::Vector3 start, Ogre::Vector3 end,
                    OgreBulletDynamics::DynamicsWorld* dynamics,btCollisionObject* object=NULL,Ogre::Vector3* hitPos = NULL);
	//creates a scaled mesh shape for the given entity. this method should be used for creating static collision shapes
    static btScaledBvhTriangleMeshShape* createScaledTriangleMeshShape(Entity* entity,Ogre::Vector3 scale=Ogre::Vector3(1,1,1));
	//adds a static rigidbody to the given mesh shape.
    static OgreBulletDynamics::RigidBody* addStaticMeshRigidBody(std::string bodyName,btScaledBvhTriangleMeshShape* shape,OgreBulletDynamics::DynamicsWorld* dynamics,
                                            Ogre::Vector3 position=Ogre::Vector3::ZERO,Quaternion orientation=Quaternion::IDENTITY);
    

};
/*
class _CustomMotionState:public btMotionState
{
public:
    _CustomMotionState(const btTransform &pos, Ogre::SceneNode* objectNode,std::string name)
    {
        this->objectNode=objectNode;
        this->position=pos;
        this->name=name;
    }

    void getWorldTransform(btTransform& worldPos) const
    {
        worldPos = position;
    }

    void setWorldTransform(const btTransform& worldPos)
    {
        btQuaternion orientation = worldPos.getRotation();
        Quaternion q(orientation.w(), orientation.x(), orientation.y(), orientation.z());
        objectNode->setOrientation(q);
        btVector3 pos = worldPos.getOrigin();
        objectNode->setPosition(OgreBulletCollisions::BtOgreConverter::to(pos));
    }

protected:
    Ogre::SceneNode* objectNode;
    btTransform position;
    std::string name;
};
*/


#endif