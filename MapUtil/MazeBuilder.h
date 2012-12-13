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

#ifndef MAZEBUILDER_H
#define	MAZEBUILDER_H

#include "stdafx.h"
#include "MapUtil/MapReader.h"
#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "Util/Converter.h"
#include "PhysicsShapeHandler.h"


/* MazeBuilder
 *
 * This class is responible for creating a level
 * built by the developers or the users of the product
 * with the file format of TTM (TeamTwo Map). MazeBuilder
 * reads the given file using MapReader and creates
 * walls, lights and objects that are defined in the
 * map file
 */


class MazeBuilder
{
public:
    //constructor of the MazeBuilder class which takes filename to read the file and physics and ogre3d
    //objects for creating visuality and implementing physics to the objects
    MazeBuilder(std::string filename,Ogre::SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics);
    //destructor of the MazeBuilder. deletes used objects
    ~MazeBuilder();
    //reads the given map file in the constructor
    void readMap();
    //generates map considering the given values such as wall height and width etc. must be
    //called after the readMap is invoked
    void generateMap(float wallWidth,float wallHeight,float mapScaleFactor,bool enablePhysics=true);
    //generates lights
    void generateLights(bool lightsEnabled=true);
    //generates static and dynamics objects defined in the map file
    void generateObjects();
    //destroys light objects that are added before
    void destroyLights();
    //destroys wall objects with their physics 
    void destroyWalls();
    //destroy all of the objects (lights, walls etc); clears the scene from mazebuilder content
    void destroyAll();
    //returns map reader used by reading the map
    MapReader* getMapReader();
    //disables spot lights (used graphical purposes to gain performance)
    void disableSpotLights();
    //enables spot lights
    void enableSpotLights();
	//returns height of the walls
	float getWallHeight();
private:
    //creates wall by considering the given values
    Ogre::Entity* createWall(float xPos, float zPos, float horizontalRotation, Ogre::Vector3 size, bool enablePhysics);
    //creates an horizontal plane for the maze
    Ogre::Entity* createHorizontalPlane(std::string name,std::string material,Ogre::Vector3 position,float yRotation,float xLen,float zLen);
    //creates a point & spot light to the given position
    void createLight(float x, float y, float z, bool lightsEnabled);

    MapReader* mapReader;
	float wallHeight;
    std::string filename;
    Ogre::SceneManager* sceneManager;
    OgreBulletDynamics::DynamicsWorld* dynamics;
    std::vector<SceneNode*> nodeVector;
    std::vector<Light*> pointLightVector;
    std::vector<Light*> spotLightVector;
    std::vector<Entity*> entityVector;
    std::vector<OgreBulletDynamics::RigidBody*> rigidVector;

	//DEBUG
public:
	SceneNode* node;
};

#endif	/* MAZEBUILDER_H */

