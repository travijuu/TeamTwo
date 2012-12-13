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

#ifndef OPERATIONSCHEDULER_H
#define	OPERATIONSCHEDULER_H

/* OperationScheduler

   OperationScheduler is a collection of
   template classes which are utilized for
   a thread-safe message queuing system. in order
   to call methods from a certain thread,
   this collection can be used by sending the
   pointer of the method and its parameters.


*/

#include <iostream>
#include <vector>
#include <boost/thread/mutex.hpp>

class OperationBase
{
public:
    virtual void run()=0;
};


template<class CallbackObject>
class Operation:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)();
    CallbackMethod method;
    CallbackObject* object;
public:
    Operation(CallbackObject* object,CallbackMethod method)
    {
        this->object=object;
        this->method=method;
    }
    void run(){(object->*method)();}
};


template<class CallbackObject,typename P1>
class Operation1:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)(P1);
    CallbackMethod method;
    CallbackObject* object;
    P1 param1;
public:
    Operation1(CallbackObject* object,CallbackMethod method,P1 param1)
    {
        this->object=object;
        this->method=method;
        this->param1=param1;
    }
    void run(){(object->*method)(param1);}
};


template<class CallbackObject,typename P1,typename P2>
class Operation2:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)(P1,P2);
    CallbackMethod method;
    CallbackObject* object;
    P1 param1;
    P2 param2;
public:
    Operation2(CallbackObject* object,CallbackMethod method,P1 param1,P2 param2)
    {
        this->object=object;
        this->method=method;
        this->param1=param1;
        this->param2=param2;
    }
    void run(){(object->*method)(param1,param2);}
};

template<class CallbackObject,typename P1,typename P2,typename P3>
class Operation3:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)(P1,P2,P3);
    CallbackMethod method;
    CallbackObject* object;
    P1 param1;
    P2 param2;
    P3 param3;
public:
    Operation3(CallbackObject* object,CallbackMethod method,P1 param1,P2 param2,P3 param3)
    {
        this->object=object;
        this->method=method;
        this->param1=param1;
        this->param2=param2;
        this->param3=param3;
    }
    void run(){(object->*method)(param1,param2,param3);}
};

template<class CallbackObject,typename P1,typename P2,typename P3,typename P4>
class Operation4:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)(P1,P2,P3,P4);
    CallbackMethod method;
    CallbackObject* object;
    P1 param1;
    P2 param2;
    P3 param3;
    P4 param4;
public:
    Operation4(CallbackObject* object,CallbackMethod method,P1 param1,P2 param2,P3 param3,P4 param4)
    {
        this->object=object;
        this->method=method;
        this->param1=param1;
        this->param2=param2;
        this->param3=param3;
        this->param4=param4;
    }
    void run(){(object->*method)(param1,param2,param3,param4);}
};

template<class CallbackObject,typename P1,typename P2,typename P3,typename P4,typename P5>
class Operation5:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)(P1,P2,P3,P4,P5);
    CallbackMethod method;
    CallbackObject* object;
    P1 param1;
    P2 param2;
    P3 param3;
    P4 param4;
    P5 param5;
public:
    Operation5(CallbackObject* object,CallbackMethod method,P1 param1,P2 param2,P3 param3,P4 param4,P5 param5)
    {
        this->object=object;
        this->method=method;
        this->param1=param1;
        this->param2=param2;
        this->param3=param3;
        this->param4=param4;
        this->param5=param5;
    }
    void run(){(object->*method)(param1,param2,param3,param4,param5);}
};

template<class CallbackObject,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class Operation6:public OperationBase
{
    typedef void (CallbackObject::*CallbackMethod)(P1,P2,P3,P4,P5,P6);
    CallbackMethod method;
    CallbackObject* object;
    P1 param1;
    P2 param2;
    P3 param3;
    P4 param4;
    P5 param5;
    P6 param6;
public:
    Operation6(CallbackObject* object,CallbackMethod method,P1 param1,P2 param2,P3 param3,P4 param4,P5 param5,P6 param6)
    {
        this->object=object;
        this->method=method;
        this->param1=param1;
        this->param2=param2;
        this->param3=param3;
        this->param4=param4;
        this->param5=param5;
        this->param6=param6;
    }
    void run(){(object->*method)(param1,param2,param3,param4,param5,param6);}
};


class OperationScheduler
{
private:
    bool working;
    std::vector<OperationBase*> operationVector;
    mutable boost::mutex _mutex;
public:
    OperationScheduler()
    {
        working=false;
    }

    void startOperation()
    {
        while (working);
        working = true;
    }

    void endOperation()
    {
        working = false;
    }

    void scheduleOperation(OperationBase* operation)
    {
        boost::mutex::scoped_lock lock(_mutex);
        operationVector.push_back(operation);
    }

    void run()
    {
        boost::mutex::scoped_lock lock(_mutex);
        for (int i = 0; i < operationVector.size(); i++)
            operationVector.at(i)->run();
        operationVector.clear();
    }
    void runOnce()
    {
        boost::mutex::scoped_lock lock(_mutex);
        if (operationVector.size() > 0)
        {
            operationVector.at(0)->run();
            operationVector.erase(operationVector.begin());
        }
    }

};


#endif	/* OPERATIONSCHEDULER_H */

