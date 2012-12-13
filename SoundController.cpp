/*
	Copyright (C) 2011-2012 Erkin Cakar

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

#include <iostream>

#include "SoundController.h"


SoundController::SoundController()
{
    soundManager = new OgreAL::SoundManager();
    backgroundSound = "";
	listenerNode=NULL;
}

SoundController::~SoundController() {
}
SoundController* SoundController::singleton=NULL;

SoundController* SoundController::getSingleton()
{
    if(singleton==NULL)
        singleton=new SoundController();
    return singleton;
}

void SoundController::attachDynamicSound(Ogre::SceneNode* node, std::string name, std::string path, bool repeat)
{
    if( dynamicSoundList[name] == NULL )
    {
        node->attachObject(soundManager->createSound(name, path, repeat));
        dynamicSoundList[name] = node;
    }
}
void SoundController::attachStaticSound(std::string name, std::string path, bool repeat)
{
	soundManager->createSound(name,path,repeat);
	staticSoundList.push_back(name);
}

bool SoundController::deleteSound(std::string name)
{
    dynamicSoundList.erase(name);
    soundManager->destroySound(name);
	return true;
}

void SoundController::setListener(Ogre::SceneNode* listenerNode)
{
	this->listenerNode=listenerNode;
	listenerNode->attachObject((Ogre::MovableObject*)soundManager->getListener());
}

void SoundController::updateAllSoundPositions()
{
	if(listenerNode!=NULL)
	{
		//soundManager->getListener()->setPosition(listenerNode->getPosition());
		//std::cout<<"listener dir: "<<soundManager->getListener()->getDirection()<<std::endl;
	}

    std::map<std::string, Ogre::SceneNode*>::iterator itr;
    for(itr=dynamicSoundList.begin();itr!=dynamicSoundList.end();itr++)
	{
        //soundManager->getSound(itr->first)->setPosition(itr->second->getPosition());
		//soundManager->getSound(itr->first)->setDirection(itr->second->_getDerivedOrientation()*Ogre::Vector3::NEGATIVE_UNIT_Z);
		//std::cout<<"node     dir: "<<soundManager->getSound(itr->first)->getDirection()<<std::endl;
	}

}
void SoundController::deleteAllSounds()
{
    std::map<std::string, Ogre::SceneNode*>::iterator itr;
    for(itr=dynamicSoundList.begin();itr!=dynamicSoundList.end();itr++)
        soundManager->destroySound(itr->first);
    dynamicSoundList.clear();
}

void SoundController::playSound(std::string name, bool restart)
{
    if(restart)
        soundManager->getSound(name)->stop();
    soundManager->getSound(name)->play();
}

void SoundController::setBackgroundSound(std::string path, bool repeat)
{
    OgreAL::Sound* sound = soundManager->createSound(path,path,repeat);
    //sound->fadeIn(3);
    backgroundSound = path;
}
void SoundController::playBackgroundSound()
{
    if(backgroundSound != "")
    {
        soundManager->getSound(backgroundSound)->setGain(.3);
        soundManager->getSound(backgroundSound)->play();
    }

}

void SoundController::setBackgroundSoundVolume(float level)
{
    if(backgroundSound!="")
        soundManager->getSound(backgroundSound)->setGain(level);
}

void SoundController::pauseBackgroundSound()
{
    if(soundManager->getSound(backgroundSound)->isPlaying())
        soundManager->getSound(backgroundSound)->pause();
}
void SoundController::stopBackgroundSound()
{
    if(soundManager->getSound(backgroundSound)->isPlaying())
        soundManager->getSound(backgroundSound)->stop();
}
