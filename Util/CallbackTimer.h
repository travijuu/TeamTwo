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

#ifndef CALLBACKTIMER_H
#define CALLBACKTIMER_H

#ifdef WINDOWS_PLATFORM
	#include <windows.h>
#else
	#include <time.h>
#endif

#include <boost/thread.hpp>
#include "CallbackTimerInterface.h"

/* Callback Timer
 *
 * This class provides a callback based
 * timer interface in order to be notified
 * after the specified time has passed.
 * It is possible to use it for continious
 * or one-time-run. Utilizes from boost::thread
 * for supporting non-blocking and operating
 * system dependent structure.
 */


class CallbackTimer
{
public:
    //constructor of the class. takes a CallbackTimerInterface instance
    //in order to perform the callback ability
    CallbackTimer(CallbackTimerInterface* callbackObject);
    //starts the timer that will be elapsed according to the given milliseconds
    //by default, the timer restarts itself if the speficied time has elapsed
    void start(int milliseconds,bool repeat=true);
    //stops the timer if it is running
    void stop();

private:
    //this method is invoked by start(). this is actually the
    //thread version of the start() method.
    void _start();

    bool repeating;
    bool enabled;
    int milliseconds;
    boost::thread* timerThread;
    CallbackTimerInterface* callbackObject;
};

#endif
