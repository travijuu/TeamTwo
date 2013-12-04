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

#include "MapEditor.h"

#define DEFAULT_SCALE_FACTOR 1000
#define SPAWN_POINT_LIMIT 5
#define WIDTH 800
#define HEIGHT 600
#define ORTHO 1


MapEditor::MapEditor(QWidget *parent, QMainWindow *main) :
    QGLWidget(parent)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    orthoValue = 1;
    ortho = 1;
    timer.start(50);
    screenX = WIDTH;
    screenY = HEIGHT;
    objType = 100;
    totalItemNumber = 0;
    isClicked = false;

    strcpy(header.name,HEADER_NAME);
    header.fileSignature = FILE_SIGNATURE;
    header.entity4Count = -1;
    header.scaleFactor = DEFAULT_SCALE_FACTOR;

    setMouseTracking(true);
    this->main = main;
    texScale =  1/30.0;
    degree = -90;
    imgScale = 0.4;
    zoomFactor = 100;
    posX=posY=0;
}

void MapEditor::initializeGL()
{
    loadTexture(ItemType::LASER_PISTOL, "images/pistol2.bmp");
    loadTexture(ItemType::LASER_MACHINE_GUN, "images/laser.bmp");

    loadTexture(ItemType::EXTRA_HEALTH, "images/health.bmp");
    loadTexture(ItemType::SPEED_BOOSTER, "images/run.bmp");

    loadTexture(MapEntityType::SPAWN_POINT, "images/spawn.bmp");
    loadTexture(MapEntityType::LIGHT_POINT, "images/light.bmp");
    loadTexture(MapEntityType::MAP_TELEPORT, "images/teleport.bmp");
    loadTexture(MapEntityType::MAP_WALL, "images/wall.bmp");

    loadTexture(ItemType::EAGLE_EYE, "images/EagleEye.bmp");
    loadTexture(ItemType::IMMORTALITY, "images/immortality.bmp");
    loadTexture(ItemType::ONE_SHOT, "images/one_shot.bmp");

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;
}

void MapEditor::keyPressEvent(QKeyEvent *event)
{
    // std::cout<<event->key()<<std::endl;

}
void MapEditor::mouseMoveEvent(QMouseEvent *event)
{
    currentX = convertCoordinate(event->pos().x(),0);
    currentY = convertCoordinate(event->pos().y(),1);


    ((MainWindow*)main)->setStatus(event->pos().x()-posX,event->pos().y()-posY);

}
void MapEditor::wheelEvent(QWheelEvent *wheel)
{
    std::cout<<wheel->delta()<<std::endl;
    float ratio = .1;

    if(zoomFactor<=100)
    {
        zoomFactor=100;
        if(wheel->delta()!=120)
        return;
    }
    if(zoomFactor>=200)
    {
        zoomFactor=200;
        if(wheel->delta() != -120)
        return;
    }

    if(wheel->delta() == 120) // zoom out
    {
        ortho -= ratio;
        orthoValue +=ratio;
        zoomFactor+=10;

    }
    else // zoom in
    {
        zoomFactor-=10;
        ortho += ratio;
        orthoValue -=ratio;
    }

    glOrtho(-ortho+posX, ortho-posX, -ortho+posY, ortho-posY, -1, 1);
    std::cout<<"Zoom: "<<zoomFactor<<std::endl;
}

