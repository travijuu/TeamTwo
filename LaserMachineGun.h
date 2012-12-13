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

#ifndef LASERMACHINEGUN_H
#define	LASERMACHINEGUN_H

/*
	LaserMachineGun class

	This class is responsible for creating and managing
	visual parts of the laser machine gun object
	and create bullets for the gun.

	This class inherits the main attributes of the Weapon 
	class.

*/

#include "Weapon.h"

class LaserMachineGun: public Weapon
{
public:
	//constructor of the class. takes the ownerid of the gun, team and a
	//boolean which indicates the local ownership.
    LaserMachineGun(int ownerID,unsigned char team,bool local);
	//creates a bullet object on the scene and returns it. this method
	//requires Ogre::SceneManager and DynamicsWorld object for visualizing
	//the bullet object. In addition, it also requires the bullet's origin
	//and its orientation
    Bullet* createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Vector3 position, Ogre::Quaternion orientation);
	//shortcut for CreateBullet method. as the difference, this method
	//does not require the position of the bullet; it uses the bullet
	//origin of the laser machine gun
    Bullet* createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Quaternion orientation);
	//creates the visual parts of the weapon. takes Ogre::SceneManager and the 
	//Ogre::Node of the player who uses it.
	void createVisual(SceneManager* sceneManager,SceneNode* playerNode);
	//changes the color/team of the weapon 
	void changeColor(unsigned char color);
private:
	//initializes the variables
    void initialize();

};

#endif	/* LASERMACHINEGUN_H */

