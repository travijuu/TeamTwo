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

#ifndef ITEM_H
#define	ITEM_H
#include "ItemCallbackInterface.h"
#include <CallbackTimer.h>

/*	Item class

	This class is responsible for holding 
	information about an item that was read
	from the Map file. These items can be both
	static and dynamic.They contain a timer for 
	reactivation. In the event of activation, 
	it uses its callback interface ItemCallbackInterface
	to notify the subscriber about the event.
*/

class Item:public CallbackTimerInterface
{
public:
	//constructor. takes item's id, its reactivation time, type and an ItemCallbackInterface object
    Item(int id,int waitTime,ItemCallbackInterface* callbackObject,int itemType,bool temporary=false);
    //returns the id of the item
	int getID();
	//returns true if the item is currently active
    bool isAlive();
	//deactivates the item and starts the reactivation timer
    void die();
	//stops reactivation timer
    void stopSpawnTimer();
	//CallbackTimer's event method
    void timeElapsedEvent();
	//returns the type of the item
    int getItemType();
	//returns true if the item is temporary
	bool isTemporary();
private:
    ItemCallbackInterface* callbackObject;
    int waitingTime;
    CallbackTimer* callbackTimer;
    int id;
    bool alive;
    float x;
    float y;
    int itemType;
    bool temporary;
};

#endif	/* ITEM_H */

