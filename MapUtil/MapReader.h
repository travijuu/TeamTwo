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

#include "MapUtil/MapStructure.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string.h>

/* Map Reader
 *
 * This class reads a map file that is
 * encoded in TTM file format which is
 * a custom format that is designed to
 * be used in this game project. It's
 * able to read walls, items, spawn points
 * that are defined in the file and
 * provides them to an intermediary
 * class such as MazeBuilder.
 */

class MapReader
{
public:
    //constuctor. gets a filename that is going to be read
    MapReader(std::string filename);
    ~MapReader();
    //tries to read the map file. if successful, returns "true"
    bool readMap();
    //returns the next wall entity
    Entity4Position_t* getWallEntity();
    //returns the next teleport entity
    Entity4Position_t* getTeleportEntity();
    //returns the next light entity
    Entity2Position_t* getLightEntity();
    //returns the next spawn point
    Entity2Position_t* getSpawnPoint();
    //returns the next game item
    MapEntity2_t* getItem();
    //this method finds out whether the given file is encoded in TTM format or not
    static bool isTTM(std::string fileName);
    //returns the scale factor for the current map
    float getScaleFactor();
    //returns the next model entity
    ModelEntity_t* getModelEntity();
private:
    float scaleFactor;

    std::string filename;
    std::ifstream is;
    std::vector<Entity4Position_t> wallVector;
    std::vector<Entity4Position_t> teleportVector;

    std::vector<Entity2Position_t> lightVector;
    std::vector<Entity2Position_t> spawnPointVector;
    std::vector<ModelEntity_t> modelEntityVector;

    std::vector<MapEntity2_t> itemVector;


    std::vector<Entity4Position_t>::iterator wallIterator;
    std::vector<Entity4Position_t>::iterator teleportIterator;
    std::vector<Entity2Position_t>::iterator lightIterator;
    std::vector<Entity2Position_t>::iterator spawnPointIterator;
    std::vector<ModelEntity_t>::iterator modelEntityIterator;
    std::vector<MapEntity2>::iterator itemIterator;
};