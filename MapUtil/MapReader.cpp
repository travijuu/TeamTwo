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

#include "MapReader.h"
#include "Util/Logger.h"


MapReader::MapReader(std::string filename)
{
    this->filename=filename;
}

MapReader::~MapReader()
{
    itemVector.clear();
    wallVector.clear();
    teleportVector.clear();
    spawnPointVector.clear();
    lightVector.clear();
}

bool MapReader::readMap()
{
    bool result=false;
    if(isTTM(filename))
    {
        MapEntity2_t ent2;
        MapEntity4_t ent4;
        MapHeader header;
        ModelEntity_t modelEnt;
		is.open(filename.c_str(),std::ios_base::binary|std::ios_base::in);
        if(is.is_open() && !is.eof())
        {
            is.read((char*)&header,sizeof(MapHeader));
            result=true;
            scaleFactor = header.scaleFactor;
            int total=sizeof(MapHeader);
			for (int i = 0; i < header.entity4Count && !is.eof(); i++)
            {
				
				is.read((char*)(&ent4),sizeof(MapEntity4_t));
				total+=sizeof(MapEntity4_t);
                switch(ent4.type)
                {
                    case MapEntityType::MAP_WALL:
                    {
                        wallVector.push_back(ent4.entityPosition);
                    }break;
                    case MapEntityType::MAP_TELEPORT:
                    {
                        teleportVector.push_back(ent4.entityPosition);
                    }break;
                }
            }

            for (int i = 0; i < header.entity2Count && !is.eof(); i++)
            {
                is.read((char*)(&ent2),sizeof(MapEntity2_t));
                switch(ent2.type)
                {
                    case MapEntityType::LIGHT_POINT:
                    {
                        lightVector.push_back(ent2.entityPosition);
                        break;
                    }
                    case MapEntityType::SPAWN_POINT:
                    {
                        spawnPointVector.push_back(ent2.entityPosition);
                        break;
                    }
                    default:
                    {
                        itemVector.push_back(ent2);
                    }
                }
            }

            for(int i=0;i<header.modelCount && !is.eof();i++)
			{
                is.read((char*)(&modelEnt),sizeof(ModelEntity_t));
                if(modelEnt.type==MapEntityType::MODEL)
                    modelEntityVector.push_back(modelEnt);
            }
        }

        wallIterator = wallVector.begin();
        teleportIterator = teleportVector.begin();
        lightIterator = lightVector.begin();
        spawnPointIterator = spawnPointVector.begin();
        itemIterator = itemVector.begin();
        modelEntityIterator=modelEntityVector.begin();
    }
    return result;
}

bool MapReader::isTTM(std::string fileName)
{
    bool result=false;
    std::ifstream fileStream;
    fileStream.open(fileName.c_str());
    if(fileStream.is_open())
    {
        MapHeader_t header;
        fileStream.read((char*)&header,sizeof(MapHeader_t));
        if(std::string(header.name)==std::string(HEADER_NAME) && header.fileSignature==FILE_SIGNATURE)
            result=true;
        fileStream.close();
    }
    return result;
}

Entity4Position_t* MapReader::getWallEntity()
{
    Entity4Position_t* entityPosition=NULL;
    if(wallIterator!=wallVector.end())
        entityPosition=&(*(wallIterator++));
    return entityPosition;
}


Entity4Position_t* MapReader::getTeleportEntity()
{
    Entity4Position_t* entityPosition=NULL;
    if (teleportIterator != teleportVector.end())
        entityPosition = &(*(teleportIterator++));
    return entityPosition;
}

Entity2Position_t* MapReader::getLightEntity()
{
    Entity2Position_t* entityPosition=NULL;
    if (lightIterator != lightVector.end())
        entityPosition = &(*(lightIterator++));
    return entityPosition;
}

Entity2Position_t* MapReader::getSpawnPoint()
{
    Entity2Position_t* entityPosition=NULL;
	if(spawnPointIterator!=spawnPointVector.end())
        entityPosition=&(*(spawnPointIterator++));
    return entityPosition;
}

ModelEntity_t* MapReader::getModelEntity()
{
    ModelEntity_t* entity=NULL;
    if(modelEntityIterator!=modelEntityVector.end())
        entity=&(*(modelEntityIterator++));
    return entity;
}

MapEntity2_t* MapReader::getItem()
{
    MapEntity2* entity=NULL;
    if(itemIterator!=itemVector.end())
        entity=&(*(itemIterator++));
    return entity;
}

float MapReader::getScaleFactor()
{
	return scaleFactor;
}