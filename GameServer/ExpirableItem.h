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

#ifndef EXPIRABLEITEM_H
#define EXPIRABLEITEM_H
#include <iostream>
#include "ExpirableItemCallbackInterface.h"
#include "CallbackTimer.h"
#include "Item.h"

/*	ExpirableItem class

	This class handles the activation and
	deactivation events of an item object
	and holds the necessary information about
	the ownership and the items type. Utilizes
	from the CallbackTimerInterface to provide
	automatic expiration.

*/

class ExpirableItem:CallbackTimerInterface
{
public:
	//Constructor of the class. takes the player id, item's type and a callbackinterface object
    ExpirableItem(unsigned int playerID,int itemType,ExpirableItemCallbackInterface* callbackObject);
	//starts the expiration timer by the given amount of time
    void startExpirationTimer(int milliseconds);
	//stops the expiration timer immediately
    void stopExpirationTimer();
	//CallbackTimer's event method
    void timeElapsedEvent();
private:
    CallbackTimer* callbackTimer;
    unsigned int playerID;
    int itemType;
    ExpirableItemCallbackInterface* callbackObject;
};


#endif