void MapEditor::mousePressEvent(QMouseEvent *event)
{
    startX = convertCoordinate(event->pos().x(),0);
    startY = convertCoordinate(event->pos().y(),1);
    
    if(event->button() == 4)
    {
        move = true;
        return;
    }
    if( objType >= 100 && objType <= 102)
    {
      findClosestEntity(&startX,&startY);
    }
    if(event->button()==2)
    {
        discoverAndDelete(startX,startY);
        return;
    }
    isClicked = true;
    if( objType == MapEntityType::MODEL )
    {
        strcpy(modelEntity.modelName,modelName.data());
        modelEntity.type = objType;
        modelEntity.entityPosition.startX = startX;
        modelEntity.entityPosition.startZ = startY;
        modelEntity.direction = degree;
        addModelEntity(modelEntity);
        ((MainWindow*)main)->setWindowModified(true);
    }
    if( objType == MapEntityType::SPAWN_POINT || objType == MapEntityType::LIGHT_POINT || objType < 100  )
    {
        entity2.type = objType;
        entity2.entityPosition.startX = startX;
        entity2.entityPosition.startZ = startY;
        entity2.direction = degree;
        std::cout<<degree<<std::endl;
        addEntity2(entity2);
        ((MainWindow*)main)->setWindowModified(true);
    }

}

void MapEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == 2)
        return;
    if(event->button() == 4)
    {
        move = false;
        return;
    }
    if(isClicked)
    {
        isClicked = false;

        if( objType == MapEntityType::SPAWN_POINT || objType == MapEntityType::LIGHT_POINT || objType < 100 || objType == MapEntityType::MODEL )
            return;
        if( startX == convertCoordinate(event->pos().x(),0) || startY == convertCoordinate(event->pos().y(),1) )
            return;

        entity4.type = objType;
        entity4.entityPosition.startX = startX;
        entity4.entityPosition.startZ = startY;

        float x1 = convertCoordinate( event->pos().x(),0);
        float y1 = convertCoordinate( event->pos().y(),1);
        findClosestEntity(&x1,&y1);
        if(!space)
        {
            if( fabs(startX-convertCoordinate(event->pos().x(),0)) > fabs(startY-convertCoordinate(event->pos().y(),1)) )
            {
                entity4.entityPosition.endZ = entity4.entityPosition.startZ;
                entity4.entityPosition.endX = x1;
            }
            else
            {
                entity4.entityPosition.endX = entity4.entityPosition.startX;
                entity4.entityPosition.endZ = y1;
            }
        }
        else
        {
            entity4.entityPosition.endX = x1;
            entity4.entityPosition.endZ = y1;
        }
        addEntity4(entity4);
    }
    ((MainWindow*)main)->setWindowModified(true);
}

void MapEditor::paintGL()
{

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT );
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(-posX,-posY,0);

    drawCoordinateSystem(30);
    glScalef(orthoValue,orthoValue,orthoValue);
    showEntities();

    if(isClicked && !( objType == MapEntityType::SPAWN_POINT || objType == MapEntityType::LIGHT_POINT || objType < 100 || objType == MapEntityType::MODEL ) )
    {
        glPushMatrix();
        glLineWidth(3);
        glColor3f(1,1,1);
        glBegin(GL_LINES);
        glVertex2f(startX,startY);
        if(!space)
        {
            if( fabs(startX-currentX) < fabs(startY-currentY) )
                glVertex2f(startX,currentY);
            else
                glVertex2f(currentX,startY);

        }
        else
            glVertex2f(currentX,currentY);
        glEnd();
        glLineWidth(1);
        glPopMatrix();
    }

    if(move)
    {
        if(startX-currentX == 0 || startY-currentY == 0)
        {
            posX = 0;
            posY = 0;
            return;
        }
        if(startX-currentX == 0 || startY-currentY == 0)
            return;
        posX = (startX-currentX) / ortho;
        posY = (startY-currentY) / ortho;
        std::cout<<"PosX: "<<posX<<std::endl;

        glOrtho(-ortho+posX, ortho-posX, -ortho+posY, ortho-posY, -1, 1);

    }

}

void MapEditor::resizeGL(int w, int h)
{
    std::cout<<"W: "<<w<<" H: "<<h<<std::endl;

    updateScreenResolution(w,h);
    glLoadIdentity();
    glViewport( 0,0,w,h) ;


    std::cout<<imgScale<<std::endl;

    glMatrixMode( GL_PROJECTION );


    glOrtho( -ortho, ortho, -ortho, ortho, -1, 1);
    glMatrixMode( GL_MODELVIEW );


}

