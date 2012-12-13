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

#ifndef EXPIRABLEITEMCALLBACKINTERFACE_H
#define EXPIRABLEITEMCALLBACKINTERFACE_H

/*	ExpirableItemCallbackInterface

	This interface is used to notify the
	subscriber that the item has been expired

*/

class ExpirableItemCallbackInterface
{
public:
	//called when the item has been expired. passes the playerid
	//who had been used it and the item identifier
    virtual void itemExpiredEvent(unsigned int playerID,int itemType)=0;
};


#endif