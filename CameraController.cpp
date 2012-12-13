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

#include "CameraController.h"

CameraController::CameraController(SceneManager* sceneManager,Camera* camera,int distance,int height)
{
    this->camera=camera;
    this->sceneManager=sceneManager;
    this->horizontalNode=NULL;
    this->saveOrientation=true;
    this->height=height;
    this->distance=distance;
	freecam=false;
	targetX=Quaternion::IDENTITY;
	targetY=Quaternion::IDENTITY;
}

void CameraController::clearNodes()
{
    if(horizontalNode!=NULL && verticalNode!=NULL && cameraNode!=NULL)
    {
        saveCurrentOrientation();
        camera->getParentSceneNode()->detachAllObjects();
        cameraNode->removeAndDestroyAllChildren();
        horizontalNode->removeAndDestroyAllChildren();
		verticalNode->removeAndDestroyAllChildren();
		sceneManager->destroySceneNode(verticalNode);
    }
}

void CameraController::resetOrientation()
{
    if(horizontalNode!=NULL && verticalNode!=NULL)
    {
        horizontalNode->resetOrientation();
        verticalNode->resetOrientation();
    }
}

void CameraController::setTarget(SceneNode* targetNode)
{
    clearNodes();
    this->targetNode=targetNode;
    verticalNode=targetNode->createChildSceneNode("verticalNode");
    verticalNode->setInheritOrientation(false);
    horizontalNode=verticalNode->createChildSceneNode("horizontalNode");
    cameraNode=horizontalNode->createChildSceneNode("cameraNode");
    cameraNode->attachObject(camera);
    cameraNode->setPosition(0,height,distance);
    
    if(saveOrientation)
    {
        horizontalNode->setOrientation(lastHorizontalOrientation);
        verticalNode->setOrientation(lastVerticalOrientation);
    }
	freecam=false;
}

void CameraController::enableFreecam()
{
    clearNodes();
	verticalNode=sceneManager->getRootSceneNode()->createChildSceneNode("verticalNode");
    //verticalNode->setInheritOrientation(false);
    horizontalNode=verticalNode->createChildSceneNode("horizontalNode");
    cameraNode=horizontalNode->createChildSceneNode("cameraNode");
    cameraNode->attachObject(camera);
	freecam=true;
}

void CameraController::rotateCamera(float relX, float relY)
{
    if(horizontalNode!=NULL && verticalNode!=NULL)
    {
        if(horizontalNode->getOrientation().getPitch(false).valueDegrees()>-45+relY && horizontalNode->getOrientation().getPitch(false).valueDegrees()<30+relY)
            horizontalNode->rotate(Ogre::Vector3(1,0,0),(Radian)(relY*-3.14/180));
        verticalNode->rotate(Ogre::Vector3(0,1,0),(Radian)(relX*-3.14/180));
    }
}

void CameraController::setOrientationInheritance(bool value)
{
    cameraNode->setInheritOrientation(value);
}

Quaternion CameraController::getHorizontalOrientation()
{
    Quaternion quaternion;
    if(horizontalNode!=NULL)
        quaternion=horizontalNode->getOrientation();
    return quaternion;
}


Quaternion CameraController::getVerticalOrientation()
{
    Quaternion quaternion;
    if(verticalNode!=NULL)
        quaternion=verticalNode->getOrientation();
    return quaternion;
}

void CameraController::setZoom(float zoomValue)
{
    if(cameraNode!=NULL)
        cameraNode->translate(0,0,-zoomValue);
}

void CameraController::moveCamera(float relX,float relZ)
{
	if(cameraNode!=NULL)
	{
		if(!freecam)
			cameraNode->translate(relX,0,relZ);
		else
		{
			float relY=0;
			if(relX==0)
				relY=horizontalNode->getOrientation().getPitch(false).valueRadians();
			relY*=(relZ>0?-1:1);
			verticalNode->translate(relX,relY,relZ,Ogre::Node::TS_LOCAL);
		}
	}
}

float CameraController::getSlerpXDiff()
{
    return targetY.getYaw().valueDegrees()-verticalNode->getOrientation().getYaw().valueDegrees();
}

bool CameraController::isFreecam()
{
	return freecam;
}

void CameraController::resetZoom()
{
    if(cameraNode!=NULL)
        cameraNode->setPosition(0,height,distance);
}

float CameraController::getZoom()
{
	if(cameraNode!=NULL)
		return cameraNode->getPosition().z;
}

void CameraController::slerpCamera(float relX, float relY)
{
    targetX = targetX * Quaternion((Radian) (relY*-3.14 / 180), Ogre::Vector3::UNIT_X);
    targetY = targetY * Quaternion((Radian) (relX*-3.14 / 180), Ogre::Vector3::UNIT_Y);
}

void CameraController::updateCameraSlerp(float time)
{
    if (!isFreecam())
    {
        Quaternion interX = Quaternion::Slerp(.2, horizontalNode->getOrientation(), targetX);
        Quaternion interY = Quaternion::Slerp(.2, verticalNode->getOrientation(), targetY);
        horizontalNode->setOrientation(interX);
        verticalNode->setOrientation(interY);
    }
}

void CameraController::setSaveOrientation(bool flag)
{
    saveOrientation=flag;
}

void CameraController::saveCurrentOrientation()
{
    lastHorizontalOrientation=getHorizontalOrientation();
    lastVerticalOrientation=getVerticalOrientation();
}

SceneNode* CameraController::getCameraNode()
{
	return cameraNode;
}