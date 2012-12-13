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

#ifndef CALLBACKTIMERINTERFACE_H
#define CALLBACKTIMERINTERFACE_H

/* Callback Timer Interface
 *
 * This abstract class is used by
 * Callback Timer in order to provide
 * a predefined callback mechanism that
 * is going to be used for notifying the
 * subscribed class that the given
 * time has been elapsed.
 */

class CallbackTimerInterface
{
public:
    //called by the Callback Timer if the time has been elapsed
    virtual void timeElapsedEvent()=0;
};

#endif
