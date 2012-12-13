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

#include "GameItem.h"
#include "SoundController.h"
#include "Util/ConfigReader.h"

GameItem::GameItem(int id,ItemType::ItemTypes type)
{
    this->id=id;
    this->type=type;
    this->itemNode=NULL;
    this->boundaryNode=NULL;
    this->visible=true;
    this->rigidBody=NULL;
    this->sceneManager=NULL;
    this->itemTakeSoundID="item_"+Converter::intToString(id);
}

GameItem::~GameItem()
{
    if(sceneManager!=NULL)
    {
        delete rigidBody;
        boundaryNode->detachAllObjects();
        sceneManager->destroySceneNode(boundaryNode);
        itemNode->detachAllObjects();
        sceneManager->destroySceneNode(itemNode);
    }
}

void GameItem::createVisual(SceneManager* sceneManager,std::string entityName,std::string materialName,Ogre::Vector3 scaleFactor,Ogre::Vector3 position)
{
    if(itemNode==NULL)
    {
		Logger::getSingleton()->addLine("Item: creating visual");
        this->sceneManager=sceneManager;
        this->scaleFactor=scaleFactor;
        this->position=position;
        boundaryNode=sceneManager->getRootSceneNode()->createChildSceneNode();
        itemNode=boundaryNode->createChildSceneNode();
        entity=sceneManager->createEntity(entityName);
        if(materialName!="NULL")
            entity->setMaterialName(materialName);
        entity->setCastShadows(false);
        
        entity->setRenderQueueGroup(RENDER_QUEUE_6);

        itemNode->attachObject(entity);
        itemNode->scale(scaleFactor);
        boundaryNode->setPosition(position);
        itemNode->rotate(Quaternion(Degree(45),Ogre::Vector3(1,1,1)));

        //save materials...
        for (int i = 0; i < entity->getNumSubEntities(); i++)
            mats.push_back(entity->getSubEntity(i)->getMaterial());
		Logger::getSingleton()->addLine("Item: attaching sounds");
		std::string soundFile=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","item-take");
        SoundController::getSingleton()->attachDynamicSound(itemNode,itemTakeSoundID,ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","item-take"));
        Logger::getSingleton()->addLine("Item: item visual created");
    }
    else
        Logger::getSingleton()->addLine("Item: cannot create item. it already exists.");
}

void GameItem::createPhysics(OgreBulletDynamics::DynamicsWorld* dynamics)
{
	Logger::getSingleton()->addLine("Item: creating physics");
    if(itemNode!=NULL)
    {
        static int count=0;
        OgreBulletCollisions::BoxCollisionShape* boxShape=PhysicsHandler::createBoxShape(entity,scaleFactor);
        rigidBody=PhysicsHandler::addRigidBody("Item"+Converter::intToString(count),boundaryNode,boxShape,dynamics,position,1,Quaternion(Degree(45),Ogre::Vector3(1,1,1)));
        rigidBody->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
        rigidBody->setKinematicObject(true);
        CollisionObjectInfo* info=new CollisionObjectInfo(boundaryNode,rigidBody,CollisionObjectTypes::ITEM,this);
        rigidBody->getBulletRigidBody()->setUserPointer(info);
        rigidBody->getBulletRigidBody()->setCollisionFlags(rigidBody->getBulletRigidBody()->getCollisionFlags()|btCollisionObject::CO_GHOST_OBJECT);
        count++;
		Logger::getSingleton()->addLine("Item: item physics created");
	}
    else
        Logger::getSingleton()->addLine("Item: cannot create physics; itemNode is NULL!");
}

void GameItem::setEagleEyeMode(bool on)
{
    if(itemNode!=NULL)
    {
        if(on)
            entity->setMaterial((MaterialPtr)MaterialManager::getSingletonPtr()->getByName("item"));
        else
        {
            for (int i = 0; i < entity->getNumSubEntities(); i++)
                entity->getSubEntity(i)->setMaterial(mats.at(i));
        }
    }
}

void GameItem::updateGraphics()
{
    if(itemNode!=NULL)
    {
        if(visible && itemNode->getScale().x<scaleFactor.x)
            itemNode->setScale(itemNode->getScale()+scaleFactor.x/50.);
        else if(!visible)
        {
            if(itemNode->getScale().x>0)
                itemNode->setScale(itemNode->getScale()-scaleFactor.x/50.);
            else
                itemNode->setVisible(false);
        }
        boundaryNode->rotate(Ogre::Vector3(0,1,0),Degree(1.5));
    }
}

void GameItem::show()
{
    if(boundaryNode!=NULL)
    {
        boundaryNode->setVisible(true);
        rigidBody->setVisible(true);
    }
    visible=true;
}

void GameItem::hide()
{
    if(boundaryNode!=NULL)
        rigidBody->setVisible(false);
    visible=false;
    SoundController::getSingleton()->playSound(itemTakeSoundID,false);
}

OgreBulletDynamics::RigidBody* GameItem::getRigidBody()
{
    return rigidBody;
}

bool GameItem::isAlive()
{
    return visible;
}

int GameItem::getID()
{
    return id;
}

ItemType::ItemTypes GameItem::getItemType()
{
    return type;
}