MapEditor::~MapEditor()
{
    mapEntity4Vector.clear();
}

float MapEditor::convertCoordinate(int pos, int direction)
{
    int currentCoorinate = direction == 0 ? screenX : screenY;
    float newPos = ( ortho * pos* 1.0 / ( currentCoorinate / 2 )  - ortho );

    if(direction)
        return newPos * -1;
    return newPos;
}

void MapEditor::showEntities()
{
    int size = mapEntity4Vector.size();
    for(int i=0;i<size;i++)
    {
        if(mapEntity4Vector.at(i).type == MapEntityType::MAP_WALL)
            glColor3d(1,1,0);
        if(mapEntity4Vector.at(i).type == MapEntityType::HIDDEN_GATE_WALL)
            glColor3d(0.7,0,0.7);
        if(mapEntity4Vector.at(i).type == MapEntityType::MAP_TELEPORT)
            glColor3d(0.3,0.5,0.3);


        glPushMatrix();
        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
        glVertex2f(mapEntity4Vector.at(i).entityPosition.startX,mapEntity4Vector.at(i).entityPosition.startZ);
        glVertex2f(mapEntity4Vector.at(i).entityPosition.endX,mapEntity4Vector.at(i).entityPosition.endZ);
        glEnd();
        glLineWidth(1);
        glPopMatrix();

        if(mapEntity4Vector.at(i).type == MapEntityType::MAP_TELEPORT)
        {
            drawImage(mapEntity4Vector.at(i).type, mapEntity4Vector.at(i).entityPosition.startX,mapEntity4Vector.at(i).entityPosition.startZ);
            drawImage(mapEntity4Vector.at(i).type, mapEntity4Vector.at(i).entityPosition.endX,mapEntity4Vector.at(i).entityPosition.endZ);
        }

        
    }

    size = mapEntity2Vector.size();
    for(int i=0;i<size;i++)
    {
        if( mapEntity2Vector.at(i).type == MapEntityType::SPAWN_POINT || mapEntity2Vector.at(i).type == MapEntityType::LIGHT_POINT )
        {
            glPushMatrix();
            glLineWidth(3);
            glColor3f(0,1,0);
            glTranslatef(mapEntity2Vector.at(i).entityPosition.startX,mapEntity2Vector.at(i).entityPosition.startZ,0);
            glRotatef(mapEntity2Vector.at(i).direction,0,0,1);
            glBegin(GL_LINE_LOOP);
            glVertex2f(0,0);
            glVertex2f(.05,0);
            glEnd();
            glLineWidth(1);
            glPopMatrix();
        }
        drawImage(mapEntity2Vector.at(i).type, mapEntity2Vector.at(i).entityPosition.startX,mapEntity2Vector.at(i).entityPosition.startZ);
    }

    size = modelEntityVector.size();
    for(int i=0;i<size;i++)
    {
        drawImage(QString(modelEntityVector.at(i).modelName).toStdString(), modelEntityVector.at(i).entityPosition.startX,modelEntityVector.at(i).entityPosition.startZ,modelEntityVector.at(i).direction);
    }
}

void MapEditor::addEntity4(MapEntity4_t entity)
{
    mapEntity4Vector.push_back(entity);
    mapHistoryList[totalItemNumber] = 0;
    totalItemNumber++;
}

void MapEditor::addEntity2(MapEntity2_t entity)
{
    mapEntity2Vector.push_back(entity);
    mapHistoryList[totalItemNumber] = 1;
    totalItemNumber++;
}
void MapEditor::addModelEntity(ModelEntity_t entity)
{
    modelEntityVector.push_back(entity);
    mapHistoryList[totalItemNumber] = 2;
    totalItemNumber++;
}

