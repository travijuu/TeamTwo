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

#include "LaserPistolBullet.h"
#include "Util/Converter.h"

LaserPistolBullet::LaserPistolBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,int ownerId,unsigned char team):Bullet(sceneManager,dynamics,1000*60,16*60,ownerId)
{
    diffColor=ColourValue(0,0,1);
	if(team==0)
		materialName="Laserball_Blue";
	else
		materialName="Laserball_Red";
    scaleValue=Ogre::Vector3(.1,.1,.1);
    this->ownerID=ownerId;
}

void LaserPistolBullet::createPhysics(Ogre::Vector3 hitboxSize,std::string name)
{
    if (bulletNode == NULL)
        return;
    this->dynamics = dynamics;
    this->hitBoxSize = hitboxSize;
    //OgreBulletCollisions::BoxCollisionShape* box = new OgreBulletCollisions::BoxCollisionShape(hitboxSize);
    OgreBulletCollisions::SphereCollisionShape* sphere=new OgreBulletCollisions::SphereCollisionShape((hitboxSize.x+hitboxSize.z)/4.);
    bulletRigid = new OgreBulletDynamics::RigidBody(name, dynamics);
    bulletRigid->setShape(bulletNode, sphere, 0, 0, .000001, position, orientation);
    bulletRigid->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    bulletRigid->getBulletRigidBody()->setLinearFactor(btVector3(0, 0, 0));
    bulletRigid->getBulletRigidBody()->setAngularFactor(btVector3(0, 0, 0));
    bulletRigid->getBulletRigidBody()->setGravity(btVector3(0, 0, 0));
    bulletRigid->getBulletRigidBody()->setCollisionFlags(bulletRigid->getBulletRigidBody()->getCollisionFlags());
    bulletRigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(bulletNode,bulletRigid,CollisionObjectTypes::BULLET,this));

    bulletRigid->getBulletRigidBody()->setCcdMotionThreshold(.0001);
    bulletRigid->getBulletRigidBody()->setCcdSweptSphereRadius(.2);
}


void LaserPistolBullet::update(double time)
{
    //if ((light->getVisible() && maxDistance - TTL > (speed * 2)) || isHit())
     //   light->setVisible(false);

    if(TTL>0 && !isHit())
    {
        Ogre::Vector3 wd=bulletNode->getOrientation()*Ogre::Vector3(0,0,-speed);
        bulletRigid->getBulletRigidBody()->setLinearVelocity(btVector3(wd.x,wd.y,wd.z));
        TTL-=speed;
        double scalar=((float)TTL)/(float)maxDistance;
        //light->setDiffuseColour(diffColor*scalar);
    }
    else
    {
        alive=false;
        bulletNode->setVisible(false);
        delete bulletRigid;
    }
}

void LaserPistolBullet::createVisual(Ogre::Vector3 pos, Quaternion orientation)
{
    this->position=pos;
    this->orientation=orientation;
    if(bulletNode!=NULL)
        sceneManager->destroySceneNode(bulletNode);
    bbset=sceneManager->createBillboardSet();
    bbset->setMaterialName(materialName);
    bbset->createBillboard(0,0,0);

    bulletNode=sceneManager->getRootSceneNode()->createChildSceneNode();
    //bulletNode->attachObject(light);
    bulletNode->attachObject(bbset);
    bulletNode->setPosition(pos);
    bulletNode->setOrientation(orientation);
    bulletNode->scale(scaleValue);
}

void LaserPistolBullet::changeColor(unsigned char color)
{
	if(color==0)
		materialName="Laserball_Blue";	
	else	
		materialName="Laserball_Red";
	bbset->setMaterialName(materialName);
}