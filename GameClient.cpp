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

#include "GameClient.h"

GameClient::GameClient(unsigned int clientID, std::string clientName,unsigned char team)
{
    id=clientID;
    name=clientName;
    ready=false;
	this->kills=this->deaths=this->color=0;
}

void GameClient::setName(std::string name)
{
    this->name=name;
}

std::string GameClient::getName()
{
    return name;
}

unsigned int GameClient::getID()
{
    return id;
}

unsigned char GameClient::getTeam()
{
    return team;
}

void GameClient::setTeam(unsigned char team)
{
    this->team=team;
}

void GameClient::setReady(bool ready)
{
    this->ready=ready;
}

bool GameClient::isReady()
{
    return ready;
}

void GameClient::setDeathCount(int deathCount)
{
    deaths=deathCount;
}

void GameClient::setKillCount(int killCount)
{
    kills=killCount;
}

int GameClient::getKillCount()
{
    return kills;
}

int GameClient::getDeathCount()
{
    return deaths;
}

void GameClient::setColor(unsigned char color)
{
	this->color=color;
}

unsigned char GameClient::getColor()
{
	return color;
}
