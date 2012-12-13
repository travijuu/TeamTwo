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

#include "Bullet.h"

Bullet::Bullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,int TTL,double speed,int ownerID)
{
    this->TTL=this->maxDistance=TTL;
    this->speed=speed;
    this->bulletNode=NULL;
    this->alive=true;
    this->_hit=false;
    this->sceneManager=sceneManager;
    this->dynamics=dynamics;
    this->ownerID=ownerID;
    this->weaponType=-1;
}


SceneNode* Bullet::getBulletNode()
{
    return bulletNode;
}

OgreBulletDynamics::RigidBody* Bullet::getRigidBody()
{
    return bulletRigid;
}

int Bullet::getTTL()
{
    return TTL;
}

int Bullet::getSpeed()
{
    return speed;
}

bool Bullet::isAlive()
{
    return alive;
}

void Bullet::setTTL(int TTL)
{
    this->TTL=TTL;
}

void Bullet::setSpeed(double speed)
{
    this->speed=speed;
}

bool Bullet::isHit()
{
    return _hit;
}

void Bullet::hit()
{
    this->_hit=true;
    TTL=speed;
}

int Bullet::getOwnerID()
{
    return ownerID;
}

void Bullet::setWeaponType(int weaponType)
{
    this->weaponType=weaponType;
}

int Bullet::getWeaponType()
{
    return weaponType;
}