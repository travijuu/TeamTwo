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

#ifndef WEAPON_H
#define	WEAPON_H

#include "stdafx.h"
#include "Bullet.h"
#include "AnimationController.h"
#include "ItemStructure.h"
#include "Util/Logger.h"

namespace WeaponSlot
{
    enum WeaponSlots
    {
        Primary=0,
        Secondary,
        Tertiary,
        Quaternary,
        Quinary,
        None
    };
}

/*	Weapon class

	This class is responsible for providing
	common attributes of a weapon such as
	its bullet speed, muzzle flashes and etc.
	Weapon cannot be used by itself, instead
	it has to be inherited by another class
	in order to utilize from it. 
*/

class Weapon
{
public:
	//constructor of the class. requires the owner id of the weapon
	//weapon's type and team
    Weapon(int ownerID,int weaponType,unsigned char team);
	//destructor of the class.
    ~Weapon()
    {
        Logger::getSingleton()->addLine("Weapon: deleting weapon");
        delete fireTimer;
        delete reloadTimer;
        Logger::getSingleton()->addLine("Weapon: weapon deleted");
    }
    //returns muzzleflash node
    SceneNode* getMuzzleFlashNode();
	//updates the weapon's graphics, reload timers etc.
    void update();
	//starts a timer to reload the weapon
    void reload();
	//fires the weapon, starts a timer to calculate the next fire
    void fire();
	//returns the ammo count on the current clip
    int getClipAmmo();
	//returns the number of clips the weapon has
    int getClipCount();
	//returns the ammo capacity of the weapon's clip
    int getClipSize();
	//returns the weaponslot which the weapon object belongs
    int getWeaponSlot();
	//returns the maximum ammo that the weapon can have
    int getMaxClip();
	//returns the type of the weapon
    int getWeaponType();
	//returns the total ammo that the weapon has
    int getTotalAmmo();
	//returns true if the weapon can fire, considers the previous reload and fire actions
    bool canFire();
	//returns true if weapon can be reloaded. cosiders the previous reload action
    bool canReload();
	//returns true if the weapon can be added extra clips
    bool canTakeClips();
	//adds the given number of clips to the weapon 
    void addClip(int count);
	//enables ammunition check. in this case fire() and canFire() will return variying results
	//enabled by default
    void enableAmmunitionCheck();
	//disables ammunition check for the weapon. weapon will always be able to fire without 
	//checking the ammunition
    void disableAmmunitionCheck();
	//shows the weapon and its sub objects
    void show();
	//hides the weapon and its sub objects
    void hide();
	//sets the number of ammo that the clips has
    void setClipAmmo(int ammo);
	//returns the AnimationController object for the weapon
    AnimationController* getAnimationController();
	//retunrs the weapon' node
    Ogre::SceneNode* getWeaponNode();
	//returns the weapon' entity
	Ogre::Entity* getWeaponEntity();
    //returns the weapon's light object
	Ogre::Light* getWeaponLight();
	//sets a SceneNode for the weapon
    void setSceneNode(Ogre::SceneNode* weaponNode);
	//returns the bullet origin of the weapon
    Ogre::Vector3 getBulletOrigin();
	//zeros the clip and ammo count of the weapon. useful for reinitializing
    void zero();
	//creates bullet
    virtual Bullet* createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Vector3 position, Ogre::Quaternion orientation)=0;
	//creates bullet
    virtual Bullet* createBullet(SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics, Ogre::Quaternion orientation){return NULL;}
	//creates visual of the weapon
    virtual void createVisual(SceneManager* sceneManager, SceneNode* playerNode)=0;
	//changes the necessary colors of the weapon
	virtual void changeColor(unsigned char color)=0;
private:
	//initializes the weapon. must be implemented by the derived class
    virtual void initialize()=0;
    bool justFired;
    int visualizationEndFrame;
    int visualizationCurrentFrame;
    bool fireVisualized;
protected:
    std::string name;
    int ownerID;
	unsigned char team;
    int totalAmmo;
    int clipAmmo;
    int clipCount;
    int clipSize;
    int maxClip;
    int roundsPerSecond;
    int weaponSlot;
    int weaponType;
    long nextFireTime;
    long reloadDuration;
    long reloadEndTime;
    bool reloading;
    bool fired;
    bool check;
	bool local;

    Entity* bulletEntity;
    AnimationController* animationController;
    Ogre::Vector3 bulletOrigin;
    Ogre::Timer* fireTimer;
    Ogre::Timer* reloadTimer;
    SceneNode* playerNode;
    SceneNode* weaponNode;
    SceneNode* muzzleFlashNode;
	Entity* weaponEntity;
	Entity* muzzleFlashEntity;
    Light* weaponLight;
	unsigned char color;
    std::string fireSoundID;
    std::vector<std::string> reloadSoundIDs;
};

#endif	/* WEAPON_H */