void MapEditor::discoverAndDelete(float x, float y)
{
    std::vector<MapEntity4_t>::iterator itr;
    std::vector<MapEntity2_t>::iterator itr2;
    std::vector<ModelEntity_t>::iterator itr3;

    for(itr = mapEntity4Vector.begin();itr < mapEntity4Vector.end();itr++)
    {
        float left = (itr->entityPosition.endZ - itr->entityPosition.startZ) * ( x - itr->entityPosition.startX );
        float right = (itr->entityPosition.endX - itr->entityPosition.startX) * ( y - itr->entityPosition.startZ );
        
        if( fabs(fabs(left) - fabs(right)) <= 0.003 )
        {
            if( (fabs(x)>=fabs(itr->entityPosition.startX) && fabs(x)<= fabs(itr->entityPosition.endX)) || ( fabs(x)<=fabs(itr->entityPosition.startX) && fabs(x)>= fabs(itr->entityPosition.endX)) )
                mapEntity4Vector.erase(itr);
        }
    }

    for(itr2=mapEntity2Vector.begin();itr2<mapEntity2Vector.end();itr2++)
    {
        float curX = fabs(itr2->entityPosition.startX - x); // 6
        float curY = fabs(itr2->entityPosition.startZ - y);  // 8
        float hipo = sqrt(curX*curX + curY*curY); // 10

        if( hipo <= 0.03)
            mapEntity2Vector.erase(itr2);
    }

    for(itr3=modelEntityVector.begin();itr3<modelEntityVector.end();itr3++)
    {
        float curX = fabs(itr3->entityPosition.startX - x); // 6
        float curY = fabs(itr3->entityPosition.startZ - y);  // 8
        float hipo = sqrt(curX*curX + curY*curY); // 10

        if( hipo <= 0.03)
            modelEntityVector.erase(itr3);
    }
}

void MapEditor::saveToFile( std::string fileName )
{
    std::ofstream file;
    file.open( fileName.c_str(), std::ios::out | std::ios::binary );
    
    header.entity4Count = mapEntity4Vector.size();
    header.entity2Count = mapEntity2Vector.size();
    header.modelCount = modelEntityVector.size();

    file.write( (char *)&header,sizeof(MapHeader));
    for(int i=0;i<mapEntity4Vector.size();i++)
        file.write((char *)&mapEntity4Vector.at(i),sizeof(MapEntity4));

    for(int i=0;i<mapEntity2Vector.size();i++)
        file.write((char *)&mapEntity2Vector.at(i),sizeof(MapEntity2));

    for(int i=0;i<modelEntityVector.size();i++)
        file.write((char *)&modelEntityVector.at(i),sizeof(ModelEntity));

    file.close();
}
void MapEditor::loadMap(std::string fileName)
{
    cleanMap();
    std::ifstream file;
    file.open(fileName.c_str(), std::ios::in | std::ios::binary );
    if(file.is_open() && !file.eof())
    {
        MapHeader header;
        MapEntity4_t ent4;
        MapEntity2_t ent2;
        ModelEntity_t model;

        file.read((char*)&header,sizeof(MapHeader));
        std::cout<<header.entity4Count<<std::endl;
        for(int i=0;i<header.entity4Count&&!file.eof();i++)
        {
            file.read((char*)(&ent4),sizeof(MapEntity4_t));
            mapEntity4Vector.push_back(ent4);
        }
        for(int i=0;i<header.entity2Count&&!file.eof();i++)
        {
            file.read((char*)(&ent2),sizeof(MapEntity2_t));
            mapEntity2Vector.push_back(ent2);
        }
        for(int i=0;i<header.modelCount&&!file.eof();i++)
        {
            file.read((char*)(&model),sizeof(ModelEntity_t));
            modelEntityVector.push_back(model);
        }
        this->header = header;
    }

    file.close();
}
bool verticalOrHorizontal(MapEntity4_t mapEntity)
{
    if( fabs(mapEntity.entityPosition.startX-mapEntity.entityPosition.endX) > fabs(mapEntity.entityPosition.startZ-mapEntity.entityPosition.endZ) )
        return true; // vertical X
    return false; // horizontal Z
}

