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

#include "Weapon.h"
#include "Util/Logger.h"
#include "Util/Converter.h"
#include "SoundController.h"

Weapon::Weapon(int ownerID,int weaponType,unsigned char team)
{
    this->team = team;
    this->ownerID=ownerID;
    this->check=true;
    this->fireTimer=new Ogre::Timer();
    this->reloadTimer=new Ogre::Timer();
    this->nextFireTime=this->reloadEndTime=-1;
    this->reloading=false;
    this->fired=false;
    this->justFired=false;
    this->fireVisualized=false;
    this->weaponType=weaponType;
    this->weaponLight=NULL;
    this->weaponNode=NULL;
    this->visualizationCurrentFrame=0;
    this->visualizationEndFrame=4;
    this->fireSoundID="SOUND_NOT_SET";
	this->local=false;
}

void Weapon::show()
{
    if(weaponNode!=NULL)
        weaponNode->setVisible(true);
    if(weaponLight!=NULL)
        weaponLight->setVisible(false);
    if(muzzleFlashNode!=NULL)
        muzzleFlashNode->setVisible(false);
}

void Weapon::hide()
{
    if(weaponNode!=NULL)
        weaponNode->setVisible(false);
    if(weaponLight!=NULL)
        weaponLight->setVisible(false);
    if(muzzleFlashNode!=NULL)
        muzzleFlashNode->setVisible(false);
}

void Weapon::zero()
{
    clipAmmo=0;
    clipCount=0;
    totalAmmo=0;
}

void Weapon::update()
{
    if(reloading && reloadEndTime<=reloadTimer->getMilliseconds())
    {
        clipAmmo=(totalAmmo>=clipSize?clipSize:totalAmmo);
        clipCount=(int)Ogre::Math::Ceil(totalAmmo/(float)clipSize);
        reloadEndTime=-1;
        reloadTimer->reset();
        Logger::getSingleton()->addLine("reloaded.");
        reloading=false;
    }
    if(fired && nextFireTime<=fireTimer->getMilliseconds())
    {
        fired=false;
        nextFireTime=-1;
        fireTimer->reset();
    }

    if(justFired)
    {
        if(visualizationCurrentFrame>visualizationEndFrame)
        {
            justFired=false;
            weaponLight->setVisible(false);
            muzzleFlashNode->setVisible(false);
        }
        else
            visualizationCurrentFrame++;
    }
}

void Weapon::fire()
{
    if(check && canFire())
    {
        nextFireTime=fireTimer->getMilliseconds()+(1000./roundsPerSecond);
        clipAmmo--;
        totalAmmo--;
        fired=true;
        //animations...
    }
    else if(!check)
        fired=true;


    if (fired)
    {
        SoundController::getSingleton()->playSound(fireSoundID,true);
        if(weaponLight!=NULL)
        {
            weaponLight->setPosition(bulletOrigin);
            weaponLight->setVisible(true);
            muzzleFlashNode->setVisible(true);
            justFired = fireVisualized = true;
            visualizationCurrentFrame = 0;
        }
    }
    
    
}

void Weapon::reload()
{
    if(check && (canReload()))
    {
        Logger::getSingleton()->addLine("reloading...");
        reloadEndTime=reloadTimer->getMilliseconds()+reloadDuration;
        reloading=true;
        if(reloadSoundIDs.size()>0)
        {
            srand(time(NULL));
            int reloadIndex=rand()%reloadSoundIDs.size();
            SoundController::getSingleton()->playSound(reloadSoundIDs[reloadIndex],true);
        }
        //animations..
    }
}

void Weapon::addClip(int clips)
{
    int _total = (totalAmmo + (getClipSize() * clips));
    _total=(_total>getMaxClip()*getClipSize()?getMaxClip()*getClipSize():_total);
    clipCount = (int) Ogre::Math::Ceil(_total / (float) clipSize);
    totalAmmo = _total;
}

bool Weapon::canTakeClips()
{
    int clipDiff=getClipSize()-getClipAmmo();
    bool extra=(clipDiff+totalAmmo==getMaxClip()*getClipSize()?true:false);
    return !extra && totalAmmo<getClipSize()*getMaxClip()?true:false;
}

bool Weapon::canFire()
{
    return !fired && (getClipAmmo()>0) && !reloading;
}

bool Weapon::canReload()
{
    return getClipAmmo()<getClipSize() && !reloading && getClipCount()>0;
}

void Weapon::setSceneNode(Ogre::SceneNode* weaponNode)
{
    this->weaponNode=weaponNode;
}

Ogre::SceneNode* Weapon::getWeaponNode()
{
    return weaponNode;
}

Ogre::Light* Weapon::getWeaponLight()
{
    return weaponLight;
}

int Weapon::getTotalAmmo()
{
    return totalAmmo;
}


int Weapon::getMaxClip()
{
    return maxClip;
}

int Weapon::getClipAmmo()
{
    return clipAmmo;
}

int Weapon::getClipCount()
{
    return clipCount;
}

int Weapon::getClipSize()
{
    return clipSize;
}

int Weapon::getWeaponSlot()
{
    return weaponSlot;
}

AnimationController* Weapon::getAnimationController()
{
    return animationController;
}

Ogre::Vector3 Weapon::getBulletOrigin()
{
    return bulletOrigin;
}

void Weapon::disableAmmunitionCheck()
{
    check=false;
}

void Weapon::enableAmmunitionCheck()
{
    check=true;
}

int Weapon::getWeaponType()
{
    return weaponType;
}

SceneNode* Weapon::getMuzzleFlashNode()
{
    return muzzleFlashNode;
}

void Weapon::setClipAmmo(int ammo)
{
    clipAmmo=ammo;
}

Ogre::Entity* Weapon::getWeaponEntity()
{
	return weaponEntity;
}
