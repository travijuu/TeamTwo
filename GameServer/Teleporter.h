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
#include "ExpirableItem.h"
#include "TeleporterCallbackInterface.h"

/*	Teleporter

	This class is responsible for holding information
	about the teleporters that are read from the map
	file and manage its activation/deactivation events.
	It inherits the ExpirableItemCallbackInterface for
	provide activation events.
*/

class Teleporter:ExpirableItemCallbackInterface
{
public:
	//constructor. takes the teleporter id and a callback object
    Teleporter(int id,TeleporterCallbackInterface* callbackObject);
	//returns true if the teleporter is active   
	bool isActivated();
	//ExpirableItemCallbackInterface method
    void itemExpiredEvent(unsigned int playerID,int itemType);
	//deactivates the teleporter with the given timeout milliseconds
    void deactivate(int timeoutMilliseconds);
	//sets the status of the teleporter
    void setActivated(bool activated);
	//stops the activation timer immediately
    void stopActivationTimer();
private:
    int id;
    bool activated;
    ExpirableItem* expirableItem;
    TeleporterCallbackInterface* callbackObject;

};

#endif	/* TELEPORTER_H */