void MapEditor::drawCoordinateSystem(int number)
{
    glColor3d(0,0,1);

    for(float i=-orthoValue;i<=orthoValue;i+=2*orthoValue/(screenX/number*1.0) )
    {
        glBegin(GL_LINES);
        glVertex2f(i,orthoValue);
        glVertex2f(i,-orthoValue);
        glEnd();
    }
    for(float i=-orthoValue;i<=orthoValue;i+=2*orthoValue/(screenY/number*1.0) )
    {
        glBegin(GL_LINES);
        glVertex2f(orthoValue,i);
        glVertex2f(-orthoValue,i);
        glEnd();
    }
    
    glColor3d(1,0,0);
    glBegin(GL_LINES);
    glVertex2f(0,orthoValue);
    glVertex2f(0,-orthoValue);

    glVertex2f(-orthoValue,0);
    glVertex2f(orthoValue,0);
    glEnd();
}

void MapEditor::circle( float x, float y, float r )
{
#define PI 3.1415
    float angle ;

    glBegin( GL_POLYGON ) ;
    for ( int i = 0 ; i < 100 ; i++ )
    {
        angle = 2*PI*i/100;
        glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
    }
    glEnd();
}

void MapEditor::cleanMap()
{
    mapEntity4Vector.clear();
    mapEntity2Vector.clear();
    modelEntityVector.clear();
}

void MapEditor::undo()
{
    std::map<int,int>::iterator itr;
    itr = mapHistoryList.find(totalItemNumber-1);

    if( itr->second == 1)
    {
        if( mapEntity2Vector.size() > 0 )
            mapEntity2Vector.erase(mapEntity2Vector.end()-1);
    }
    else if (itr->second == 0)
    {
        if( mapEntity4Vector.size() > 0 )
            mapEntity4Vector.erase(mapEntity4Vector.end()-1);
    }
    else if(itr->second == 2)
    {
        if( modelEntityVector.size() >0 )
            modelEntityVector.erase(modelEntityVector.end()-1);
    }
    if(mapHistoryList.size()>0)
    {
        totalItemNumber--;
        mapHistoryList.erase(totalItemNumber);
    }
}

void MapEditor::updateScreenResolution(int x, int y)
{
    screenX = x;
    screenY = y;
}

void MapEditor::findClosestEntity(float* x, float* y)
{
    int size = mapEntity4Vector.size();
    float gap = 0.015;
    for(int i=0;i<size;i++)
    {
        if( ( mapEntity4Vector.at(i).entityPosition.startX <= *x + gap && mapEntity4Vector.at(i).entityPosition.startX >= *x - gap ) &&
                ( mapEntity4Vector.at(i).entityPosition.startZ <= *y + gap && mapEntity4Vector.at(i).entityPosition.startZ >= *y - gap ) )
        {
            *x = mapEntity4Vector.at(i).entityPosition.startX  ;
            *y = mapEntity4Vector.at(i).entityPosition.startZ  ;
        }

        if( ( mapEntity4Vector.at(i).entityPosition.endX <= *x + gap && mapEntity4Vector.at(i).entityPosition.endX >= *x - gap ) &&
                ( mapEntity4Vector.at(i).entityPosition.endZ <= *y + gap && mapEntity4Vector.at(i).entityPosition.endZ >= *y - gap ) )
        {
            *x = mapEntity4Vector.at(i).entityPosition.endX ;
            *y = mapEntity4Vector.at(i).entityPosition.endZ ;
        }

    }
}

void MapEditor::setScaleFactor(int value)
{
    header.scaleFactor = value;
}
int MapEditor::getScaleFactor()
{
    return header.scaleFactor;
}

void MapEditor::setObjectType(int type)
{
    objType = type;
    isClicked = false;
}

void MapEditor::setModelName(std::string name)
{
    modelName = name;
}

