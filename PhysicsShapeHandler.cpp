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

#include "PhysicsShapeHandler.h"
#include "Util/Logger.h"
#include "Util/ConfigReader.h"

OgreBulletCollisions::BoxCollisionShape* PhysicsHandler::createBoxShape(Ogre::Entity* entity,Ogre::Vector3 scaleFactor)
{
    Logger::getSingleton()->addLine("createBoxShape: creating box shape");
    OgreBulletCollisions::BoxCollisionShape* boxShape=NULL;
    if(entity!=NULL)
    {
        Ogre::Vector3 size=entity->getBoundingBox().getSize();
        size*=.96;
        size*=scaleFactor;
        size/=2;        
        boxShape=new OgreBulletCollisions::BoxCollisionShape(size);

    }
    else
        Logger::getSingleton()->addLine("PhysicsShapeHandler: box shape cannot be created! entity is NULL!");
    return boxShape;
}

OgreBulletCollisions::CapsuleCollisionShape* PhysicsHandler::createCapsuleShape(Ogre::Entity* entity,Ogre::Vector3 scaleFactor)
{
    OgreBulletCollisions::CapsuleCollisionShape* capsule=NULL;
    if(entity!=NULL)
    {
		Ogre::Vector3 size=entity->getBoundingBox().getSize();
		float radius=size.x/2.*scaleFactor.x*.85;
		float height=size.y/2.*scaleFactor.x*.95;
		//radius=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsDouble("capsule","rad");
		//height=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsDouble("capsule","height");
        capsule=new OgreBulletCollisions::CapsuleCollisionShape(radius,height,Ogre::Vector3(0,1,0));
    }
    else
        Logger::getSingleton()->addLine("createCapsuleShape: capsule shape cannot be created! entity is NULL!");
   
    return capsule;
}

OgreBulletCollisions::CompoundCollisionShape* PhysicsHandler::createCompoundShapeAndAddChild(OgreBulletCollisions::CollisionShape* shape, Ogre::Vector3 position)
{
    OgreBulletCollisions::CompoundCollisionShape* compound=NULL;
    if(shape!=NULL)
    {
        compound=new OgreBulletCollisions::CompoundCollisionShape();
        compound->addChildShape(shape,position);
    }
    else
        Logger::getSingleton()->addLine("createCompoundShapeAndAddChild: compound shape cannot be created! shape is NULL!");
    
    return compound;
}



OgreBulletDynamics::RigidBody* PhysicsHandler::addRigidBody(std::string bodyName,Ogre::SceneNode* sceneNode,
        OgreBulletCollisions::CollisionShape* shape,OgreBulletDynamics::DynamicsWorld* dynamics,Ogre::Vector3 position,float mass,Quaternion orientation)
{
    OgreBulletDynamics::RigidBody* rigidbody=NULL;;
    if(shape!=NULL && dynamics!=NULL)
    {
        rigidbody=new OgreBulletDynamics::RigidBody(bodyName,dynamics);
        rigidbody->setShape(sceneNode,shape,.6,.6,mass,position,orientation);
    }
    else
        Logger::getSingleton()->addLine("addRigidBody: rigidbody cannot be created! one/all of the parameters is/are NULL!");
    return rigidbody;
}

  OgreBulletDynamics::RigidBody* PhysicsHandler::addStaticMeshRigidBody(std::string bodyName,btScaledBvhTriangleMeshShape* shape,
                                        OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Vector3 position,Quaternion orientation)
  {
      OgreBulletDynamics::RigidBody* rigidBody=NULL;
      if(shape!=NULL && dynamics!=NULL)
      {
          rigidBody=new OgreBulletDynamics::RigidBody(bodyName,dynamics);
          rigidBody->setStaticShape(shape,0,100,position,orientation);
          Logger::getSingleton()->addLine("PhysicsHandler: static mesh shape added to a rigidbody ("+bodyName+")");
      }
      else
          Logger::getSingleton()->addLine("PhysicsHandler: cannot add static mesh shape!");
      return rigidBody;
  }



bool PhysicsHandler::performRaycastTest(Ogre::Vector3 start, Ogre::Vector3 end,OgreBulletDynamics::DynamicsWorld* dynamics,btCollisionObject* object,Ogre::Vector3* hitPos)
{
    bool result=false;
    btVector3 startVector(start.x,start.y,start.z);
    btVector3 endVector(end.x,end.y,end.z);
    btCollisionWorld::ClosestRayResultCallback callback(startVector,endVector);
    dynamics->getBulletCollisionWorld()->rayTest(startVector,endVector,callback);
    if((result=callback.hasHit()) && callback.m_collisionObject!=NULL)
    {
		if(object!=NULL)
			*object = *callback.m_collisionObject;
        if (hitPos != NULL)
            *hitPos = OgreBulletCollisions::BtOgreConverter::to(callback.m_hitPointWorld);
    }
    return result;
}

btScaledBvhTriangleMeshShape* PhysicsHandler::createScaledTriangleMeshShape(Entity* entity,Ogre::Vector3 scale)
{
    OgreBulletCollisions::StaticMeshToShapeConverter* converter=new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
    OgreBulletCollisions::TriangleMeshCollisionShape* triShape=converter->createTrimesh();
    delete converter;
    btScaledBvhTriangleMeshShape* scaledTri=new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)triShape->getBulletShape(),OgreBulletCollisions::OgreBtConverter::to(scale));
    return scaledTri;
}
