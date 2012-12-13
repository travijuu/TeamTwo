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

#include "LaserMachineGunBullet.h"

LaserMachineGunBullet::LaserMachineGunBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,int _ownerID,unsigned char team): LaserPistolBullet(sceneManager,dynamics,_ownerID,team)
{
    this->TTL=this->maxDistance=2000*35;
    this->speed=48*35;
    diffColor=ColourValue(1,0,0);
    if (team==0)
        materialName = "Laserball_Blue";
    else
        materialName = "Laserball_Red";
    scaleValue=Ogre::Vector3(.1,.1,.1);
}