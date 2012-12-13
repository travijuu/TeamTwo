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

#include "LaserMachineGun.h"
#include "LaserMachineGunBullet.h"
#include "Util/Converter.h"
#include "SoundController.h"
#include "Util/ConfigReader.h"

LaserMachineGun::LaserMachineGun(int ownerID,unsigned char team,bool local):Weapon(ownerID,ItemType::LASER_MACHINE_GUN,team)
{
    initialize();
	this->local=local;
}

void LaserMachineGun::initialize()
{
    clipAmmo=60;
    clipSize=60;
    clipCount=6;
    maxClip=10;
    totalAmmo=(clipSize*clipCount);
    roundsPerSecond=8;
    reloadDuration=2000;
    nextFireTime=-roundsPerSecond;
    reloadEndTime=-reloadDuration;
    reloading=false;
    weaponSlot=WeaponSlot::Primary;
    name="LaserMachineGun";
    fireSoundID=name+"_"+Converter::intToString(ownerID);
    // bulletOrigin=Vector3(3.5, 23.6001, -5.6)
}

Bullet* LaserMachineGun::createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,Ogre::Vector3 position, Ogre::Quaternion orientation)
{
    static int bulletID=0;
    LaserMachineGunBullet* bullet=new LaserMachineGunBullet(sceneManager,dynamics,ownerID,color);
    bullet->setWeaponType(ItemType::LASER_MACHINE_GUN);
    bullet->createVisual(position,orientation);
    //muzzleFlashNode->setPosition(position);
    //muzzleFlashNode->resetOrientation();dddddd

    Entity* ent=sceneManager->createEntity("cube.mesh");
    Ogre::Vector3 size=ent->getBoundingBox().getSize();
    size*=.02;
    size.z*=2.5;
    sceneManager->destroyEntity(ent);

    bullet->createPhysics(size,"LMG_bullet_"+Converter::intToString(bulletID++));

    return bullet;
}

void LaserMachineGun::createVisual(SceneManager* sceneManager,SceneNode* playerNode)
{
    weaponNode = playerNode->createChildSceneNode();
	weaponEntity = sceneManager->createEntity("LaserRifle.mesh");

    weaponNode->attachObject(weaponEntity);
    weaponNode->rotate(Ogre::Vector3::UNIT_Y, Degree(88));
    weaponNode->setPosition(1.5, 3.6, -3);
    weaponNode->scale(.9, 1.1, 1.9);
    bulletOrigin = Ogre::Vector3(2, 7, -18);

    this->playerNode=playerNode;
    weaponLight=sceneManager->createLight();
    weaponLight->setPosition(bulletOrigin);
    weaponLight->setType(Light::LT_POINT);
    weaponLight->setCastShadows(true);
    weaponLight->setAttenuation(750, 1.0, 4.5/750., 75./Ogre::Math::Pow(750.,2.));
    weaponLight->setVisible(false);
    weaponNode->attachObject(weaponLight);


    muzzleFlashNode=weaponNode->createChildSceneNode();
    muzzleFlashEntity = sceneManager->createEntity("MuzzleFlashPlane.mesh");

	if(local)
		muzzleFlashEntity->setRenderQueueGroup(RENDER_QUEUE_7);
	else
		muzzleFlashEntity->setRenderQueueGroup(RENDER_QUEUE_6);
    muzzleFlashEntity->setCastShadows(false);
    //ent->setMaterialName("_muzzleflash2");
    muzzleFlashNode->attachObject(muzzleFlashEntity);
    muzzleFlashNode->rotate(Ogre::Vector3::UNIT_X, Degree(90));
    muzzleFlashNode->rotate(Ogre::Vector3::UNIT_Y, Degree(180));
    muzzleFlashNode->setPosition(4.6,.7,0);
    muzzleFlashNode->setInheritScale(false);
    muzzleFlashNode->scale(8,8,8);
    muzzleFlashNode->setVisible(true);

	changeColor(team);

    std::string fireSound=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","lmg-fire");
    reloadSoundIDs.push_back("lmg-reload-1");
    reloadSoundIDs.push_back("lmg-reload-2");
    reloadSoundIDs.push_back("lmg-reload-3");
    SoundController::getSingleton()->attachDynamicSound(weaponNode,fireSoundID,fireSound,false);
    SoundController::getSingleton()->attachDynamicSound(weaponNode,"lmg-reload-1",ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","lmg-reload-1"),false);
    SoundController::getSingleton()->attachDynamicSound(weaponNode,"lmg-reload-2",ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","lmg-reload-2"),false);
    SoundController::getSingleton()->attachDynamicSound(weaponNode,"lmg-reload-3",ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","lmg-reload-3"),false);

}

Bullet* LaserMachineGun::createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Quaternion orientation)
{
    if(playerNode!=NULL)
    {
        Ogre::Vector3 pos=playerNode->getOrientation()*bulletOrigin+playerNode->getPosition();
        return createBullet(sceneManager,dynamics,pos,orientation);
    }
    return NULL;
}

void LaserMachineGun::changeColor(unsigned char color)
{
    if (color==0)
    {
		muzzleFlashEntity->getSubEntity(0)->setMaterialName("MuzzleFlash1Side_Blue");
        muzzleFlashEntity->getSubEntity(1)->setMaterialName("MuzzleFlash1Front_Blue");
		weaponEntity->getSubEntity(1)->setMaterialName("LaserSight_Blue");
        weaponLight->setDiffuseColour(0, 0, 1);
    }
    
    else
    {
        muzzleFlashEntity->getSubEntity(0)->setMaterialName("MuzzleFlash1Side_Red");
        muzzleFlashEntity->getSubEntity(1)->setMaterialName("MuzzleFlash1Front_Red");
		weaponEntity->getSubEntity(1)->setMaterialName("LaserSight_Red");
        weaponLight->setDiffuseColour(1, 0, 0);
    }
	this->color=color;
}
