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

#ifndef LASERMACHINEGUNBULLET_H
#define	LASERMACHINEGUNBULLET_H

/*	LaserMachineGunBullet

	This class is responsible for visualizing a
	laser machine gun bullet. it shares the same
	attributes with the LaserPistolBullet, therefore,
	it inherits the related class.


*/

#include "LaserPistolBullet.h"

class LaserMachineGunBullet: public LaserPistolBullet
{
public:
	//constructor of the class. take Ogre::SceneManager and DynamicsWorld object for
	//creating visual and physical components of the bullet. In addition, it requires
	//owner id of the weapon owner and the team
    LaserMachineGunBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,int ownerID,unsigned char team);
private:

};

#endif	/* LASERMACHINEGUNBULLET_H */

