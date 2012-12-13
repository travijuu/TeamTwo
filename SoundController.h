/*
	Copyright (C) 2011-2012  Erkin Cakar

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

#ifndef SOUNDCONTROLLER_H
#define	SOUNDCONTROLLER_H

#include <OgreAL/OgreAL.h>
#include <OgreAL/OgreALListener.h>

class SoundController {

public:
    SoundController();
    ~SoundController();
    static SoundController* getSingleton();

    void attachDynamicSound( Ogre::SceneNode* sceneNode, std::string name, std::string path, bool repat=false );
    void attachStaticSound( std::string name, std::string path, bool repeat=false );
    void playSound( std::string name, bool restart=false );
    void pauseSound( std::string name );
    void stopSound( std::string name );
    void updateAllSoundPositions();

    void setBackgroundSound( std::string path, bool repeat=true );
    void setBackgroundSoundVolume(float level);
    void playBackgroundSound();
    void pauseBackgroundSound();
    void stopBackgroundSound();
    void setListener(Ogre::SceneNode* listenerNode);

    bool deleteSound(std::string name);
    void deleteAllSounds();


    static SoundController *singleton;
    OgreAL::SoundManager *soundManager;
private:
    std::map<std::string, Ogre::SceneNode*> dynamicSoundList;
    std::vector<std::string> staticSoundList;
    std::string backgroundSound;
    Ogre::SceneNode* listenerNode;

};

#endif	/* SOUNDCONTROLLER_H */

