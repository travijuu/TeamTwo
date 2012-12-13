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

#ifndef TELEPORTERCALLBACKINTERFACE_H
#define	TELEPORTERCALLBACKINTERFACE_H

/*	TeleportercallbackInterface

	This interface is implemented by the 
	subscriber class that needs to know
	when the teleporter has been reactivated.

*/

class TeleporterCallbackInterface
{
public:
	//called when the teleporter with the given Id has been reactivated
    virtual void teleporterActivatedEvent(int teleporterID)=0;
};



#endif	/* TELEPORTERCALLBACKINTERFACE_H */