void MapEditor::setDrawStraight(bool value)
{
    space=value;
}

void MapEditor::loadTexture(int texId, const char *path)
{
    textureList[texId] = LoadTexture(path);
    std::cout<<"WIDTH:"<<getTextureWidth(texId)<<std::endl;

}

void MapEditor::loadModelTexture(std::string name, const char *path, float scaleFactor)
{
    modelTextureList[name] = LoadTexture(path);
    modelScaleFactorList[name] = scaleFactor;
    std::cout<<name<<"-"<<path<<"-"<<modelTextureList[name]<<"-"<<scaleFactor<<std::endl;
}

void MapEditor::drawImage(int type, float x, float z)
{
    glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    glTranslatef(x,z,0);

    glTranslatef(-imgScale/20,-imgScale/20,0);
    glScalef(imgScale,imgScale,imgScale);
    glBindTexture(GL_TEXTURE_2D,textureList[type]);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glTexCoord2f(0,1);
    glVertex2f(0,.1);

    glTexCoord2f(0,0);
    glVertex2f(0,0);

    glTexCoord2f(1,0);
    glVertex2f(.1,0);

    glTexCoord2f(1,1);
    glVertex2f(.1,.1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND) ;
    glDepthMask(GL_TRUE) ;

    glPopMatrix();
}
void MapEditor::drawImage(std::string name, float x, float z, int direction)
{
    glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND) ;
    glDepthMask(GL_FALSE) ;

    modelScale = modelScaleFactorList[name];

    glTranslatef(x,z,0);
    glRotatef(direction+90,0,0,1);

    glTranslatef(-modelScale /2,-modelScale /2,0);
    glScalef(modelScale ,modelScale ,modelScale );

    glBindTexture(GL_TEXTURE_2D,modelTextureList[name]);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(0,0);


    glTexCoord2f(1,0);
    glVertex2f(1,0);

    glTexCoord2f(1,1);
    glVertex2f(1,1);

    glTexCoord2f(0,1);
    glVertex2f(0,1);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND) ;
    glDepthMask(GL_TRUE) ;

    glPopMatrix();
}

void MapEditor::drawImageLine(int type, float startX, float startZ, float endX, float endZ)
{
    glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND) ;
    glDepthMask(GL_FALSE) ;

    if(startX>endX)
    {
        float tmp = startX;
        startX = endX;
        endX = tmp;
    }
    glBindTexture(GL_TEXTURE_2D,textureList[type]);
    glColor3f(1,1,1);
    for(float i= startX+texScale/2; i<endX; i+=texScale)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex2f(i-texScale/2,startZ+texScale/2);

        glTexCoord2f(0,0);
        glVertex2f(i-texScale/2,startZ-texScale/2);

        glTexCoord2f(1,0);
        glVertex2f(i+texScale/2,startZ-texScale/2);

        glTexCoord2f(1,1);
        glVertex2f(i+texScale/2,startZ+texScale/2);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND) ;
    glDepthMask(GL_TRUE) ;

    glPopMatrix();
}

void MapEditor::setMainWindow(QMainWindow *main)
{
    this->main = main;
}

void MapEditor::setDegree(int val)
{
    degree = val;

    if( mapHistoryList[totalItemNumber-1] == 0)
        return;
    if( mapHistoryList[totalItemNumber-1] == 1 )
        mapEntity2Vector.at(mapEntity2Vector.size()-1).direction = degree;
    if( mapHistoryList[totalItemNumber-1] == 2 )
        modelEntityVector.at(modelEntityVector.size()-1).direction = degree;
}
bool MapEditor::isFileProperToSave()
{
    int counter = 0;
    for(int i=0;i<mapEntity2Vector.size();i++)
    {
        if( mapEntity2Vector.at(i).type == MapEntityType::SPAWN_POINT )
            counter++;
    }

    if(counter>=SPAWN_POINT_LIMIT)
        return true;
    else
        return false;
}
