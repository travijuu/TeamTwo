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

#include "Teleporter.h"

Teleporter::Teleporter(int id, TeleporterCallbackInterface* callbackObject)
{
    this->id=id;
    this->callbackObject=callbackObject;
    this->activated=true;
    expirableItem=new ExpirableItem(-1,-1,this);
}

bool Teleporter::isActivated()
{
    return activated;
}

void Teleporter::setActivated(bool activated)
{
    this->activated=activated;
}

void Teleporter::deactivate(int timeoutMilliseconds)
{
    std::cout<<"teleporter deactivated\n";
    activated=false;
    expirableItem->startExpirationTimer(timeoutMilliseconds);
}

void Teleporter::itemExpiredEvent(unsigned int playerID, int itemType)
{
    std::cout<<"teleporterd reactivated\n";
    activated=true;
    callbackObject->teleporterActivatedEvent(this->id);
}

void Teleporter::stopActivationTimer()
{
    expirableItem->stopExpirationTimer();
}
