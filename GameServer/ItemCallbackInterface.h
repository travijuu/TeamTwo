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

#ifndef ITEMCALLBACKINTERFACE_H
#define	ITEMCALLBACKINTERFACE_H

/*	ItemCallbackInterface

	This class is used by the Item's
	subscriber. In the event of item
	reactivation, its methods are called 
	to notify the event subscriber.
*/

class ItemCallbackInterface
{
public:
	//called whenever the item with the given ID has been reactivated
    virtual void itemResurrected(int id)=0;
};



#endif	/* ITEMCALLBACKINTERFACE_H */

