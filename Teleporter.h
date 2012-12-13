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

#ifndef TELEPORTER_H
#define	TELEPORTER_H
#include "stdafx.h"
#include "Util/Converter.h"
#include "PhysicsShapeHandler.h"

/*	Teleporter

	This class is used for handling the attributes
	and the graphics of the teleporters that are
	used on the application.

*/

class Teleporter
{
public:
	//constructor of the class. gets the id and  input&output teleport position 
    Teleporter(int id,Ogre::Vector3 inPosition,Ogre::Vector3 outPosition);
	//creates the visual parts of the teleporter
    void createVisual(SceneManager* sceneManager);
	//creates the physics on the teleporter objects
    void createPhysics(OgreBulletDynamics::DynamicsWorld* dynamics);
	//updates the graphics. usually called at the each frame
    void updateGraphics();
	//sets the status of the teleporter object
    void setEnabled(bool value);
	//returns true if the teleporter has been activated
    bool isActivated();
	//returns the id of the teleporter
    int getID();
	//deactivates the teleporter; hides the effects
    void deactivate();
	//activats the teleporter; shows the necessary effects
    void activate();
	//returns the rigidbody of the teleporter entrance point
    OgreBulletDynamics::RigidBody* getTeleportPointRigid();
	//returns the out position of the teleport
    Ogre::Vector3 getOutPosition();
    
private:
    int id;
    Ogre::Vector3 inTelPos;
    Ogre::Vector3 outTelPos;
    SceneNode* inTelNode;
    SceneNode* teleportPointNode;
    SceneNode* outTelNode;
    Entity* inTelEnt;
    Entity* outTelEnt;
    OgreBulletDynamics::RigidBody* inTelRigid;
    OgreBulletDynamics::RigidBody* outTelRigid;
    OgreBulletDynamics::RigidBody* teleportPointRigid;
    BillboardSet* inTelBillboardSet;
    Ogre::Light* bbLight;
    bool activated;
    std::string inTelSoundID;
    std::string outTelSoundID;
	ParticleSystem* inParticleSystem;
	Ogre::SceneNode* inParticleNode;
	ParticleSystem* outParticleSystem;
	Ogre::SceneNode* outParticleNode;

};

#endif	/* TELEPORTER_H */

