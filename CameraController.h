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

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H
#include "stdafx.h"

/* Camera Controller
 *
 * Camera controller provides a third person
 * shooter camera to its users. It uses
 * Ogre3D's high level camera functions to
 * create a TPS look. It also provides a
 * free camera ability to fly around the scene.
 */

class CameraController
{
public:
    //constructor of the class. Takes the Ogre::SceneManager
    //and main camera instance to build a third-person-camera
    //system. distance and height has their own default values.
    CameraController(SceneManager* sceneManager, Camera* camera, int distance = 100, int height = 300);
    //returns the Y axis orientation
    Quaternion getVerticalOrientation();
    //returns the X axis orientation
    Quaternion getHorizontalOrientation();
    //resets the current orientation of the camera
    void resetOrientation();
    //returns true if freecam mode has been activated
    bool isFreecam();
    //sets the target of the TPS camera to be locked
    void setTarget(SceneNode* targetNode);
    //enables the free camera and releases the target object
    void enableFreecam();
    //
    void setOrientationInheritance(bool value);
    //rotates the camera according to the given relative
    //rotation values. Values must be Radian.
    void rotateCamera(float relX, float relY);
    //changes the position of the camera. generally, used in
    //the freecam mode.
    void moveCamera(float relX, float relZ);
    //provides a smooth rotation to the camera by interpolating
    //the angle instead of rotating it directly.
    void slerpCamera(float relX, float relY);
    //continues to interpolate the slerp value if any.
    //this method has to be called at the every frame of the game.
    //if slerpCamera() is not called, then it is not required
    void updateCameraSlerp(float time);
    //zooms to the target according to the given value
    void setZoom(float zoomValue);
    //returns the current zoom value of the camera
    float getZoom();
    //resets the zoom
    void resetZoom();
    //if it is desired to save the orientation of the
    //camera and reload it after switching to
    //the TPS system from freecam, "true" must be passed.
    void setSaveOrientation(bool flag);
    //returns the Ogre::SceneNode of the camera object
    SceneNode* getCameraNode();
    //returns the diff. between target and current orientation according to x-axis
    float getSlerpXDiff();
private:
    Camera* camera;
    bool saveOrientation;
    bool freecam;
    SceneNode* cameraNode;
    SceneNode* verticalNode;
    SceneNode* horizontalNode;
    SceneNode* targetNode;
    SceneManager* sceneManager;
    Quaternion lastHorizontalOrientation;
    Quaternion lastVerticalOrientation;
    int distance;
    int height;
    Quaternion targetX;
    Quaternion targetY;
    //clears the necessary nodes used to create the
    //TPS camera system and releases the camera
    void clearNodes();
    //saves the current orientation of the camera
    //valid only if the setSaveOrientation() was called
    //with "true" parameter
    void saveCurrentOrientation();

};

#endif
