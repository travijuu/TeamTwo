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

#include "AnimationController.h"

AnimationController::AnimationController(Entity* entity)
{
    if(entity!=NULL)
    {
        this->entity=entity;
        initialize();
    }
}

void AnimationController::initialize()
{
    AnimationStateSet* set=entity->getAllAnimationStates();
    AnimationStateIterator it=set->getAnimationStateIterator();
    AnimationState* state;
    AnimationInfo_t* info;
    while(it.hasMoreElements())
    {
        state=(AnimationState*)it.getNext();
        info=new AnimationInfo;
        info->enabled=false;
        info->loop=false;
        info->speed=1;
        info->state=state;
        info->playTillEnd=false;
        info->oneFrame=false;
        info->idleState=NULL;
        info->autoDisable=false;
        animationMap[state->getAnimationName()]=info;
    }
}

void AnimationController::enableAnimation(std::string animationName)
{
    if(animationMap.count(animationName)>0)
    {
        AnimationState* state=entity->getAnimationState(animationName);
        AnimationInfo_t* info=animationMap[animationName];
        info->enabled=true;
        state->setEnabled(true);
    }    
}

void AnimationController::disableAnimation(std::string animationName)
{
    if(animationMap.count(animationName)>0)
    {
        AnimationState* state=entity->getAnimationState(animationName);
        AnimationInfo_t* info=animationMap[animationName];
        info->enabled=false;
        state->setEnabled(false);
        state->setTimePosition(0);
    }
}

void AnimationController::updateOneFrame(std::string animationName, double speed,bool loop)
{
    if(animationMap.count(animationName)>0)
    {
        AnimationState* state=entity->getAnimationState(animationName);
        AnimationInfo_t* info=animationMap[animationName];
        info->speed=speed;
        info->oneFrame=true;
        state->setLoop(loop);
    }
}

void AnimationController::playTillEnd(std::string animationName,double speed,bool autoDisable)
{
    if(animationMap.count(animationName)>0)
    {
        animationMap[animationName]->playTillEnd=true;
         animationMap[animationName]->autoDisable=autoDisable;
        animationMap[animationName]->speed=speed;
        animationMap[animationName]->state->setTimePosition(0);
		animationMap[animationName]->state->setLoop(false);
    }
}

void AnimationController::updateAnimations(double updateTime)
{
    AnimationState* state;
    bool resetAnimation=false;
    bool up;
    double speed;
    double animationLength;

    AnimationInfo* info;
    std::map<std::string,AnimationInfo_t*>::iterator iter=animationMap.begin();
    while(iter!=animationMap.end())
    {
        info=iter->second;
        if(info->enabled)
        {
            state=info->state;
            speed=info->speed;
            animationLength = state->getLength();


			if(info->playTillEnd)
                        {
                            if(state->getTimePosition()+speed<=state->getLength())
				state->addTime(info->speed);
                            else if(info->autoDisable)
                                state->setEnabled(false);

                        }

			else
			{
				if(info->oneFrame)
                                {
					state->addTime(info->speed);
                                        if(info->idleState!=NULL)
                                        {
                                            if(info->idleState->getWeight()>0)
                                                info->idleState->setWeight(0);
                                        }
                                }
                                else//reset
				{
					speed=fabs(speed);
                                        if(info->idleState!=NULL)
                                        {
                                            if(info->idleState->getWeight()<4)
                                                info->idleState->setWeight(info->idleState->getWeight()+.2);
                                        }
                                        
 

					up = false;
					if (state->getTimePosition() >= animationLength / 2.0)
						up = !up;

					if (up)
						state->addTime(speed);
					else
						state->addTime(-speed);
					state->setLoop(false);
				}
			}
        }

        info->oneFrame=false;
        iter++;
    }
}

double AnimationController::getAnimationTime(std::string animationName)
{
    return entity->getAnimationState(animationName)->getTimePosition();
}

double AnimationController::getAnimationLength(std::string animationName)
{
    return entity->getAnimationState(animationName)->getLength();
}

AnimationState* AnimationController::getAnimationState(std::string animationName)
{
    return animationMap[animationName]->state;
}

void AnimationController::addIdleAnimation(std::string sourceAnimation,std::string subAnimation)
{
    AnimationInfo_t* info=animationMap[sourceAnimation];
    info->idleState=animationMap[subAnimation]->state;
}

