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

#ifndef LASERPISTOLBULLET_H
#define	LASERPISTOLBULLET_H

/*	LaserPistolBullet

	This class creates the visual and physical
	parts of the LaserPistol object.

*/

#include "Bullet.h"

class LaserPistolBullet:public Bullet
{
public:
	//constructor. gets necessary parameters to initialize visual and physical parts of the object
	//in addition, it requires the owner id of the weapon/bullet and team
    LaserPistolBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,int ownerID,unsigned char team);
    //updates the position and the graphics (if necessary) of the bullet object
	virtual void update(double time);
	//creates the visual parts of the bullet
    virtual void createVisual(Ogre::Vector3 pos, Quaternion orientation);
	//creates the physical parts of the bullet
    virtual void createPhysics(Ogre::Vector3 hitboxSize,std::string name);
	//changes the color of the bullet
	virtual void changeColor(unsigned char color);
private:

};

#endif	/* LASERPISTOLBULLET_H */

