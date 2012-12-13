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

#include "Teleporter.h"
#include "CollisionObjectInfo.h"
#include "Util/ConfigReader.h"
#include "SoundController.h"


Teleporter::Teleporter(int id, Ogre::Vector3 inPosition, Ogre::Vector3 outPosition)
{
    this->id=id;
    this->inTelPos=inPosition;
    this->outTelPos=outPosition;
    this->activated=true;
}

void Teleporter::deactivate()
{
    activated=false;
    SoundController::getSingleton()->playSound(inTelSoundID,false);
    SoundController::getSingleton()->playSound(outTelSoundID,false);
}

void Teleporter::activate()
{
    activated=true;
}

void Teleporter::createVisual(SceneManager* sceneManager)
{
    inTelNode=sceneManager->getRootSceneNode()->createChildSceneNode("in_teleporter_node"+Converter::intToString(getID()));
    outTelNode=sceneManager->getRootSceneNode()->createChildSceneNode("out_teleporter_node"+Converter::intToString(getID()));
    inTelEnt=sceneManager->createEntity("TeleporterIn.mesh");
    outTelEnt=sceneManager->createEntity("TeleporterOut.mesh");
    inTelNode->attachObject(inTelEnt);
    outTelNode->attachObject(outTelEnt);
    inTelNode->setPosition(inTelPos);
    outTelNode->setPosition(outTelPos);
    outTelNode->scale(20,20,20);
    inTelNode->scale(20,20,20);
    //inTelNode->translate(0,inTelEnt->getBoundingBox().getSize().y/2*20,0);
    outTelNode->translate(0,outTelEnt->getBoundingBox().getSize().y/2*20,0);


    teleportPointNode=sceneManager->getRootSceneNode()->createChildSceneNode("teleporter_node_blink"+Converter::intToString(getID()));
    teleportPointNode->scale(.2,.2,.2);
    teleportPointNode->setPosition(inTelNode->getPosition()+Ogre::Vector3(0,inTelEnt->getBoundingBox().getSize().y*14,0));

    bbLight=sceneManager->createLight();
    bbLight->setDiffuseColour(0,0,1);
    bbLight->setAttenuation(250, 1.0, 4.5/250, 75./Ogre::Math::Pow(250,2.));
    bbLight->setPosition(teleportPointNode->getPosition());

	inParticleSystem=sceneManager->createParticleSystem("teleporterin_particle"+Converter::intToString(getID()),"TeleporterIN_Particle");
	outParticleSystem=sceneManager->createParticleSystem("teleporterout_particle"+Converter::intToString(getID()),"teleporterOut_Particle");
	inParticleNode=sceneManager->getRootSceneNode()->createChildSceneNode();
	outParticleNode=sceneManager->getRootSceneNode()->createChildSceneNode();
	inParticleNode->attachObject(inParticleSystem);
	outParticleNode->attachObject(outParticleSystem);
	inParticleNode->scale(.05,.05,.05);
	outParticleNode->scale(.05,.05,.05);
	inParticleNode->setPosition(teleportPointNode->getPosition());
	outParticleNode->setPosition(outTelNode->getPosition()+Ogre::Vector3(0,25,0));
	inParticleSystem->setRenderQueueGroup(RENDER_QUEUE_6);
	outParticleSystem->setRenderQueueGroup(RENDER_QUEUE_6);


    inTelSoundID="in_tel_"+Converter::intToString(id);
    outTelSoundID="out_tel_"+Converter::intToString(id);
    SoundController::getSingleton()->attachDynamicSound(inTelNode,inTelSoundID,ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","teleport-in"),false);
    SoundController::getSingleton()->attachDynamicSound(outTelNode,outTelSoundID,ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","teleport-out"),false);
}

void Teleporter::createPhysics(OgreBulletDynamics::DynamicsWorld* dynamics)
{
    static int count = 0;

    btScaledBvhTriangleMeshShape* scaledTri=PhysicsHandler::createScaledTriangleMeshShape(inTelEnt,inTelNode->getScale());
    inTelRigid=PhysicsHandler::addStaticMeshRigidBody("Teleporter_IN" + Converter::intToString(count),scaledTri,dynamics,inTelNode->getPosition(),inTelNode->getOrientation());
    inTelRigid->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    CollisionObjectInfo* intelInfo = new CollisionObjectInfo(inTelNode, inTelRigid, CollisionObjectTypes::STATIC_OBJECT, this);
    inTelRigid->getBulletRigidBody()->setUserPointer(intelInfo);
    inTelRigid->getBulletRigidBody()->setFriction(.1);
    inTelRigid->getBulletRigidBody()->setRestitution(0);
    
    scaledTri=PhysicsHandler::createScaledTriangleMeshShape(outTelEnt,outTelNode->getScale());
    outTelRigid=PhysicsHandler::addStaticMeshRigidBody("Teleporter_OUT" + Converter::intToString(count),scaledTri,dynamics,outTelNode->getPosition(),outTelNode->getOrientation());
    outTelRigid->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    CollisionObjectInfo* outtelInfo = new CollisionObjectInfo(outTelNode, outTelRigid, CollisionObjectTypes::STATIC_OBJECT, this);
    outTelRigid->getBulletRigidBody()->setUserPointer(outtelInfo);
    outTelRigid->getBulletRigidBody()->setFriction(.1);
    outTelRigid->getBulletRigidBody()->setRestitution(0);

    OgreBulletCollisions::BoxCollisionShape* entranceShape = PhysicsHandler::createBoxShape(inTelEnt, teleportPointNode->getScale());
    teleportPointRigid = PhysicsHandler::addRigidBody("Teleporter_Entrance" + Converter::intToString(++count), teleportPointNode, entranceShape, dynamics,teleportPointNode->getPosition(), 1);
    teleportPointRigid->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    teleportPointRigid->setKinematicObject(true);
    CollisionObjectInfo* bbInfo = new CollisionObjectInfo(teleportPointNode, teleportPointRigid, CollisionObjectTypes::TELEPORTER, this);
    teleportPointRigid->getBulletRigidBody()->setUserPointer(bbInfo);
    teleportPointRigid->getBulletRigidBody()->setCollisionFlags(teleportPointRigid->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CO_GHOST_OBJECT);
    
    count++;
}

void Teleporter::updateGraphics()
{
    if(activated)
    {
        if(bbLight->getDiffuseColour().b<1)
            bbLight->setDiffuseColour(0,0,bbLight->getDiffuseColour().b+1/25.);
        if(inParticleNode->getScale().x<.05)
		{
            inParticleNode->setScale(inParticleNode->getScale()+.05/25.);
            outParticleNode->setScale(outParticleNode->getScale()+.05/25.);
		}
    }
    else
    {
        if(bbLight->getDiffuseColour().b>0)
            bbLight->setDiffuseColour(0,0,bbLight->getDiffuseColour().b-1/25.);
        if(inParticleNode->getScale().x>0)
		{
            inParticleNode->setScale(inParticleNode->getScale()-.05/25.);
            outParticleNode->setScale(outParticleNode->getScale()-.05/25.);
		}
	}
}

void Teleporter::setEnabled(bool value)
{
    activated=value;
}

bool Teleporter::isActivated()
{
    return activated;
}

OgreBulletDynamics::RigidBody* Teleporter::getTeleportPointRigid()
{
    return teleportPointRigid;
}

int Teleporter::getID()
{
    return id;
}

Ogre::Vector3 Teleporter::getOutPosition()
{
    return outTelPos;
}
