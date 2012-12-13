/*
	Copyright (C) 2011-2012 Erkin Cakar

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

#ifndef _MAPSTRUCTURE_H
#define	_MAPSTRUCTURE_H

/* MapStructure

   This structure collection is used for storing
   and reading maps from a file. it also contains
   the file signature and header name of the TTM
   map file format.

*/

#define HEADER_NAME "TTM"
#define FILE_SIGNATURE 654321

namespace MapEntityType
{
    enum MapEntityTypes
    {
        MAP_WALL=100, HIDDEN_GATE_WALL, MAP_TELEPORT, SPAWN_POINT, LIGHT_POINT, MODEL = 500
    };
}

typedef struct Entity4Position
{
    float startX, startZ, endX, endZ;
}Entity4Position_t;

typedef struct Entitiy2Position
{
    float startX, startZ;
}Entity2Position_t;

typedef struct MapEntity4
{
    int type;
    Entity4Position_t entityPosition;
}MapEntity4_t;

typedef struct MapEntity2
{
    int type;
    Entity2Position_t entityPosition;
    float direction;
}MapEntity2_t;

typedef struct ModelEntity
{
    int type;
    char modelName[16];
    Entity2Position_t entityPosition;
    float direction;

}ModelEntity_t;

typedef struct MapHeader
{
    char name[4];
    int fileSignature;
    int entity4Count;
    int entity2Count;
    int modelCount;
    int scaleFactor;
}MapHeader_t;

#endif	/* _MAPSTRUCTURE_H */





/* 
 * File:   MapStructure.h
 * Author: travego
 *
 * Created on October 22, 2011, 6:42 PM
 */

#ifndef _MAPSTRUCTURE_H
#define	_MAPSTRUCTURE_H

#include <iostream>

#define HEADER_NAME "TTM"
#define FILE_SIGNATURE 654321

namespace MapEntityType
{
    enum MapEntityTypes
    {
        MAP_WALL=100, HIDDEN_GATE_WALL, MAP_TELEPORT, SPAWN_POINT, LIGHT_POINT
    };
}
namespace MapWeaponType
{
    enum MapWeaponTypes
    {
        LASER=100, PISTOL
    };
}
namespace MapExtrasType
{
    enum MapExtrasTypes
    {
        IMMORTALITY=200, INVISIBILITY, EXTRA_HEALTH, AMMO
    };
}
typedef struct Entity4Position
{
    float startX, startZ, endX, endZ;
}Entity4Position_t;

typedef struct Entitiy2Position
{
    float startX, startZ;
}Entity2Position_t;

typedef struct MapEntity4
{
    int type;
    Entity4Position_t entityPosition;
}MapEntity4_t;

typedef struct MapEntity2
{
    int type;
    Entity2Position_t entityPosition;
    float direction;
}MapEntity2_t;

typedef struct MapHeader
{
    char name[4];
    int fileSignature;
    int entity4Count;
    int entity2Count;
    int scaleFactor;
}MapHeader_t;

typedef struct MapMenuItem
{
    std::string name;
    std::string menuName;
    int type;
    Entity4Position_t position;
}MapMenuItem_t;


#endif	/* _MAPSTRUCTURE_H */



