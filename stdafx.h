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

#include <math.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#ifndef WINDOWS_PLATFORM
	#include <dirent.h>
#endif
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include "ExampleApplication.h"

 #include "OgreBulletDynamicsRigidBody.h"
 #include "Shapes/OgreBulletCollisionsBoxShape.h"
 #include "Shapes/OgreBulletCollisionsStaticPlaneShape.h"
 #include "Shapes/OgreBulletCollisionsCapsuleShape.h"
 #include "Shapes/OgreBulletCollisionsSphereShape.h"
 #include "Shapes/OgreBulletCollisionsCylinderShape.h"
 #include "Shapes/OgreBulletCollisionsConvexHullShape.h"
 #include "Shapes/OgreBulletCollisionsTriangleShape.h"
#include "Shapes/OgreBulletCollisionsTerrainShape.h"
#include <Utils/OgreBulletConverter.h>
#include <OgreBulletCollisionsPreRequisites.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>
#include <Shapes/OgreBulletCollisionsTriangleShape.h>
#include <Shapes/OgreBulletCollisionsTrimeshShape.h>

 #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 #define WIN32_LEAN_AND_MEAN
 #include "windows.h"
 #endif
