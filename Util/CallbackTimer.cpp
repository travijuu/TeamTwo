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

#include "CallbackTimer.h"


CallbackTimer::CallbackTimer(CallbackTimerInterface* callbackObject)
{
    this->callbackObject=callbackObject;
    this->enabled=false;
    this->repeating=true;
    this->timerThread=NULL;
}

void CallbackTimer::start(int milliseconds,bool repeating)
{
    if(!enabled)
    {
        this->milliseconds=milliseconds;
        this->repeating=repeating;
        enabled=true;
        timerThread=new boost::thread(&CallbackTimer::_start,this);
    }
}

void CallbackTimer::stop()
{
    if(enabled)
    {
        enabled=false;
        if(boost::this_thread::get_id()!=timerThread->get_id())
            timerThread->join();
    }
}

void CallbackTimer::_start()
{
#ifdef WINDOWS_PLATFORM

	LARGE_INTEGER _current,_millisec;
	QueryPerformanceFrequency(&_millisec);
	QueryPerformanceCounter(&_current);
	long double oneMillisec=_millisec.QuadPart/1000000.;
	long double target;
	int i=0;
	do
	{
		target=_current.QuadPart+(milliseconds*1000.*(oneMillisec));
		while(enabled && _current.QuadPart<=target)
		{
			Sleep(50);
			QueryPerformanceCounter(&_current);
		}

		if (enabled)
            callbackObject->timeElapsedEvent();
        else
            break;
	}while(repeating);
	
#else
    long targetMicrosec = milliseconds * 1000;
    long totalPassedMicrosec;
    long previousMicrosec;
    long diff;
    int passed;
    struct timezone timeZone;
    struct timeval timeVal;
    struct tm* _time;
    gettimeofday(&timeVal, &timeZone);
    _time = localtime(&(timeVal.tv_sec));

    do
	{
        totalPassedMicrosec=0;
        previousMicrosec=timeVal.tv_usec;
        diff=0;
        passed=0;
        while(enabled && totalPassedMicrosec<=targetMicrosec)
        {
			usleep(100000);
            gettimeofday(&timeVal,&timeZone);
            _time=localtime(&(timeVal.tv_sec));
            diff=timeVal.tv_usec-previousMicrosec;
            passed=(diff<0?1000000-previousMicrosec+timeVal.tv_usec:diff);            
            totalPassedMicrosec+=passed;
            previousMicrosec=timeVal.tv_usec;
        }

        if (enabled)
            callbackObject->timeElapsedEvent();
        else
            break;

    }while(repeating);
#endif
    enabled = false;
}
