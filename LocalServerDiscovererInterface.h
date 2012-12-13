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

#ifndef LOCALSERVERDISCOVERERINTERFACE_H
#define	LOCALSERVERDISCOVERERINTERFACE_H

/*	LocalServerDiscovererInterface

	This interface is used for notifying
	the subscriber class about the events.

*/

class LocalServerDiscovererInterface
{
public:
	//called when the started discovery has finished (timeout)
	//it passes a map of server information. if no servers found,
	//map size will be zero
    virtual void discoveryFinalizedEvent(std::map<std::string,std::string> serverList)=0;
};



#endif	/* LOCALSERVERDISCOVERERINTERFACE_H */

