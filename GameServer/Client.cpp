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

#include "Client.h"

Client::Client(std::string name,int id,std::string ipAddress,int socket)
{
    this->name=name;
    this->id=id;
    this->lobbySocket=socket;
    this->ipAddress=ipAddress;
    this->ready=false;
    this->team=0;
    this->joined=false;
    this->spawnTimer=new CallbackTimer(this);
    this->hadMapDownloaded=false;
    this->deaths=this->kills=0;
}

void Client::startSpawnTimer(int milliseconds,ClientCallbackInterface* callbackObject)
{
    this->callbackObject=callbackObject;
    spawnTimer->start(milliseconds,false);
}

void Client::stopSpawnTimer()
{
    spawnTimer->stop();
}

void Client::timeElapsedEvent()
{
    callbackObject->clientResurrectedEvent(getId());
}

int Client::getId()
{
    return id;
}

int Client::getLobbySocket()
{
    return lobbySocket;
}

void Client::setName(std::string name)
{
    this->name=name;
}

std::string Client::getName()
{
    return name;
}

bool Client::isPlaying()
{
    return playing;
}

bool Client::isJoined()
{
    return joined;
}

void Client::setJoined(bool joined)
{
    this->joined=joined;
}

void Client::setPlaying(bool playing)
{
    this->playing=playing;
}

int Client::getGameSocket()
{
    return gameSocket;
}

void Client::setColor(unsigned char color)
{
	this->color=color;
}

unsigned char Client::getColor()
{
	return this->color;
}

void Client::setGameSocket(int socket)
{
    gameSocket=socket;
}

std::string Client::getIp()
{
    return ipAddress;
}

void Client::setReady(bool ready)
{
    this->ready=ready;
}

bool Client::isReady()
{
    return ready;
}

void Client::setTeam(unsigned char team)
{
    this->team=team;
}

unsigned char Client::getTeam()
{
    return team;
}

bool Client::hasMap()
{
    return hadMapDownloaded;
}

void Client::setHasMap(bool value)
{
    this->hadMapDownloaded=value;
}

void Client::increaseDeathCount()
{
    deaths++;
}

void Client::increaseKillCount()
{
    kills++;
}

int Client::getKillCount()
{
    return kills;
}

int Client::getDeathCount()
{
    return deaths;
}

void Client::zeroRatio()
{
    kills=deaths=0;
}