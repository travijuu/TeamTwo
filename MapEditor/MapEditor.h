/*
	Copyright (C) 2011-2012 Erkin Cakar

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

#ifndef _MAPEDITOR_H
#define	_MAPEDITOR_H


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "MapStructure.h"
#include "ItemStructure.h"
#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <GL/glut.h>
#include <GL/glu.h>
#include "texture.h"
#include "mainwindow.h"

#include <QMainWindow>


class MapEditor : public QGLWidget
{
private:
    std::vector<MapEntity4_t> mapEntity4Vector;
    std::vector<MapEntity2_t> mapEntity2Vector;
    std::vector<ModelEntity_t> modelEntityVector;
    std::map<int, int> textureList;
    std::map<std::string, int> modelTextureList;
    std::map<std::string, float> modelScaleFactorList;
    std::map<int, int> mapHistoryList;

    QTimer timer;
    QMainWindow *main;
    MapHeader_t header;
    MapEntity2_t entity2;
    MapEntity4_t entity4;
    ModelEntity_t modelEntity;

    int screenX, screenY, objType, totalItemNumber, degree, textureCount, zoomFactor;
    float startX,startY,currentX, currentY,orthoValue,endX,endY, texScale, imgScale, ortho, posX, posY, modelScale;
    bool space,isClicked, move;
    std::string modelName;

public:
    MapEditor(QWidget *parent = 0, QMainWindow *main = NULL);
    ~MapEditor();
    float convertCoordinate(int pos, int direction);
    void showEntities();
    void addEntity4(MapEntity4_t entity);
    void addEntity2(MapEntity2_t entity);
    void addModelEntity(ModelEntity_t entity);
    void discoverAndDelete(float x, float y);
    void saveToFile( std::string fileName );
    void drawCoordinateSystem(int number);
    void circle( float x, float y, float r );
    void cleanMap();
    void undo();
    void updateScreenResolution(int x, int y);
    void findClosestEntity(float *x, float *y);
    void loadMap(std::string file);
    void setScaleFactor(int value);
    int getScaleFactor();
    bool isFileProperToSave();
    void setMainWindow(QMainWindow *main);
    void setDegree(int );
    void setObjectType(int type);
    void setModelName(std::string name);
    void setDrawStraight(bool);
    void loadTexture(int, const char *);
    void loadModelTexture(std::string name, const char *path, float scaleFactor);
    void drawImage(int type, float x, float z);
    void drawImage(std::string name, float x, float z, int direction);
    void drawImageLine(int type, float startX, float startZ, float endX, float endZ);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
};


#endif	/* _MAPEDITOR_H */

