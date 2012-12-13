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

#include "LaserPistol.h"
#include "Util/Converter.h"

LaserPistol::LaserPistol(int ownerID,unsigned char team,bool local):Weapon(ownerID,ItemType::LASER_PISTOL,team)
{
	this->local=local;
    initialize();
}

void LaserPistol::initialize()
{
    clipAmmo=12;
    clipSize=12;
    clipCount=6;
    totalAmmo=clipSize*clipCount-(clipSize-clipAmmo);
    maxClip=8;
    roundsPerSecond=2;
    reloadDuration=2000;
    nextFireTime=-roundsPerSecond;
    reloadEndTime=-reloadDuration;
    reloading=false;
    weaponSlot=WeaponSlot::Secondary;
    name="LaserPistol";
    bulletOrigin=Ogre::Vector3(0,20,0);
    fireSoundID=name+"_"+Converter::intToString(ownerID);
}

Bullet* LaserPistol::createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics,Ogre::Vector3 position, Ogre::Quaternion orientation)
{
    static int bulletID=0;
    LaserPistolBullet* bullet=new LaserPistolBullet(sceneManager,dynamics,ownerID,color);
    bullet->setWeaponType(ItemType::LASER_PISTOL);
    bullet->createVisual(position,orientation);

    Entity* ent=sceneManager->createEntity("cube.mesh");
    Ogre::Vector3 size=ent->getBoundingBox().getSize();
    size*=.02;
    sceneManager->destroyEntity(ent);

    bullet->createPhysics(size,"LP_bullet_"+Converter::intToString(bulletID++));

    return bullet;
}


void LaserPistol::createVisual(SceneManager* sceneManager,SceneNode* playerNode)
{
    weaponNode = playerNode->createChildSceneNode();
	weaponEntity = sceneManager->createEntity("Weapon2.mesh");
    weaponNode->attachObject(weaponEntity);
    weaponNode->rotate(Ogre::Vector3::UNIT_Y, Degree(88));
    weaponNode->setPosition(1.6, 3.8, -5);
    weaponNode->scale(1.6,1.26,1.6);
    bulletOrigin = Ogre::Vector3(2, 7.2, -19);
    this->playerNode=playerNode;
    weaponLight=sceneManager->createLight();
    weaponLight->setPosition(bulletOrigin);
    weaponLight->setType(Light::LT_POINT);
    weaponLight->setCastShadows(true);
    weaponLight->setAttenuation(750, 1.0, 4.5/750., 75./Ogre::Math::Pow(750.,2.));
    weaponLight->setVisible(false);
    weaponNode->attachObject(weaponLight);
	
    muzzleFlashNode = weaponNode->createChildSceneNode();
	muzzleFlashEntity = sceneManager->createEntity("MuzzleFlashPlane2.mesh");
	if(local)
		muzzleFlashEntity->setRenderQueueGroup(RENDER_QUEUE_7);
	else
		muzzleFlashEntity->setRenderQueueGroup(RENDER_QUEUE_6);
    muzzleFlashEntity->setCastShadows(false);
	changeColor(team);
    //ent->setMaterialName("_muzzleflash2");
    muzzleFlashNode->attachObject(muzzleFlashEntity);
    muzzleFlashNode->rotate(Ogre::Vector3::UNIT_X, Degree(90));
    muzzleFlashNode->rotate(Ogre::Vector3::UNIT_Y, Degree(180));
    muzzleFlashNode->setPosition(4.6, .7, 0);
    muzzleFlashNode->setInheritScale(false);
    muzzleFlashNode->scale(8, 8, 8);
    muzzleFlashNode->setVisible(true);

    std::string fireSound=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsString("GameSounds","lp-fire");
    SoundController::getSingleton()->attachDynamicSound(weaponNode,fireSoundID,fireSound,false);
}

Bullet* LaserPistol::createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Quaternion orientation)
{
    if(playerNode!=NULL)
    {
        Ogre::Vector3 pos=playerNode->getOrientation()*bulletOrigin+playerNode->getPosition();
        return createBullet(sceneManager,dynamics,pos,orientation);
    }
	return NULL;
}

void LaserPistol::changeColor(unsigned char color)
{
    if (color==0)
    {
		muzzleFlashEntity->getSubEntity(0)->setMaterialName("MuzzleFlash2Side_Blue");
        muzzleFlashEntity->getSubEntity(1)->setMaterialName("MuzzleFlash2Front_Blue");
		weaponLight->setDiffuseColour(0,0,1);
    }
    
    else
    {
        muzzleFlashEntity->getSubEntity(0)->setMaterialName("MuzzleFlash2Side_Red");
        muzzleFlashEntity->getSubEntity(1)->setMaterialName("MuzzleFlash2Front_Red");
		weaponLight->setDiffuseColour(1,0,0);
    }
	this->color=color;
}