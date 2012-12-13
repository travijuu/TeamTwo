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

#ifndef LASERPISTOL_H
#define	LASERPISTOL_H

/*	LaserPistol

	This class is responsible for creating visual
	parts of LaserPistol weapon such as muzzle flashes,
	muzzleflash lights etc. It inhertirs the Weapon class
	and overrides the necessary methods.


*/

#include "Weapon.h"
#include "LaserPistolBullet.h"
#include "Util/ConfigReader.h"
#include "SoundController.h"

class LaserPistol:public Weapon
{
public:
	//constructor of the class. takes the weapon owner's unique id, team and the information
	//whether the weapon is owned by the local player or not
    LaserPistol(int ownerID,unsigned char team,bool local);
	//this method creates the visual and physical parts of the weapon's bullet.
	//it requires the orientation and the position of the bullet.
    virtual Bullet* createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,Ogre::Vector3 position, Ogre::Quaternion orientation);
	//shortcut for CreateBullet method. as the difference, this method
	//does not require the position of the bullet; it uses the bullet
	//origin of the laser pistol
    Bullet* createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Quaternion orientation);
	//creates and initializes the visual parts of the laser pistol. then, attaches it into
	//the passed Ogre::SceneNode (usually the player's node) object
	void createVisual(SceneManager* sceneManager,SceneNode* playerNode);
	//this method changes the necessary colors of the weapon, its muzzle flash and bullets
	void changeColor(unsigned char color);
private:
	//initializes the necessary variables of the weapon
    virtual void initialize();
};

#endif	/* LASERPISTOL_H */

